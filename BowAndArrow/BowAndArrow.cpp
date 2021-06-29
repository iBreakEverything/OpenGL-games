#include "BowAndArrow.h"

#include <vector>
#include <iostream>

#include <Core/Engine.h>
#include "Transform2D_BowAndArrow.h"
#include "Object2D_BowAndArrow.h"

using namespace std;

struct objectProps {
	string type;
	int id;
	float x, y, scaleX, scaleY, roatation;
	bool check;
};

vector<objectProps> objects;
objectProps player;
objectProps arrow;
objectProps bar;

BowAndArrow::BowAndArrow()  // TODO FIXME
{
	rotation = 0;
	power = 0;
	powerUp = true;
	score = 0;
	maxItems = 10;
	lastScore = -1;
	pause = false;
	level = 0;
}

BowAndArrow::~BowAndArrow()
{
}

void BowAndArrow::Init()
{
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	player = { "player", 0, resolution.x * PLAYER_SPACE / 2, (float)resolution.y / 2, 1, 1, 0, false };
	Mesh* playerMesh = Object2D_BowAndArrow::CreatePlayer();
	AddMeshToList(playerMesh);

	arrow = { "arrow", 0, player.x, player.y, 1, 1, 0, false };
	Mesh* arrowMesh = Object2D_BowAndArrow::CreateArrow();
	AddMeshToList(arrowMesh);

	bar = { "bar", 0, player.x, player.y - PLAYER_SIZE, 1, 1, 0, false };
	Mesh* barMesh = Object2D_BowAndArrow::CreatePowerBar();
	AddMeshToList(barMesh);

	for (int i = 0; i < maxItems; i++)
	{
		float x = ((float)rand());
		while (x > resolution.x - resolution.x * PLAYER_SPACE)
			x -= resolution.x - resolution.x * PLAYER_SPACE;
		x += resolution.x * PLAYER_SPACE;

		objects.push_back({"balloon", i, x, 0, 0.5, 0.5, 0, false });
		Mesh* balloon = Object2D_BowAndArrow::CreateBalloon("balloon" + std::to_string(i), glm::vec3(1, 0, 0));
		AddMeshToList(balloon);
	}
}

void BowAndArrow::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0.644f, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

objectProps checkBalloonCollision(objectProps* balloon, float x, float y)
{
	const float radius = 50;
	const float points = 7;
	const float pi = 3.1415f;
	for (float i = 0; i < points; i++)
	{
		float distanceArrowCenter = sqrt(powf(x - balloon->x, 2) + powf(y - balloon->y, 2));
		float distanceLineCenter = (radius / 1.3f * cos((i + 1) * 2 * pi / points) * balloon->scaleX);
		if (distanceLineCenter > distanceArrowCenter)
		{
			balloon->check = true;
		}
	}
	return *balloon;
}

void BowAndArrow::checkCollision()
{
	float arrowTipX = arrow.x + ARROW_LENGTH * cos(arrow.roatation);
	float arrowTipY = arrow.y + ARROW_LENGTH * sin(arrow.roatation);
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i].type == "balloon")
		{
			objects[i] = checkBalloonCollision(&objects[i], arrowTipX, arrowTipY);
		}
	}
}

void BowAndArrow::increaseDiff()
{
	if (score < 0)
		return;
	level = sqrt(score);
	maxItems = INITIAL_ITEMS + level * 3;
	glm::ivec2 resolution = window->GetResolution();

	while (maxItems > objects.size()) {
		float x = ((float)rand());
		while (x > resolution.x - resolution.x * PLAYER_SPACE)
			x -= resolution.x - resolution.x * PLAYER_SPACE;
		x += resolution.x * PLAYER_SPACE;
		float y = (rand() % 30);
		int i = objects.size();
		objects.push_back({ "balloon", i, x, y, 0.5, 0.5, 0, false });
		Mesh* balloon = Object2D_BowAndArrow::CreateBalloon("balloon" + std::to_string(i), glm::vec3(1, 1, 0));
		AddMeshToList(balloon);
	}
}

void BowAndArrow::printScore()
{
	if (lastScore != score) {
		lastScore = score;
		printf("Score: %d\n", score);
	}
}

void BowAndArrow::Update(float deltaTimeSeconds)
{
	if (pause)
	{
		return;
	}
	printScore();
	glm::ivec2 resolution = window->GetResolution();
	checkCollision();
	increaseDiff();
	float speed = 2;
	{  // bow
		modelMatrix = Transform2D_BowAndArrow::Translate(player.x, player.y);
		modelMatrix *= Transform2D_BowAndArrow::Scale(player.scaleX, player.scaleY);
		modelMatrix *= Transform2D_BowAndArrow::Rotate(player.roatation);
		RenderMesh2D(meshes[player.type], shaders["VertexColor"], modelMatrix);
	}
	{  // bar
		bar.scaleX = power / MAX_POWER;
		modelMatrix = Transform2D_BowAndArrow::Translate(bar.x, bar.y);
		modelMatrix *= Transform2D_BowAndArrow::Scale(bar.scaleX, bar.scaleY);
		RenderMesh2D(meshes[bar.type], shaders["VertexColor"], modelMatrix);
	}
	{  // arrow
		if (arrow.check)
		{
			arrow.x += deltaTimeSeconds * ARROW_LENGTH * (speed + power/10) * cos(arrow.roatation);
			arrow.y += deltaTimeSeconds * ARROW_LENGTH * (speed + power/10) * sin(arrow.roatation);
			float arrowDelay = 100;
			if (arrow.x - arrowDelay > resolution.x || arrow.y - arrowDelay > resolution.y || arrow.x + arrowDelay < 0 || arrow.y + arrowDelay < 0)
			{
				power = 0;
				arrow.x = player.x;
				arrow.y = player.y;
				arrow.roatation = player.roatation;
				arrow.check = false;
			}
		}
		modelMatrix = Transform2D_BowAndArrow::Translate(arrow.x, arrow.y);
		modelMatrix *= Transform2D_BowAndArrow::Scale(arrow.scaleX, arrow.scaleY);
		modelMatrix *= Transform2D_BowAndArrow::Rotate(arrow.roatation);
		RenderMesh2D(meshes[arrow.type], shaders["VertexColor"], modelMatrix);
	}
	{  // Objects
		glm::ivec2 resolution = window->GetResolution();

		for (int i = 0; i < objects.size(); i++)
		{
			if (objects[i].type == "balloon")  // Balloons
			{
				if (!objects[i].check)
				{
					if (objects[i].y > (float)resolution.y + BALLOON_OFFSET)
						objects[i].y = 0 - BALLOON_OFFSET;
					objects[i].y += deltaTimeSeconds * speed * 100;
					modelMatrix = Transform2D_BowAndArrow::Translate(objects[i].x, objects[i].y);
					modelMatrix *= Transform2D_BowAndArrow::Scale(objects[i].scaleX, objects[i].scaleY);
					RenderMesh2D(meshes["balloon" + std::to_string(objects[i].id)], shaders["VertexColor"], modelMatrix);
				}
				else
				{
					if (objects[i].scaleX > 0.2f)
					{
						objects[i].scaleX -= 0.1f;
					}
					if (objects[i].y < 0 - BALLOON_OFFSET)
					{
						objects[i].y = 0 - BALLOON_OFFSET;
						objects[i].scaleX = 0.5f;
						objects[i].check = false;
						if (objects[i].id < 10)
							score++;
						else
							score -= 3;
					}
					objects[i].y -= deltaTimeSeconds * speed * 100;
					modelMatrix = Transform2D_BowAndArrow::Translate(objects[i].x, objects[i].y);
					modelMatrix *= Transform2D_BowAndArrow::Scale(objects[i].scaleX, objects[i].scaleY);
					RenderMesh2D(meshes["balloon" + std::to_string(objects[i].id)], shaders["VertexColor"], modelMatrix);
				}
			}
		}
	}
}

void BowAndArrow::FrameEnd()
{

}

void BowAndArrow::OnInputUpdate(float deltaTime, int mods)
{
	glm::ivec2 resolution = window->GetResolution();
	if (window->MouseHold(GLFW_MOUSE_BUTTON_1))
	{
		if (!arrow.check)
		{
			if (power <= 0)
				powerUp = true;
			if (power >= MAX_POWER)
				powerUp = false;

			if (powerUp)
			{
				power += deltaTime * DELTA_MULT;
				if (power > MAX_POWER)
					power = MAX_POWER;
			}
			else
			{
				power -= deltaTime * DELTA_MULT;
				if (power < 0)
					power = 0;
			}
		}
	}
	if (window->KeyHold(GLFW_KEY_W) || window->KeyHold(GLFW_KEY_UP))
	{
		if (player.y < resolution.y - PLAYER_SIZE / 2)
		{
			player.y += deltaTime * DELTA_MULT * 2;
			if (!arrow.check)
				arrow.y += deltaTime * DELTA_MULT * 2;
		}
		if (player.y > resolution.y - PLAYER_SIZE / 2)
		{
			player.y = resolution.y - PLAYER_SIZE / 2;
			if (!arrow.check)
				arrow.y = resolution.y - PLAYER_SIZE / 2;
		}
	}
	if (window->KeyHold(GLFW_KEY_S) || window->KeyHold(GLFW_KEY_DOWN))
	{
		if (player.y > PLAYER_SIZE / 2)
		{
			player.y -= deltaTime * 150;
			if (!arrow.check)
				arrow.y -= deltaTime * 150;
		}
		if (player.y < PLAYER_SIZE / 2)
		{
			player.y = PLAYER_SIZE / 2;
			if (!arrow.check)
				arrow.y = PLAYER_SIZE / 2;
		}
	}
}

void BowAndArrow::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_P)
	{
		pause = !pause;
	}
}

void BowAndArrow::OnKeyRelease(int key, int mods)
{
}

void BowAndArrow::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	glm::ivec2 resolution = window->GetResolution();
	float angle = (float)atan2(mouseY + (player.y - resolution.y), mouseX - resolution.x / 2 * PLAYER_SPACE);
	player.roatation = -angle;
	if (!arrow.check)
		arrow.roatation = -angle;
}

void BowAndArrow::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}

void BowAndArrow::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	if (IS_BIT_SET(button,GLFW_MOUSE_BUTTON_1))
	{
		if (!arrow.check)
		{
			powerUp = true;
			arrow.check = true;
		}
	}
}

void BowAndArrow::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void BowAndArrow::OnWindowResize(int width, int height)
{
}
