#pragma once

#include <map>
#include <string>
#include "raylib.h"

class TileMap;

class App
{
public:

	App();
	~App();

	void Run();

	void Load();
	void Unload();

	void Update(float dt);
	void Draw();

	void DrawTile(const std::string& texture, int srcX, int srcY, int srcW, int srcH, int dstX, int dstY, int dstW, int dstH, unsigned int color);

protected:

	int m_windowWidth = 800;
	int m_windowHeight = 450;

	TileMap* m_map = nullptr;
	std::map<std::string, Texture2D> m_textureLookup; // filename to texture

private:
};
