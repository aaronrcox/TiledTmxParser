#include "TileMap.h"
#include "ITileMapRenderer.h"
#include "TiledMapParser.h"
#include "TileLayer.h"
#include <algorithm>

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
		loadTextureCallback(iter.second.imageFileName);
	}
}

void TileMap::SetRenderer(ITileMapRenderer* renderer)
{
	if (m_renderer != nullptr)
		delete m_renderer;

	m_renderer = renderer;
	if (m_renderer == nullptr)
		return;

	for (auto iter : tileSets)
	{
		m_renderer->LoadTexture(iter.second.imageFileName);
	}
}

ITileMapRenderer* TileMap::GetRenderer()
{
	return m_renderer;
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

void TileMap::DrawLayer(ILayer* iLayer)
{
	union sColor
	{
		struct { unsigned char r, g, b, a; };
		unsigned int value;
	};
	
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


	int minX = (m_renderer->viewX / (int)tileWidth);
	int maxX = minX + (m_renderer->viewW / (int)tileWidth) + 1;
	int minY = (m_renderer->viewY / (int)tileHeight);
	int maxY = minY + (m_renderer->viewH / (int)tileHeight) + 1;

	sx = std::max(minX, std::min(sx, maxX));
	sy = std::max(minY, std::min(sy, maxY));
	maxX = std::max(0, std::min(maxX, (int)cols));
	maxY = std::max(0, std::min(maxY, (int)rows));

	

	if (iLayer->type == ILayer::LayerType::LAYER)
	{
		auto layer = dynamic_cast<TileLayer*>(iLayer);
		if (layer == nullptr)
			return;

		if (layer->visible == false)
			return;

		// loop through each tile for the layer
		for (int y = sy; y >= 0 && y < maxY; y += yd)
		{
			for (int x = sx; x >= 0 && x < maxX; x += xd)
			{
				// get the Global tileSetTileId from the layer data
				auto& tileData = layer->GetTileData(x, y);

				if (tileData.globalTileId == 0)
					continue;

				// calculate the color for the rendered tile: layerTintColor + apply opacity to alpha
				sColor col;
				col.value = layer->tintColor;
				col.a = layer->opacity * col.a;

				// Finally, we have enough information to draw the tile
				m_renderer->DrawTile(

					tileData.tileset->imageFileName,

					tileData.xIndex * tileData.tileset->tileWidth,
					tileData.yIndex * tileData.tileset->tileHeight,
					tileData.tileset->tileWidth,
					tileData.tileset->tileHeight,

					x * tileWidth,
					y * tileHeight,
					tileWidth,
					tileHeight,

					col.value
				);

			}
		}
	}

	m_renderer->DrawRectLines(sx* tileWidth, sy* tileHeight, (maxX-sx)*tileWidth, (maxY-sy)*tileHeight, 2, 0xFF000000);
	m_renderer->DrawRectLines(m_renderer->viewX, m_renderer->viewY, m_renderer->viewW, m_renderer->viewH, 2, 0x7f0000FF);
	
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