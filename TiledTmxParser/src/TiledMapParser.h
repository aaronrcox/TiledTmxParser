#pragma once

#include <string>
#include "TinyXml2/TinyXml2.h"

#include "TileMap.h"

class TiledMapParser
{
public:

	TiledMapParser();
	~TiledMapParser();

	bool LoadFromFile(const char *filename, TileMap *map);

protected:

	bool TryParseTileSetElement(tinyxml2::XMLElement* elem, TileSetCollection& tilesets);
	bool TryParseLayerElement(tinyxml2::XMLElement* elem, TileMap* map, NamedTileLayerCollection& layer);
	bool TryParseObjectGroupElement(tinyxml2::XMLElement* elem, TileMap* map);
	bool TryParseImageLayerElement(tinyxml2::XMLElement* elem, TileMap* map);
	bool TryParseGroupElement(tinyxml2::XMLElement* elem, TileMap* map);
	bool TryParseEditorSettingsElement(tinyxml2::XMLElement* elem, TileMap* map);
	bool TryParsePropertiesElement(tinyxml2::XMLElement* elem, PropertyCollection& properties);
	bool TryParseDataElement(tinyxml2::XMLElement* elem, TileLayer& layer, LayerData& data);

	bool TryParseImageElement(tinyxml2::XMLElement* elem, TileSet& tileset);

	TileMap::Orientation OrientationFromString(const char *sOrientation);
	TileMap::RenderOrder RenderOrderFromString(const char* sRenderOrder);
	Property::Type PropertyTypeFromString(const char* sPropertyType);
	unsigned int ColorFromString(const char* sColor);

	std::string TrimString(const std::string& s);

private:
};


