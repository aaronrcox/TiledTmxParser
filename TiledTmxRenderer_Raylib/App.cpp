#include "App.h"

#include "TileMap.h"
#include "TileMapRenderer.h"


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

	// setup camera
	m_camera.target = { m_playerPos.x + 20.0f, m_playerPos.y + 20.0f };
	m_camera.offset = { m_windowWidth / 2.0f, m_windowHeight / 2.0f };
	m_camera.rotation = 0.0f;
	m_camera.zoom = 1.0f;

}
void App::Unload()
{
	m_map->SetRenderer(nullptr);
	delete m_map;
}

void App::Update(float dt)
{
	if (IsKeyDown(KEY_UP)) m_camera.zoom -= 1 * dt;
	if (IsKeyDown(KEY_DOWN)) m_camera.zoom += 1 * dt;
	
	if (IsKeyDown(KEY_A)) m_playerPos.x -= 100 * dt;
	if (IsKeyDown(KEY_D)) m_playerPos.x += 100 * dt;
	if (IsKeyDown(KEY_W)) m_playerPos.y -= 100 * dt;
	if (IsKeyDown(KEY_S)) m_playerPos.y += 100 * dt;

	// update camera
	m_camera.target = { m_playerPos.x + 20.0f, m_playerPos.y + 20.0f };
	
	view.x = m_camera.target.x - (m_camera.offset.x * (1.0f / m_camera.zoom));
	view.y = m_camera.target.y - (m_camera.offset.y * (1.0f / m_camera.zoom));
	view.width = m_windowWidth * (1.0f/m_camera.zoom);
	view.height = m_windowHeight * (1.0f/m_camera.zoom);
}

void App::Draw()
{
	BeginMode2D(m_camera);

	// Setting view port enables renderer
	// to only draw tiles that are visible.
	m_map->GetRenderer()->SetView(view.x, view.y, view.width, view.height);

	// loop through each layer - invoke draw method
	for (auto& layer : m_map->layers)
	{
		m_map->DrawLayer(layer);
	}

	EndMode2D();

	// Draw Game FPS
	DrawRectangle(0, 0, m_windowWidth, 42, { 0, 0, 0, 128 });
	DrawFPS(10, 10);
}

