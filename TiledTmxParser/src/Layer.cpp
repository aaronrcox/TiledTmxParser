
#include "TileLayer.h"

TileLayer::TileState TileLayer::GetTileData(unsigned int x, unsigned int y)
{

	// data in a tile also includes flags for if it should be flipped
	// we need to read the bits that indicate flipped, and then clear them out
	// to acquire the globalTileId
	// Refer to documentation on Tile Flippling
	// https://doc.mapeditor.org/en/stable/reference/tmx-map-format/#tile-flipping


	static const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
	static const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
	static const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;

	unsigned id = data[y * cols + x];

	//// get filpped status from the id
	bool flipped_horizontally = (id & FLIPPED_HORIZONTALLY_FLAG);
	bool flipped_vertically = (id & FLIPPED_VERTICALLY_FLAG);
	bool flipped_diagonally = (id & FLIPPED_DIAGONALLY_FLAG);

	// clear out the flags
	id &= ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);

	 return { id, flipped_horizontally, flipped_vertically, flipped_diagonally };
}