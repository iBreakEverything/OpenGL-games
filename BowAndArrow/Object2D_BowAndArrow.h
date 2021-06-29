#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

namespace Object2D_BowAndArrow
{
	Mesh* CreateBalloon(std::string name, glm::vec3 color);
	Mesh* CreateShuriken(std::string name, glm::vec3 color);
	Mesh* CreatePlayer();
	Mesh* CreateArrow();
	Mesh* CreatePowerBar();
}
