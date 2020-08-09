#pragma once

#include "PropertyCollection.h"

class TileLayer;
class TileSet;

class TileState
{
public:

	// the global id of a tile accross all tilesets
	unsigned int globalTileId = 0;

	// flags set to determine if the tile should be flipped
	bool flipped_vertical = false;
	bool flipped_horizontal = false;

	// the local id of a tile on the tileset.
	unsigned int localTileId = 0;

	// x and y index for the given tile on the tileset
	// calculated from localTileId
	unsigned int xIndex = 0;
	unsigned int yIndex = 0;

	// pointer to the layer that this tile belongs to
	TileLayer* layer = nullptr;

	// the tileset used by this tile
	TileSet* tileset = nullptr;

	// any properties associated with the tile
	PropertyCollection* properties = nullptr;
};
