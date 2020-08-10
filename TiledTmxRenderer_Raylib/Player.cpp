#include "Player.h"
#include "PlayerKeyboardController.h"
#include "Path.h"
#include "FollowPathBehaviour.h"

Player::Player() : GameObject()
{

	m_keyboardController = new PlayerKeyboardController();
	m_followPath = new FollowPathBehaviour();

	SetBehaviour(m_keyboardController);
	SetFriction(1.0f);

	m_playerTexture = LoadTexture("./assets/topdown-shooter/PNG/Man Old/manOld_stand.png");

	m_followPath->OnPathComplete([this]() {
		SetBehaviour(m_keyboardController);
		m_followPath->OnPathComplete(nullptr);
	});
}

Player::~Player()
{
	UnloadTexture(m_playerTexture);

	SetBehaviour(nullptr);

	delete m_keyboardController;
	delete m_followPath;
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

	GameObject::Draw();
}

void Player::SetPath(Path* p)
{
	m_path = p;
}

Player::State Player::GetState()
{
	if (m_behaviour == m_keyboardController)
		return State::KeyboardController;

	if (m_behaviour == m_followPath)
		return State::FollowPath;

	return State::NONE;
}

void Player::SetState(State newState)
{
	if (newState == State::KeyboardController)
	{
		SetBehaviour(m_keyboardController);
	}
	else if (newState == State::FollowPath)
	{
		m_followPath->SetPath(m_path);
		SetBehaviour(m_followPath);
	}
		
}