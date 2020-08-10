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


TileLayer* TileMap::GetTileLayer(const std::string& name)
{
	auto iter = namedTileLayers.find(name);
	if (iter == namedTileLayers.end())
		return nullptr;

	return &iter->second;
}

ObjectGroupLayer* TileMap::GetObjectGroup(const std::string& name)
{
	auto iter = namedObjectLayers.find(name);
	if (iter == namedObjectLayers.end())
		return nullptr;

	return &iter->second;
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

	// calculate clipping area based on the viewport defined in the renderer
	//------------------
	const int& viewX = m_renderer->settings.viewX;
	const int& viewY = m_renderer->settings.viewY;
	const int& viewW = m_renderer->settings.viewW;
	const int& viewH = m_renderer->settings.viewH;

	int minX = (viewX / (int)tileWidth);
	int maxX = minX + (viewW / (int)tileWidth) + 2.0f;
	int minY = (viewY / (int)tileHeight);
	int maxY = minY + (viewH / (int)tileHeight) + 2.0f;

	sx = std::max(minX, std::min(sx, maxX));
	sy = std::max(minY, std::min(sy, maxY));
	maxX = std::max(0, std::min(maxX, (int)cols));
	maxY = std::max(0, std::min(maxY, (int)rows));
	//------------------



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

				int flipX = tileData.flipped_horizontal ? -1 : 1;
				int flipY = tileData.flipped_vertical ? -1 : 1;

				// Finally, we have enough information to draw the tile
				m_renderer->DrawTile(

					tileData.tileset->imageFileName,

					tileData.xIndex* tileData.tileset->tileWidth,
					tileData.yIndex* tileData.tileset->tileHeight,
					tileData.tileset->tileWidth,
					tileData.tileset->tileHeight,

					x* tileWidth,
					y* tileHeight,
					tileWidth* flipX,
					tileHeight* flipY,

					col.value
				);
			}
		}
	}


	// Renders Debugging Info
	if (m_renderer->settings.drawDebugViewLines)
	{
		m_renderer->DrawRectLines(sx* tileWidth, sy* tileHeight, (maxX - sx)* tileWidth, (maxY - sy)* tileHeight, 2, 0xFF000000);
		m_renderer->DrawRectLines(viewX, viewY, viewW, viewH, 2, 0x7f0000FF);
	}
}

