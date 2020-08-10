#pragma once

#include "Behaviour.h"
#include "raymath.h"
#include <functional>

class Path;

class FollowPathBehaviour : public Behaviour
{
public:

	FollowPathBehaviour();
	virtual ~FollowPathBehaviour();

	virtual void Update(GameObject* obj, float deltaTime);
	virtual void Draw(GameObject* obj);


	Path* GetPath();
	void SetPath(Path* path);

	void OnPathComplete(std::function<void()> callback);

protected:

	Path* m_path = nullptr;
	int m_currentPathIndex = 0;

	std::function<void()> m_onPathComplete;

private:
};