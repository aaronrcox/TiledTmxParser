#include "TileMap.h"
#include "TiledMapParser.h"
#include "TileLayer.h"

TileMap::TileMap()
{

}

TileMap::~TileMap()
{

}

void TileMap::Load(const char* filename)
{
	TiledMapParser parser;
	parser.LoadFromFile(filename, this);
}

void TileMap::LoadTextures(std::function<void(std::string)> loadTextureCallback)
{
	for (auto iter : tileSets)
	{
		loadTextureCallback(iter.second.imageFileName.c_str());
	}
}

TileSet *TileMap::GetTileSetFromGID(unsigned int gTilesetId)
{
	for (auto& iter : tileSets)
	{
		auto& tileset = iter.second;
		int maxId = tileset.firstGlobalTileId + tileset.tileCount;

		if (gTilesetId >= tileset.firstGlobalTileId && gTilesetId < maxId)
			return &(iter.second);
	}

	return nullptr;
}

void TileMap::DrawLayer(ILayer* iLayer, DrawTileCb drawTileCb)
{
	
	int sx = 0, sy = 0;
	int xd = 1, yd = 1;

	// Tiled supports a property that allows us to specify rendering order of tiles
	// we are setting approprate startX, startY and Direction values for our for loop below
	// to achieve the correct rendering direction.
	switch (renderOrder)
	{
	case TileMap::RenderOrder::RIGHT_DOWN:  sx = 0;        sy = 0;        xd = 1,  yd = 1;  break;
	case TileMap::RenderOrder::RIGHT_UP:    sx = 0;        sy = rows - 1; xd = 1,  yd = -1; break;
	case TileMap::RenderOrder::LEFT_UP:     sx = cols - 1; sy = rows - 1; xd = -1, yd = -1; break;
	case TileMap::RenderOrder::LEFT_DOWN:   sx = cols - 1; sy = 0;        xd = -1; yd = 1;  break;
	}

	if (iLayer->type == ILayer::LayerType::LAYER)
	{

		auto layer = dynamic_cast<TileLayer*>(iLayer);
		if (layer == nullptr)
			return;

		if (layer->visible == false)
			return;

		// loop through each tile for the layer
		for (int y = sy; y >= 0 && y < rows; y += yd)
		{
			for (int x = sx; x >= 0 && x < cols; x += xd)
			{
				// get the index for the current tile in the layer
				unsigned int layerDataIndex = y * cols + x;

				// get the Global tileSetTileId from the layer data
				auto tileData = layer->GetTileData(x, y);

				if (tileData.globalTileId == 0)
					continue;

				// find the approprate tileSet to use based on the tilesetId.
				// and convert it to the local tileset index.
				auto tileset = GetTileSetFromGID(tileData.globalTileId);
				auto tilesetId = tileset->GlobalToLocalTileId(tileData.globalTileId);

				int srcXIndex = tilesetId % tileset->tilesPerRow;
				int srcYIndex = tilesetId / tileset->tilesPerRow;

				// Finally, we have enough information to draw the tile
				drawTileCb(

					tileset->imageFileName,

					srcXIndex * tileset->tileWidth,
					srcYIndex * tileset->tileHeight,
					tileset->tileWidth,
					tileset->tileHeight,

					x * tileWidth,
					y * tileHeight,
					tileWidth,
					tileHeight,

					layer->tintColor
				);

			}
		}
	}
	
}


//unsigned int TileMap::LayerCount()
//{
//	return namedLayers.size();
//}
//Layer* TileMap::GetLayerById(unsigned int id)
//{
//	for (auto& iter : namedLayers)
//	{
//		if (iter.second.id == id)
//			return &(iter.second);
//	}
//
//	return nullptr;
//}
//Layer* TileMap::GetLayerByName(const std::string& name)
//{
//	auto iter = namedLayers.find(name);
//	if (iter == namedLayers.end())
//		return nullptr;
//
//	return &(iter->second);
//}