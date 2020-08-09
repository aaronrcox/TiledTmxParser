#include "Player.h"
#include "PlayerKeyboardController.h"

Player::Player() : GameObject()
{

	m_keyboardController = new PlayerKeyboardController();
	SetBehaviour(m_keyboardController);
	SetFriction(1.0f);

	m_playerTexture = LoadTexture("./assets/topdown-shooter/PNG/Man Old/manOld_stand.png");

}

Player::~Player()
{
	UnloadTexture(m_playerTexture);

	SetBehaviour(nullptr);
	delete m_keyboardController;
}

void Player::Update(float deltaTime)
{
	GameObject::Update(deltaTime);
}

void Player::Draw()
{
	float rot = atan2f(m_facingDir.y, m_facingDir.x) * (180.0f / 3.141592653589793238463f);

	float tw = (float)m_playerTexture.width;
	float th = (float)m_playerTexture.height;

	DrawTexturePro(m_playerTexture,
		{ 0.0f, 0.0f, (float)m_playerTexture.width, (float)m_playerTexture.height },
		{ m_position.x, m_position.y, tw, th },
		{ tw * 0.5f, th * 0.5f }, 
		rot, WHITE);

	DrawCircle(m_position.x, m_position.y, 3, WHITE);

	// GameObject::Draw();
}