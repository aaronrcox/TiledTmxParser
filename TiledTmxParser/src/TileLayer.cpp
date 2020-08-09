
#include "TileLayer.h"

TileState& TileLayer::GetTileData(unsigned int x, unsigned int y)
{
	return tileData[y * cols + x];
}