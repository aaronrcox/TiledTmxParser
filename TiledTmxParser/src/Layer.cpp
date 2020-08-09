
#include "Layer.h"

Layer::TileState Layer::GetTileData(unsigned int x, unsigned int y)
{

	static const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
	static const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
	static const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;

	int index = y * cols + x;

	unsigned id = data[index];

	

	//// get filpped status from the id
	bool flipped_horizontally = (id & FLIPPED_HORIZONTALLY_FLAG);
	bool flipped_vertically = (id & FLIPPED_VERTICALLY_FLAG);
	bool flipped_diagonally = (id & FLIPPED_DIAGONALLY_FLAG);

	// clear out the flags
	id &= ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);

	//if (id > 1000)
	//	id = 0;

	 return { id, flipped_horizontally, flipped_vertically, flipped_diagonally };
}