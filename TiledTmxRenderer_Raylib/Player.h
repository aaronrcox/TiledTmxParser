#pragma once

#include "GameObject.h"

class PlayerKeyboardController;
class FollowPathBehaviour;
class Path;

class Player : public GameObject
{
public:

	enum class State
	{
		NONE,
		KeyboardController,
		FollowPath
	};

	Player();
	virtual ~Player();

	virtual void Update(float deltaTime);
	virtual void Draw();

	void SetPath(Path* p);

	void SetState(State newState);
	State GetState();

protected:

	Texture2D m_playerTexture;

	PlayerKeyboardController* m_keyboardController = nullptr;
	FollowPathBehaviour* m_followPath = nullptr;
	Path* m_path;

private:
};

