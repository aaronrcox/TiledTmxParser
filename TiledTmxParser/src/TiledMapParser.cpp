#include "TiledMapParser.h"
#include "TileMap.h"
#include <iostream>
#include <map>
#include <functional>
#include <sstream>
#include "Encoding/base64.h"
#include "miniz/miniz.h"

using namespace tinyxml2;

TiledMapParser::TiledMapParser()
{

}

TiledMapParser::~TiledMapParser()
{

}

bool TiledMapParser::LoadFromFile(const std::string& filename, TileMap* map)
{
	XMLDocument doc;
	doc.LoadFile(filename.c_str());

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
		return TryParseLayerElement(elem, map, map->namedLayers);
	};
	parseFnLookup["objectgroup"] = [&](auto elem) { 
		return TryParseObjectGroupElement(elem, map);
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

	return true;
}

bool TiledMapParser::TryParseTileSetElement(tinyxml2::XMLElement* elem, TileMap *map, TileSetCollection& tileset)
{
	// Refer to documentation for tmx file <tileset> element
	// https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#tileset

	if (strcmp("tileset", elem->Name()) != 0)
		return false;

	// ensure the tileset has the "name" attribute
	auto name = elem->Attribute("name");
	if (name == nullptr)
		return false;

	// if the source element is set, then the data is stored in an external xml file.
	auto source = elem->Attribute("source");
	if (source != nullptr)
	{
		// TODO: load the tileset xml doc from file
		return false;
	}

	tileset[name].name = name;
	tileset[name].firstGlobalTileId = elem->IntAttribute("firstgid");
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
		// TODO: support tile
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
	layers[name].name = name;
	layers[name].id = elem->IntAttribute("id");
	layers[name].rows = elem->IntAttribute("height");
	layers[name].cols = elem->IntAttribute("width");
	layers[name].opacity = elem->FloatAttribute("opacity", 1.0f);
	layers[name].visible = elem->BoolAttribute("visible", true);
	layers[name].tintColor = ColorFromString(elem->Attribute("tintcolor"));
	layers[name].offsetX = elem->IntAttribute("offsetx");
	layers[name].offsetY = elem->IntAttribute("offsety");
	
	std::map<std::string, std::function<bool(XMLElement* elem)>> parseFnLookup;
	parseFnLookup["data"] = [&](auto elem) {
		return TryParseDataElement(elem, layers[name], layers[name].data);
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

bool TiledMapParser::TryParseObjectGroupElement(tinyxml2::XMLElement* elem, TileMap* map)
{
	if (strcmp("objectgroup", elem->Name()) != 0)
		return false;

	// TODO: support reading <objectgroup>
	// will need to generate approprate structs for it

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
			properties[name] = Property(name, value, type);
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


bool TiledMapParser::TryParseDataElement(tinyxml2::XMLElement* elem, TileLayer& layer, LayerData& data)
{
	// Refer to documentation for <image> element
	// https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#data

	if (strcmp("data", elem->Name()) != 0)
		return false;

	auto encoding = elem->Attribute("encoding"); // csv, base64
	auto compression = elem->Attribute("compression"); // uncompressed(nullptr), gzip, zlib, zstd
	
	std::string sData = TrimString(elem->GetText());
	std::string sBytes = base64_decode(sData);
	
	// layer data should always end up an array of unsigned ints
	// reserve the memory to prevent resizing
	unsigned int numTiles = layer.rows * layer.cols + 1;
	data.reserve(numTiles);

	if (strcmp("csv", encoding) == 0)
	{
		// TODO: support reading csv data.
	}
	else if (strcmp("base64", encoding) == 0 && compression == nullptr)
	{
		// uncompressed - copy sBytes to layerdata
		data.assign(
			(unsigned int*)&sBytes.data()[0],
			(unsigned int*)&sBytes.data()[sBytes.size() - 1]);
	}
	else if (strcmp("base64", encoding) == 0 && strcmp("zlib", compression) == 0)
	{
		// decompress the data using zlib uncompress
		// we can calculte the total bytes we need (rows * cols * sizeof(uint_32)
		auto zlibDecompressed = miniz::uncompress(sBytes, numTiles * sizeof(uint32_t));

		// copy the decompressed bytes to our layer data
		data.assign(
			(unsigned int*)&zlibDecompressed.data()[0],
			(unsigned int*)&zlibDecompressed.data()[zlibDecompressed.size() - 1]);

	}
	else if (strcmp("base64", encoding) == 0 && strcmp("gzip", compression) == 0)
	{
		// TODO: support gzip
		//  - find a gzip compression lib, use it to decompress data
		//  - copy uncompressed bytes to data.
	}
	else if (strcmp("base64", encoding) == 0 && strcmp("zstd", compression) == 0)
	{
		// TODO: support zstd
		//  - find a gzip compression lib, use it to decompress data
		//  - copy uncompressed bytes to data.
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
	size_t a = 0, b = l - 1;
	char c;
	while (a < l && ((c = s.at(a)) == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r' || c == '\0')) a++;
	while (b > a && ((c = s.at(b)) == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r' || c == '\0')) b--;

	return s.substr(a, 1 + b - a);
}

