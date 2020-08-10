#pragma once

#include <string>
#include "TinyXml2/TinyXml2.h"

#include "TileMap.h"

class TiledMapParser
{
public:

	TiledMapParser();
	~TiledMapParser();

	bool LoadFromFile(const std::string& filename, TileMap *map);

protected:

	bool TryParseTileSetElement(tinyxml2::XMLElement* elem, TileMap* map, TileSetCollection& tilesets);
	bool TryParseLayerElement(tinyxml2::XMLElement* elem, TileMap* map, NamedTileLayerCollection& layers);
	bool TryParseLayerAttributes(tinyxml2::XMLElement* elem, TileMap* map, const std::string& layerName, ILayer* layer);
	bool TryParseObjectGroupElement(tinyxml2::XMLElement* elem, TileMap* map, NamedObjectGroupLayerCollection& layers);
	bool TryParseObjectElement(tinyxml2::XMLElement* elem, TileMap* map, ObjectGroupLayer& layer);
	bool TryParseImageLayerElement(tinyxml2::XMLElement* elem, TileMap* map);
	bool TryParseGroupElement(tinyxml2::XMLElement* elem, TileMap* map);
	bool TryParseEditorSettingsElement(tinyxml2::XMLElement* elem, TileMap* map);
	bool TryParsePropertiesElement(tinyxml2::XMLElement* elem, PropertyCollection& properties);
	bool TryParseDataElement(tinyxml2::XMLElement* elem, TileLayer& layer, TileLayerData& data);
	bool TryParseImageElement(tinyxml2::XMLElement* elem, TileMap* map, TileSet& tileset);

	void PopulateLayerTileData(TileMap* map, TileLayer* layer);

	TileMap::Orientation OrientationFromString(const char *sOrientation);
	TileMap::RenderOrder RenderOrderFromString(const char* sRenderOrder);
	Property::Type PropertyTypeFromString(const char* sPropertyType);
	unsigned int ColorFromString(const char* sColor);
	std::vector<IObject::Point> PointsFromString(const std::string& str);

	std::string TrimString(const std::string& s);

	TileSet* GetTileSetFromGID(TileMap* map, unsigned int gTilesetId);

private:
};


