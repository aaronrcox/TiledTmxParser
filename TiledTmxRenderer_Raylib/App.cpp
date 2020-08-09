#include "App.h"

#define _INCLUDE_TiledTmxParserLib_
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

	auto DrawTile = [this](const std::string& texture, int srcX, int srcY, int srcW, int srcH, 
		int dstX, int dstY, int dstW, int dstH, unsigned int color)
	{
		Rectangle src = { srcX, srcY, srcW, srcH };
		Rectangle dst = { dstX, dstY, dstW, dstH };

		// convert unsigned int to Raylib color struct
		Color tintColor = *((Color*)(&color));

		// get thexture from our lookup table
		auto& tex = m_textureLookup[texture];

		// draw the tile via raylib
		DrawTexturePro(tex, src, dst, { 0, 0 }, 0.0f, tintColor);
	};

	
	for (auto& layer : m_map->layers)
	{
		m_map->DrawLayer(layer, DrawTile);
	}


}


