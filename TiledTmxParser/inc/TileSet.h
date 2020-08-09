#pragma once

#include <string>

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
};
