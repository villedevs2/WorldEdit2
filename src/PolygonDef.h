#pragma once

#include <glm.hpp>

class PolygonDef
{
public:
	static const int CAPACITY_MAX = 8;

	enum Status
	{
		POLY_STATUS_OK = 0,
		POLY_STATUS_CAPACITY,
		POLY_STATUS_NONCONVEX,
	};

	PolygonDef();
	~PolygonDef();

	PolygonDef::Status insertPoint(glm::vec2 point);
	void deleteLatest();
	int getNumPoints();
	void reset();
	glm::vec2 getPoint(int index);
	bool convexTest(glm::vec2 point);
	bool fullConvexTest();
	void edit(int index, glm::vec2 point);
	bool lineSide(glm::vec2& p0, glm::vec2& p1, glm::vec2& point);
	bool isPointInside(glm::vec2 point);
	void calculateBounds(float* minx, float* maxx, float* miny, float* maxy);
	bool isPointOnEdge(glm::vec2 point, int v1, int v2, float threshold);

private:
	glm::vec2 m_points[CAPACITY_MAX];
	int m_num_points;
};