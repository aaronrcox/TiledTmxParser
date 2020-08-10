#pragma once

#include <map>
#include <string>
#include "raylib.h"

class TileMap;
class Player;
class Path;

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

	void DrawPlayer();

	void SmoothCameraFollow(Vector2 targetPos, float dt);

protected:

	int m_windowWidth = 1440;
	int m_windowHeight = 900;

	TileMap* m_map = nullptr;
	std::map<std::string, Texture2D> m_textureLookup; // filename to texture

	Camera2D m_camera = { 0 };
	Rectangle view = { 0, 0, 1440, 900 };

	Player* m_player = nullptr;
	Path* m_playerSpawnPath;

private:
};
