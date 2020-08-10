#pragma once

#pragma once

#include "raymath.h"
#include <vector>

class Path
{
public:

	Path();
	~Path();

	void Add(Vector2 point);

	int Count();

	void Draw();

	Vector2& operator [] (int index);


protected:

	std::vector<Vector2> m_points;

private:
};