#include "Path.h"

Path::Path()
{

}

Path::~Path()
{

}

void Path::Add(Vector2 point)
{
	m_points.push_back(point);
}

int Path::Count()
{
	return m_points.size();
}

Vector2& Path::operator [] (int index)
{
	return m_points[index];
}

void Path::Draw()
{

	// draw path lines
	for (int i = 1; i < m_points.size(); i++)
	{
		DrawLine(m_points[i].x, m_points[i].y, m_points[i - 1].x, m_points[i - 1].y, BLACK);
	}

	for (int i = 0; i < m_points.size(); i++)
	{
		DrawCircle(m_points[i].x, m_points[i].y, 4, BLACK);
	}
}