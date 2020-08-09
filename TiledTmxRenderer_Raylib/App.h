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

protected:

	int m_windowWidth = 1440;
	int m_windowHeight = 900;

	TileMap* m_map = nullptr;
	std::map<std::string, Texture2D> m_textureLookup; // filename to texture

private:
};
