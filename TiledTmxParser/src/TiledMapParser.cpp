#include "TiledMapParser.h"
#include "TileMap.h"
#include <iostream>
#include <map>
#include <functional>
#include <sstream>
#include "Encoding/base64.h"
#include "miniz/miniz.h"

using namespace tinyxml2;

static std::vector<std::string> explode(const std::string& delimiter, const std::string& str, unsigned int reserve = 0);

TiledMapParser::TiledMapParser()
{

}

TiledMapParser::~TiledMapParser()
{

}

bool TiledMapParser::LoadFromFile(const std::string& filename, TileMap* map)
{
	XMLDocument doc;
	auto result = doc.LoadFile(filename.c_str());

	// bail out if we have not been able to load the file.
	if (result != XMLError::XML_SUCCESS)
		return false;

	auto mapElem = doc.RootElement();

	// Parse <Map> element attributes
	map->baseFilePath = filename.substr(0, filename.find_last_of("\\/")) + "/";
	map->version = mapElem->Attribute("version");
	map->orientation = OrientationFromString(mapElem->Attribute("orientation"));
	map->renderOrder = RenderOrderFromString(mapElem->Attribute("renderorder"));
	map->rows = mapElem->IntAttribute("height");
	map->cols = mapElem->IntAttribute("width");
	map->tileWidth = mapElem->IntAttribute("tilewidth");
	map->tileHeight = mapElem->IntAttribute("tileheight");


	// Create a lookup table of methods to call for each child of <Map>
	std::map<std::string, std::function<bool(XMLElement *elem)>> parseFnLookup;
	parseFnLookup["tileset"] = [&](auto elem) { 
		return TryParseTileSetElement(elem, map, map->tileSets);
	};
	parseFnLookup["layer"] = [&](auto elem) { 
		return TryParseLayerElement(elem, map, map->namedTileLayers);
	};
	parseFnLookup["objectgroup"] = [&](auto elem) { 
		return TryParseObjectGroupElement(elem, map, map->namedObjectLayers);
	};
	parseFnLookup["imagelayer"] = [&](auto elem) { 
		return TryParseImageLayerElement(elem, map);
	};
	parseFnLookup["group"] = [&](auto elem) {
		return TryParseGroupElement(elem, map);
	};
	parseFnLookup["editorsettings"] = [&](auto elem) {
		return TryParseEditorSettingsElement(elem, map);
	};
	parseFnLookup["properties"] = [&](auto elem) {
		return TryParsePropertiesElement(elem, map->properties);
	};

	// Iterate through each Map children elements
	auto childMapElem = mapElem->FirstChildElement();
	while (childMapElem != nullptr)
	{
		auto childElementName = childMapElem->Name();
		if (parseFnLookup[childElementName])
		{
			// do we have a parsing function setup in the lookup table above, call it...
			parseFnLookup[childElementName](childMapElem);
		}
		else
		{
			// no function was setup for the child element... log message
			std::cout << "Skipping unknown element: " << childElementName << std::endl;
		}
			
		// move to next map child element
		childMapElem = childMapElem->NextSiblingElement();
	}

	// We have finished loading...
	// populate cached tile data for each layer so that we dont have to do it at runtime.
	for (auto layer : map->layers)
	{
		if (layer->type == ILayer::LayerType::LAYER) {
			PopulateLayerTileData(map, (TileLayer*)layer);
		}
	}
	return true;
}

bool TiledMapParser::TryParseTileSetElement(tinyxml2::XMLElement* elem, TileMap *map, TileSetCollection& tileset)
{
	// Refer to documentation for tmx file <tileset> element
	// https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#tileset

	if (strcmp("tileset", elem->Name()) != 0)
		return false;

	int firstGlobalTileId = elem->IntAttribute("firstgid");

	tinyxml2::XMLDocument doc;

	// if the source element is set, then the data is stored in an external xml file.
	auto source = elem->Attribute("source");
	if (source != nullptr)
	{
		std::string tsxFile = map->baseFilePath + source;
		doc.LoadFile(tsxFile.c_str());
		elem = doc.FirstChildElement();
	}

	// ensure the tileset has the "name" attribute
	auto name = elem->Attribute("name");
	if (name == nullptr)
		return false;

	// Read <tileset> attributes
	tileset[name].name = name;
	tileset[name].firstGlobalTileId = firstGlobalTileId;
	tileset[name].tileWidth = elem->IntAttribute("tilewidth");
	tileset[name].tileHeight = elem->IntAttribute("tileheight");
	tileset[name].spacing = elem->IntAttribute("spacing");
	tileset[name].margin = elem->IntAttribute("margin");
	tileset[name].tileCount = elem->IntAttribute("tilecount");
	tileset[name].tilesPerRow = elem->IntAttribute("columns");
	tileset[name].tileOffsetX = 0;
	tileset[name].tileOffsetY = 0;

	// TODO: SUPPORT tileset objectAlignment attribute; tileset[name].objectAlignment = elem->Attribute("objectalignment");

	std::map<std::string, std::function<bool(XMLElement* elem)>> parseFnLookup;
	parseFnLookup["image"] = [&](auto elem) {
		return TryParseImageElement(elem, map, tileset[name]);
	};
	parseFnLookup["terraintypes"] = [&](auto elem) {
		// TODO: support tileset terraintypes
		return false;
	};
	parseFnLookup["tile"] = [&](auto elem) {
		
		return TryParseTileElement(elem, map, &tileset[name]);
	};
	
	auto childElem = elem->FirstChildElement();
	while (childElem != nullptr)
	{
		if (parseFnLookup[childElem->Name()])
		{
			// do we have a parsing function setup in the lookup table above, call it...
			parseFnLookup[childElem->Name()](childElem);
		}
		else
		{
			// no function was setup for the child element...
			std::cout << "Skipping unknown tileset element: " << childElem->Name() << std::endl;
		}
		childElem = childElem->NextSiblingElement();
	}

	return true;
}

bool TiledMapParser::TryParseTileElement(tinyxml2::XMLElement* elem, TileMap* map, TileSet* tileset)
{
	if (strcmp("tile", elem->Name()) != 0)
		return false;

	unsigned int localTileId = elem->IntAttribute("id");
	const char* szTileType = elem->Attribute("type");
	float probability = elem->FloatAttribute("probability", 0.0f);

	if (probability > 0.0f)
		tileset->properties[localTileId]["probability"].Set(probability);

	std::map<std::string, std::function<bool(XMLElement* elem)>> parseFnLookup;
	parseFnLookup["properties"] = [&](auto elem) {
		return TryParsePropertiesElement(elem, tileset->properties[localTileId]);
	};
	parseFnLookup["image"] = [&](auto elem) {
		// TODO:
		return false;
	};
	parseFnLookup["objectgroup"] = [&](auto elem) {
		// TODO:
		return false;
	};

	auto childElem = elem->FirstChildElement();
	while (childElem != nullptr)
	{
		if (parseFnLookup[childElem->Name()])
		{
			// do we have a parsing function setup in the lookup table above, call it...
			parseFnLookup[childElem->Name()](childElem);
		}
		else
		{
			// no function was setup for the child element...
			std::cout << "Skipping unknown tileset element: " << childElem->Name() << std::endl;
		}
		childElem = childElem->NextSiblingElement();
	}

	return true;
}


bool TiledMapParser::TryParseLayerAttributes(tinyxml2::XMLElement* elem, TileMap* map, const std::string& layerName, ILayer* layer)
{
	layer->name = layerName;
	layer->map = map;
	layer->id = elem->IntAttribute("id");
	layer->opacity = elem->FloatAttribute("opacity", 1.0f);
	layer->visible = elem->BoolAttribute("visible", true);
	layer->tintColor = ColorFromString(elem->Attribute("tintcolor"));
	layer->offsetX = elem->IntAttribute("offsetx");
	layer->offsetY = elem->IntAttribute("offsety");

	return true;
}

bool TiledMapParser::TryParseLayerElement(tinyxml2::XMLElement* elem, TileMap* map, NamedTileLayerCollection& layers)
{
	// Refer to documentation for tmx file <layer> element
	// https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#layer


	if (strcmp("layer", elem->Name()) != 0)
		return false;

	auto name = elem->Attribute("name");
	if (name == nullptr)
		return false;

	// load layer attributes
	TryParseLayerAttributes(elem, map, name, &layers[name]);
	layers[name].rows = elem->IntAttribute("height");
	layers[name].cols = elem->IntAttribute("width");
	
	std::map<std::string, std::function<bool(XMLElement* elem)>> parseFnLookup;
	parseFnLookup["data"] = [&](auto elem) {
		return TryParseDataElement(elem, layers[name], layers[name].rawTileData);
	};
	parseFnLookup["properties"] = [&](auto elem) {
		return TryParsePropertiesElement(elem, layers[name].properties);
	};

	auto childElem = elem->FirstChildElement();
	while (childElem != nullptr)
	{
		if (parseFnLookup[childElem->Name()])
		{
			// do we have a parsing function setup in the lookup table above, call it...
			parseFnLookup[childElem->Name()](childElem);
		}
		else
		{
			// no function was setup for the child element...
			std::cout << "Skipping unknown layer element: " << childElem->Name() << std::endl;
		}
		childElem = childElem->NextSiblingElement();
	}

	// Add the layer to the renderable list of layers
	map->layers.push_back(&layers[name]);

	return true;
}

bool TiledMapParser::TryParseObjectGroupElement(tinyxml2::XMLElement* elem, TileMap* map, NamedObjectGroupLayerCollection& layers)
{
	if (strcmp("objectgroup", elem->Name()) != 0)
		return false;

	auto name = elem->Attribute("name");
	if (name == nullptr)
		return false;

	// load attributes
	TryParseLayerAttributes(elem, map, name, &layers[name]);

	std::map<std::string, std::function<bool(XMLElement* elem)>> parseFnLookup;
	parseFnLookup["properties"] = [&](auto elem) {
		return TryParsePropertiesElement(elem, layers[name].properties);
	};
	parseFnLookup["object"] = [&](auto elem) {
		return TryParseObjectElement(elem, map, layers[name]);
	};

	auto childElem = elem->FirstChildElement();
	while (childElem != nullptr)
	{
		if (parseFnLookup[childElem->Name()])
		{
			// do we have a parsing function setup in the lookup table above, call it...
			parseFnLookup[childElem->Name()](childElem);
		}
		else
		{
			// no function was setup for the child element...
			std::cout << "Skipping unknown layer element: " << childElem->Name() << std::endl;
		}
		childElem = childElem->NextSiblingElement();
	}

	// Add the layer to the renderable list of layers
	map->layers.push_back(&layers[name]);


	return true;	
}

bool TiledMapParser::TryParseObjectElement(tinyxml2::XMLElement* elem, TileMap* map, ObjectGroupLayer& layer)
{
	if (strcmp("object", elem->Name()) != 0)
		return false;

	// TODO: Investigate
	// for reason unknown elem->IntAttribute seems to be failing for the below properties using std::atoi instead
	auto x = elem->Attribute("x");
	auto y = elem->Attribute("y");
	auto w = elem->Attribute("width");
	auto h = elem->Attribute("height");
	auto r = elem->Attribute("rotation");
	auto t = elem->Attribute("type");

	layer.objects.push_back(IObject());
	IObject& obj = layer.objects.back();

	obj.id = elem->IntAttribute("id");
	obj.name = elem->Attribute("name");
	obj.type = t == nullptr ? "" : t;

	obj.x = x == nullptr ? 0 : std::stoi(x);
	obj.y = y == nullptr ? 0 : std::stoi(y);
	obj.width = w == nullptr ? 0 : std::stoi(w);
	obj.height = h == nullptr ? 0 : std::stoi(h);
	obj.rotation = r == nullptr ? 0.0f : std::stod(r);
	obj.visible = elem->BoolAttribute("visible");

	std::map<std::string, std::function<bool(XMLElement* elem)>> parseFnLookup;
	parseFnLookup["properties"] = [&](auto elem) {
		return TryParsePropertiesElement(elem, obj.properties);
	};

	parseFnLookup["point"] = [&](auto elem) {
		obj.shapeType = IObject::Type::POINT;
		return true;
	};

	parseFnLookup["ellipse"] = [&](auto elem) {
		obj.shapeType = IObject::Type::ELLIPSE;
		return true;
	};

	parseFnLookup["polyline"] = [&](auto elem) {
		obj.shapeType = IObject::Type::POLYLINE;
		auto points = elem->Attribute("points");
		if (points != nullptr)
			obj.shapePoints = PointsFromString(points);
		return true;
	};

	parseFnLookup["ploygon"] = [&](auto elem) {
		obj.shapeType = IObject::Type::POLYGON;
		auto points = elem->Attribute("points");
		if (points != nullptr)
			obj.shapePoints = PointsFromString(points);
		return true;
	};

	auto childElem = elem->FirstChildElement();
	while (childElem != nullptr)
	{
		if (parseFnLookup[childElem->Name()])
		{
			// do we have a parsing function setup in the lookup table above, call it...
			parseFnLookup[childElem->Name()](childElem);
		}
		else
		{
			// no function was setup for the child element...
			std::cout << "Skipping unknown layer element: " << childElem->Name() << std::endl;
		}
		childElem = childElem->NextSiblingElement();
	}


	return true;
}

bool TiledMapParser::TryParseImageLayerElement(tinyxml2::XMLElement* elem, TileMap* map)
{
	if (strcmp("imagelayer", elem->Name()) != 0)
		return false;

	// TODO: support reading <imagelayer>
	// will need to generate approprate structs for it

	return true;
}

bool TiledMapParser::TryParseGroupElement(tinyxml2::XMLElement* elem, TileMap* map)
{
	if (strcmp("group", elem->Name()) != 0)
		return false;

	// TODO: support reading <group>
	// will need to generate approprate structs for it

	return true;
}

bool TiledMapParser::TryParseEditorSettingsElement(tinyxml2::XMLElement* elem, TileMap* map)
{
	if (strcmp("editorsettings", elem->Name()) != 0)
		return false;

	// TODO: support reading <editorsettings>
	// will need to generate approprate structs for it

	return true;
}

bool TiledMapParser::TryParsePropertiesElement(tinyxml2::XMLElement* elem, PropertyCollection& properties)
{

	// Refer to documentation for tmx file <property> element
	// https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#properties

	if (strcmp("properties", elem->Name()) != 0)
		return false;

	// iterate through each <property> element
	auto propertyElem = elem->FirstChildElement();
	while (propertyElem != nullptr)
	{
		auto name = propertyElem->Attribute("name");
		auto type = PropertyTypeFromString( propertyElem->Attribute("type") );
		auto value = propertyElem->Attribute("value");

		if (name != nullptr) 
		{
			properties[name] = Property(value, type);
		}

		// move to next <property> element
		propertyElem = propertyElem->NextSiblingElement();
	}

	return true;
}

bool TiledMapParser::TryParseImageElement(tinyxml2::XMLElement* elem, TileMap *map, TileSet& tileset)
{
	// Refer to documentation for <image> element
	// https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#image
	// we only care about the "source" attribute

	if (strcmp("image", elem->Name()) != 0)
		return false;

	tileset.imageFileName = map->baseFilePath + elem->Attribute("source");
	tileset.imageWidth = elem->IntAttribute("width");
	tileset.imageHeight = elem->IntAttribute("height");

	return true;
}


bool TiledMapParser::TryParseDataElement(tinyxml2::XMLElement* elem, TileLayer& layer, TileLayerData& data)
{
	// Refer to documentation for <image> element
	// https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#data

	if (strcmp("data", elem->Name()) != 0)
		return false;

	auto encoding = elem->Attribute("encoding"); // csv, base64
	auto compression = elem->Attribute("compression"); // uncompressed(nullptr), gzip, zlib, zstd
	
	std::string sData = TrimString(elem->GetText());
	
	
	// layer data should always end up an array of unsigned ints
	// reserve the memory to prevent resizing
	unsigned int numTiles = layer.rows * layer.cols;
	data.reserve(numTiles);

	if (strcmp("csv", encoding) == 0)
	{
		auto sValues = explode(",", sData, numTiles);
		for (auto& value : sValues)
		{
			data.push_back(std::stoi(value));
		}
	}
	else if (strcmp("base64", encoding) == 0 && compression == nullptr)
	{
		// uncompressed - copy sBytes to layerdata
		// throw std::exception("Parsing base64(uncompressed) format in TSX not yet supported, please update to use Base64 - ZLib export");

		std::string sBytes = base64_decode(sData);

		uint32_t value = *(uint32_t*)&sBytes.data()[0];
		
		data.assign(
			(uint32_t*)&sBytes.data()[0],
			(uint32_t*)&sBytes.data()[sBytes.size()]);
		
	}
	else if (strcmp("base64", encoding) == 0 && strcmp("zlib", compression) == 0)
	{
		// decompress the data using zlib uncompress
		// we can calculte the total bytes we need (rows * cols * sizeof(uint_32)
		std::string sBytes = base64_decode(sData);
		auto zlibDecompressed = miniz::uncompress(sBytes, numTiles * sizeof(uint32_t));

		// copy the decompressed bytes to our layer data
		data.assign(
			(uint32_t*)&zlibDecompressed.data()[0],
			(uint32_t*)&zlibDecompressed.data()[zlibDecompressed.size()]);

	}
	else if (strcmp("base64", encoding) == 0 && strcmp("gzip", compression) == 0)
	{
		// TODO: support gzip
		//  - find a gzip compression lib, use it to decompress data
		//  - copy uncompressed bytes to data.
		throw std::exception("Parsing Base64(GZip) format in TSX not yet supported, please update to use Base64 - ZLib, uncompressed or csv export");
	}
	else if (strcmp("base64", encoding) == 0 && strcmp("zstd", compression) == 0)
	{
		// TODO: support zstd
		//  - find a zstd compression lib, use it to decompress data
		//  - copy uncompressed bytes to data.
		throw std::exception("Parsing Base64(zstd) format in TSX not yet supported, please update to use Base64 - ZLib, uncompressed or csv export");
		std::string sBytes = base64_decode(sData);
	}

	return true;
}



TileMap::Orientation TiledMapParser::OrientationFromString(const char* sOrientation)
{
	// Refer to documentation for supported Orientation strings
	// https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#map

	// return default
	if (sOrientation == nullptr)
		return TileMap::Orientation::ORTHOGONAL;

	// return specified
	if (strcmp("orthogonal", sOrientation) == 0) return TileMap::Orientation::ORTHOGONAL;
	if (strcmp("isometric", sOrientation) == 0) return TileMap::Orientation::UNSUPPORTED;
	if (strcmp("staggered", sOrientation) == 0) return TileMap::Orientation::UNSUPPORTED;
	if (strcmp("hexagonal", sOrientation) == 0) return TileMap::Orientation::UNSUPPORTED;

	// return unknown
	return TileMap::Orientation::UNKNOWN;
}

TileMap::RenderOrder TiledMapParser::RenderOrderFromString(const char* sRenderOrder)
{
	// // Refer to documentation for supported renderorder strings
	// https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#map

	// return the default
	if (sRenderOrder == nullptr) return TileMap::RenderOrder::RIGHT_UP;

	// return specified
	if (strcmp("right-down", sRenderOrder) == 0) return TileMap::RenderOrder::RIGHT_DOWN;
	if (strcmp("right-up", sRenderOrder) == 0) return TileMap::RenderOrder::RIGHT_UP;
	if (strcmp("left-down", sRenderOrder) == 0) return TileMap::RenderOrder::LEFT_DOWN;
	if (strcmp("left-up", sRenderOrder) == 0) return TileMap::RenderOrder::LEFT_UP;

	// unknown
	return TileMap::RenderOrder::UNKNOWN;
}

Property::Type TiledMapParser::PropertyTypeFromString(const char* sPropertyType)
{
	// type: The type of the property.
	// https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#tmx-property
	// Can be string(default), int, float, bool, color, file or object.

	if (sPropertyType == nullptr)
		return Property::Type::STRING;

	if (strcmp("string", sPropertyType) == 0) return Property::Type::STRING;
	if (strcmp("int", sPropertyType) == 0) return Property::Type::INT;
	if (strcmp("float", sPropertyType) == 0) return Property::Type::FLOAT;
	if (strcmp("bool", sPropertyType) == 0) return Property::Type::BOOL;
	if (strcmp("color", sPropertyType) == 0) return Property::Type::COLOR;
	if (strcmp("file", sPropertyType) == 0) return Property::Type::FILE;
	if (strcmp("object", sPropertyType) == 0) return Property::Type::OBJECT;

	// for any other type, we will also default to STRING
	return Property::Type::STRING;
}

unsigned int TiledMapParser::ColorFromString(const char* sColor)
{

	if (sColor == nullptr)
		return 0xFFFFFFFF;

	std::string color = sColor;
	if (color[0] == '#')
	{
		color = "0x" + color.substr(1);
	}
	if (color.size() == 8)
	{
		color += "ff"; // add ff for alpha
	}

	unsigned int value;
	std::stringstream stream;
	stream << std::hex << color;
	stream >> value;
	return value;
}

std::string TiledMapParser::TrimString(const std::string& s)
{ 
	// removes whitespace characters from beginnig and end of string
	// solution was one of the answers on stack overflow... seemed to be mmost efficent though was not the "approved"
	// https://stackoverflow.com/questions/1798112/removing-leading-and-trailing-spaces-from-a-string

	const int l = (int)s.length();
	size_t a = 0, b = l-1;
	char c;
	while (a < l && ((c = s.at(a)) == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r' || c == '\0')) a++;
	while (b > a && ((c = s.at(b)) == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r' || c == '\0')) b--;

	return s.substr(a, 1 + b - a);
}


void TiledMapParser::PopulateLayerTileData(TileMap *map, TileLayer* layer)
{
	// data in a tile also includes flags for if it should be flipped
	// we need to read the bits that indicate flipped, and then clear them out
	// to acquire the globalTileId
	// Refer to documentation on Tile Flippling
	// https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#tile-flipping

	static const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
	static const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
	static const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;

	layer->tileData.clear();

	// reserve to prevent resizing
	unsigned int numTiles = layer->rows * layer->cols;
	layer->tileData.reserve(numTiles + 1);

	for (int y = 0; y < layer->rows; y++)
	{
		for (int x = 0; x < layer->cols; x++)
		{
			int index = y * layer->cols + x;
			unsigned globalTileId = layer->rawTileData[index];

			// get filpped status from the globalTileId
			bool flipped_horizontally = (globalTileId & FLIPPED_HORIZONTALLY_FLAG);
			bool flipped_vertically = (globalTileId & FLIPPED_VERTICALLY_FLAG);
			bool flipped_diagonally = (globalTileId & FLIPPED_DIAGONALLY_FLAG);

			// clear out the flags
			globalTileId &= ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);

			// configure the tileState information
			TileState tileData;
			tileData.globalTileId = globalTileId;
			tileData.layer = layer;

			if (globalTileId > 0)
			{
				tileData.tileset = GetTileSetFromGID(map, globalTileId);
				tileData.localTileId = tileData.tileset->GlobalToLocalTileId(globalTileId);
				tileData.properties = &tileData.tileset->properties[tileData.localTileId];
				tileData.flipped_horizontal = flipped_horizontally || flipped_diagonally;
				tileData.flipped_vertical = flipped_vertically || flipped_diagonally;
				tileData.xIndex = tileData.localTileId % tileData.tileset->tilesPerRow;
				tileData.yIndex = tileData.localTileId / tileData.tileset->tilesPerRow;
			}			
			// save the tileData on the layer..
			layer->tileData.push_back(tileData);
		}

	}
}

TileSet* TiledMapParser::GetTileSetFromGID(TileMap *map, unsigned int gTilesetId)
{
	for (auto& iter : map->tileSets)
	{
		auto& tileset = iter.second;
		int maxId = tileset.firstGlobalTileId + tileset.tileCount;

		if (gTilesetId >= tileset.firstGlobalTileId && gTilesetId < maxId)
			return &(iter.second);
	}

	return nullptr;
}

std::vector<IObject::Point> TiledMapParser::PointsFromString(const std::string& str)
{
	std::vector<IObject::Point> points;

	// explode string by space to get comma seperated x,y numbers
	auto xy_list = explode(" ", str);
	
	for (auto& xys : xy_list)
	{
		// explode by , to get individual x and y numbers
		auto xy = explode(",", xys);

		// add to points
		points.push_back({
			std::stoi(xy[0]),
			std::stoi(xy[1])
		});
	}

	return points;
}


// ------------------------------------------------------
// Explose method from internet:
// http://www.zedwood.com/article/cpp-explode-function
// ------------------------------------------------------
static std::vector<std::string> explode(const std::string& delimiter, const std::string& str, unsigned int reserve)
{
	
	std::vector<std::string> arr;
	if (reserve > 0) {
		arr.reserve(reserve);
	}

	int strleng = str.length();
	int delleng = delimiter.length();
	if (delleng == 0)
		return arr;//no change

	int i = 0;
	int k = 0;
	while (i < strleng)
	{
		int j = 0;
		while (i + j < strleng && j < delleng && str[i + j] == delimiter[j])
			j++;
		if (j == delleng)//found delimiter
		{
			arr.push_back(str.substr(k, i - k));
			i += delleng;
			k = i;
		}
		else
		{
			i++;
		}
	}
	arr.push_back(str.substr(k, i - k));
	return arr;
}