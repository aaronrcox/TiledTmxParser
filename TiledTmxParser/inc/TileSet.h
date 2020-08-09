#pragma once

#include <string>
#include <map>
#include "PropertyCollection.h"

typedef std::map<unsigned int, PropertyCollection> TilePropertyCollection;

class TileSet
{
public:

public:

	TileSet();
	~TileSet();

	unsigned int GlobalToLocalTileId(unsigned int globalTileId);

public:

	std::string name;
	std::string imageFileName;

	unsigned int firstGlobalTileId;
	unsigned int tileWidth;
	unsigned int tileHeight;
	unsigned int tileCount;
	unsigned int tilesPerRow;
	unsigned int spacing;
	unsigned int margin;
	unsigned int tileOffsetX;
	unsigned int tileOffsetY;

	unsigned int imageWidth;
	unsigned int imageHeight;

	unsigned int colorKey;

	// access to a property for a given tile
	// properties[localTileId]["propertyName"]
	TilePropertyCollection properties;
};
