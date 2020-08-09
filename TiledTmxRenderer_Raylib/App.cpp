#include "App.h"
#include "TileMap.h"

App::App()
{

}

App::~App()
{

}

void App::Run()
{
	InitWindow(m_windowWidth, m_windowHeight, "Raylib Tiled2D Map Renderer");
	SetTargetFPS(60);

	Load();

	while (!WindowShouldClose())
	{
		Update(GetFrameTime());

		BeginDrawing();
		ClearBackground(RAYWHITE);

		Draw();
		
		EndDrawing();
	}

	Unload();

	CloseWindow();
}

void App::Load()
{
	m_map = new TileMap();
	m_map->Load("./assets/orthogonal-outside.tmx");
	m_map->LoadTextures([this](std::string filename) {
		auto file = "./assets/" + filename;
		m_textureLookup[filename] = LoadTexture(file.c_str());
	});
}
void App::Unload()
{
	delete m_map;
}

void App::Update(float dt)
{

}

void App::Draw()
{
	unsigned int rows = m_map->rows;
	unsigned int cols = m_map->cols;

	int sx = 0, sy = 0;
	int xd = 1, yd = 1;

	// Tiled supports a property that allows us to specify rendering order of tiles
	// we are setting approprate startX, startY and Direction values for our for loop below
	// to achieve the correct rendering direction.
	switch (m_map->renderOrder)
	{
	case TileMap::RenderOrder::RIGHT_DOWN:  sx = 0;      sy = 0;      xd = 1, yd = 1; break;
	case TileMap::RenderOrder::RIGHT_UP:    sx = 0;      sy = rows-1; xd = 1, yd =-1; break;
	case TileMap::RenderOrder::LEFT_UP:     sx = cols-1; sy = rows-1; xd =-1, yd =-1; break;
	case TileMap::RenderOrder::LEFT_DOWN:   sx = cols - 1; sy = 0;    xd =-1; yd = 1; break;
	}
	

	// Loop through each layer in the tilemap
	// Tiled added support for different types of layers
	// so we need to check which layer type we are rendering
	for (auto iLayer : m_map->layers)
	{
		if (iLayer->type == ILayer::LayerType::LAYER) {
			auto layer = dynamic_cast<Layer *>(iLayer);
			if (layer == nullptr)
				continue;

			if (layer->visible == false)
				continue;

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
					auto tileset = m_map->GetTileSetFromGID(tileData.globalTileId);
					auto tilesetId = tileset->GlobalToLocalTileId(tileData.globalTileId);

					int srcXIndex = tilesetId % tileset->tilesPerRow;
					int srcYIndex = tilesetId / tileset->tilesPerRow;

					DrawTile(
						
						tileset->imageFileName,

						srcXIndex * tileset->tileWidth,
						srcYIndex * tileset->tileHeight,
						tileset->tileWidth,
						tileset->tileHeight,

						x * m_map->tileWidth,
						y * m_map->tileHeight,
						m_map->tileWidth,
						m_map->tileHeight,

						layer->tintColor
					);
					
				}
			}
		}
	}
}


void App::DrawTile(const std::string& texture, int srcX, int srcY, int srcW, int srcH, int dstX, int dstY, int dstW, int dstH, unsigned int color)
{
	Rectangle src = { srcX, srcY, srcW, srcH };
	Rectangle dst = { dstX, dstY, dstW, dstH };

	// convert unsigned int to Raylib color struct
	Color tintColor = *((Color*)(&color));

	// get thexture from our lookup table
	auto& tex = m_textureLookup[texture];

	// draw the tile
	DrawTexturePro(tex, src, dst, { 0, 0 }, 0.0f, tintColor);
}