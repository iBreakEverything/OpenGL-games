#include "Object2D_BowAndArrow.h"

#include <Core/Engine.h>

float pi = 3.1415f;
float radius = 50;
int points = 25;

std::vector<VertexFormat> CreateVerticesBalloon(const int points, const float radius, const glm::vec3 color)
{
	std::vector<VertexFormat> vertices;

	for (float i = 0; i < points; i++)
	{
		vertices.push_back(
			VertexFormat(glm::vec3(
				(radius / 1.3f * cos(i * 2 * pi / points)),
				(radius * sin(i * 2 * pi / points)),
				0
			), color + (i * i - 2 * (float)points * i + (float)points * points) / (float)(points * points * 3))
		);
	}

	vertices.push_back(VertexFormat(glm::vec3(-radius * 1 / 10, -radius * 8 / 10, 0), color));
	vertices.push_back(VertexFormat(glm::vec3(-radius / 4, -radius * 13 / 10, 0), color));
	vertices.push_back(VertexFormat(glm::vec3(radius / 5, -radius * 13 / 10, 0), color));

	return vertices;
}

std::vector<unsigned short> CreateIndicesBalloon(const int points)
{
	std::vector<unsigned short> indices;
	const int knot = 3;

	for (unsigned short i = 0; i < points; i++)
	{
		indices.push_back(0);
		indices.push_back(i);
		indices.push_back((i + 1 < points) ? i + 1 : 1);
	}

	for (unsigned short i = points; i < points + knot; i++)
	{
		indices.push_back(i);
	}

	return indices;
}

Mesh* Object2D_BowAndArrow::CreateBalloon(std::string name, glm::vec3 color)
{
	int points = 25;

	std::vector<VertexFormat> vertices = CreateVerticesBalloon(points, radius, color);

	std::vector<unsigned short> indices = CreateIndicesBalloon(points);

	Mesh* balloon = new Mesh(name);
	balloon->SetDrawMode(GL_TRIANGLES);
	balloon->InitFromData(vertices, indices);
	return balloon;
}

Mesh* Object2D_BowAndArrow::CreateShuriken(std::string name, glm::vec3 color)
{
	float slope = 50;
	float riseX = 10;
	float riseY = 40;
	float offset = 0.35f;
	glm::vec3 light = color + offset;
	glm::vec3 dark = color - offset;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(glm::vec3(0, 0, 0), color),			// center

		VertexFormat(glm::vec3(slope, slope, 0), light),	// slope x+ y+
		VertexFormat(glm::vec3(riseX, riseY, 0), dark),		// rise  x+ y+

		VertexFormat(glm::vec3(-slope, slope, 0), light),	// slope x- y+
		VertexFormat(glm::vec3(-riseY, riseX, 0), dark),	// rise  x- y+

		VertexFormat(glm::vec3(-slope, -slope, 0), light),	// slope x- y-
		VertexFormat(glm::vec3(-riseX, -riseY, 0), dark),	// rise  x- y-

		VertexFormat(glm::vec3(slope, -slope, 0), light),	// slope x+ y-
		VertexFormat(glm::vec3(riseY, -riseX, 0), dark),	// rise  x+ y-
	};

	std::vector<unsigned short> indices = {
		0,1,2,
		0,3,4,
		0,5,6,
		0,7,8,
	};

	Mesh* shuriken = new Mesh(name);
	shuriken->SetDrawMode(GL_TRIANGLES);
	shuriken->InitFromData(vertices, indices);
	return shuriken;
}

Mesh* Object2D_BowAndArrow::CreatePlayer()
{
	std::vector<VertexFormat> vertices;

	int bowPoints = 0;

	for (float i = 0; i <= pi + 0.01f; i += 0.01f)
	{
		bowPoints++;
		vertices.push_back(
			VertexFormat(glm::vec3(
				(radius / 1.3f * sin(i)),
				(radius * cos(i)),
				0
			), glm::vec3(0))
		);
	}

	std::vector<unsigned short> indices;

	for (unsigned short i = 0; i <= bowPoints; i++)
	{
		indices.push_back(i);
	}
	indices.push_back(0);
	Mesh* player = new Mesh("player");
	player->SetDrawMode(GL_LINE_STRIP);
	player->InitFromData(vertices, indices);
	return player;
}

Mesh* Object2D_BowAndArrow::CreateArrow()
{
	float shaftSize = 5;
	float arrowLength = 100;
	float arrowWidth = 20;
	float headLength = 30;
	glm::vec3 color = glm::vec3(0);

	std::vector<VertexFormat> vertices =
	{
		
		VertexFormat(glm::vec3(arrowLength, 0, 0), color),							// tip head
		VertexFormat(glm::vec3(arrowLength -arrowWidth, 0, 0), color),				// tip bottom
		VertexFormat(glm::vec3(arrowLength -headLength, arrowWidth / 2, 0), color),	// up
		VertexFormat(glm::vec3(arrowLength -headLength, -arrowWidth / 2, 0), color),// down
		VertexFormat(glm::vec3(arrowLength -arrowWidth, shaftSize / 2, 0), color),	// shaft tip up
		VertexFormat(glm::vec3(arrowLength -arrowWidth, -shaftSize / 2, 0), color),	// shaft tip down
		VertexFormat(glm::vec3(0, shaftSize / 2, 0), color),						// shaft end up
		VertexFormat(glm::vec3(0, -shaftSize / 2, 0), color),						// shaft end down
	};

	std::vector<unsigned short> indices = {
		0,2,1,
		0,1,3,
		4,6,5,
		5,6,7,
	};

	Mesh* arrow = new Mesh("arrow");
	arrow->SetDrawMode(GL_TRIANGLES);
	arrow->InitFromData(vertices, indices);
	return arrow;
}

Mesh* Object2D_BowAndArrow::CreatePowerBar()
{
	glm::vec3 color = glm::vec3(0.3f, 0.3f, 0.3f);

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(glm::vec3(50, 5, 0), color),
		VertexFormat(glm::vec3(50, -5, 0), color),
		VertexFormat(glm::vec3(-50, -5, 0), color),
		VertexFormat(glm::vec3(-50, 5, 0), color),
	};

	std::vector<unsigned short> indices = {
		0,3,1,
		1,3,2,
	};

	Mesh* bar = new Mesh("bar");
	bar->SetDrawMode(GL_TRIANGLES);
	bar->InitFromData(vertices, indices);
	return bar;
}
