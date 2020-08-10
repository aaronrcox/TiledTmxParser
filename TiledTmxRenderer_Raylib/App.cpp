#include "App.h"
#include "raygui.h"
#include "raymath.h"
#include "TileMap.h"
#include "TileMapRenderer.h"

#include "Player.h"
#include "Path.h"

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
	m_map->Load("./assets/test.tmx");
	m_map->SetRenderer(new TileMapRenderer());

	auto levelInfo = m_map->GetObjectGroup("LevelInfo");
	auto ps = levelInfo->GetObjectByName("PlayerSpawn");
	auto playerSpawnPath = levelInfo->GetObjectByName("PlayerSpawnPath");

	// load the path from tiled into the Path object
	m_playerSpawnPath = new Path();
	for (auto& p : playerSpawnPath->shapePoints)
	{
		m_playerSpawnPath->Add({ 
			levelInfo->offsetX + (float)playerSpawnPath->x + p.x, 
			levelInfo->offsetY + (float)playerSpawnPath->y + p.y
		});
	}

	// create the player
	m_player = new Player();
	m_player->SetPosition({
		ps != nullptr ? ps->x + levelInfo->offsetX : m_map->rows * m_map->tileWidth / 2.0f,
		ps != nullptr ? ps->y + levelInfo->offsetY : m_map->cols * m_map->tileHeight / 2.0f
	});

	m_player->SetPath(m_playerSpawnPath);
	m_player->SetState(Player::State::FollowPath);

	// setup camera
	m_camera.target = { m_player->GetPosition().x + 20.0f, m_player->GetPosition().y + 20.0f };
	m_camera.offset = { m_windowWidth / 2.0f, m_windowHeight / 2.0f };
	m_camera.rotation = 0.0f;
	m_camera.zoom = 1.0f;

}
void App::Unload()
{
	delete m_playerSpawnPath;
	delete m_player;

	m_map->SetRenderer(nullptr);
	delete m_map;
}

void App::Update(float dt)
{
	if (IsKeyDown(KEY_UP)) m_camera.zoom -= 1 * dt;
	if (IsKeyDown(KEY_DOWN)) m_camera.zoom += 1 * dt;
	
	m_player->Update(dt);

	// update camera
	SmoothCameraFollow(m_player->GetPosition(), dt);

	
}

void App::Draw()
{
	BeginMode2D(m_camera);

	// Setting view port enables renderer
	// to only draw tiles that are visible.
	m_map->GetRenderer()->SetView(view.x, view.y, view.width, view.height);

	// loop through each layer - invoke draw method
	for (int i = 0; i < m_map->layers.size(); i++)
	{
		m_map->DrawLayer(m_map->layers[i]);

		// draw player on layer 1 (underneith walls / trees)
		if (i == 1)
		{
			DrawPlayer();
		}
	}

	EndMode2D();

	// Draw Game FPS
	DrawRectangle(0, 0, m_windowWidth, 42, { 0, 0, 0, 128 });
	char sFps[16]; sprintf(sFps, "FPS: %d", GetFPS());
	DrawText(sFps, 10, 10, 24, RAYWHITE);

	// Draw GUI
	if (GuiButton({ m_windowWidth - 160.0f, 5.0f, 150.0f, 30.0f }, "Toggle Debug Render"))
	{
		auto& rendererSettings = m_map->GetRenderer()->settings;
		rendererSettings.drawDebugViewLines = !rendererSettings.drawDebugViewLines;
		GameObject::DebugRenderEnabled = !GameObject::DebugRenderEnabled;
	}
}

void App::DrawPlayer()
{
	m_player->Draw();

}

void App::SmoothCameraFollow(Vector2 targetPos, float dt)
{
	static float minSpeed = 30;
	static float minEffectLength = 10;
	static float fractionSpeed = 0.8f;

	m_camera.offset = { m_windowWidth / 2.0f, m_windowHeight / 2.0f };
	Vector2 diff = Vector2Subtract(targetPos, m_camera.target);
	float length = Vector2Length(diff);

	if (length > minEffectLength)
	{
		float speed = fmaxf(fractionSpeed * length, minSpeed);
		m_camera.target = Vector2Add(m_camera.target, Vector2Scale(diff, speed * dt / length));
	}

	// This logic updates the view rect
	// based on the camera position
	view.x = m_camera.target.x - (m_camera.offset.x * (1.0f / m_camera.zoom));
	view.y = m_camera.target.y - (m_camera.offset.y * (1.0f / m_camera.zoom));
	view.width = m_windowWidth * (1.0f / m_camera.zoom);
	view.height = m_windowHeight * (1.0f / m_camera.zoom);
}
