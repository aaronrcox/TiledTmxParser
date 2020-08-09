
#include "TileSet.h"



TileSet::TileSet()
{

}

TileSet::~TileSet()
{

}

unsigned int TileSet::GlobalToLocalTileId(unsigned int globalTileId)
{
	return globalTileId - firstGlobalTileId;
}

