#pragma once

#include "GameObject.h"

class PlayerKeyboardController;

class Player : public GameObject
{
public:

	Player();
	virtual ~Player();

	virtual void Update(float deltaTime);
	virtual void Draw();

protected:

	Texture2D m_playerTexture;

	PlayerKeyboardController* m_keyboardController = nullptr;

private:
};

