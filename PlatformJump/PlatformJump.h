#pragma once
#include <Component/SimpleScene.h>
#include "PlatformJumpCamera.h"
#include "Platform.h"

class PlatformJump : public SimpleScene
{
public:
	PlatformJump();
	~PlatformJump();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int powerUp, bool platform, bool isGuiElement);

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;
	
	void CheckLevelUp();
	void OnLevelUp();
	void ApplyGravity(float deltaTime);
	void IsAbovePlatform();
	void ApplyPowerUp(Platform::Platform* platform);
	void SecondWind();
	void CheckGameOver();
	void OnGameOver();

protected:
	const float FOW_MIN = 45;
	const float FOW_MAX = 120;
	const float FOW_DEFAULT = 60;
	Camera::Camera* camera;
	glm::mat4 projectionMatrix;

	/* GAME ENGINE */
	// Camera
	bool firstPerson;
	float fow;

	// Physics
	const float JUMP_VELOCITY = 6.0f;
	const float ENERGY_RETAIN = 0.5f;
	const float GRAVITY = -15.0f;
	float lastY;
	int bounces;
	bool isBouncing;
	bool hasJumped;

	// Distance
	const float LEVEL_DIST = 300.0f;
	const float LEVEL_DIST_MOD = 1.15f;
	float nextLevelDist;
	float distance;

	// Mechanics
	bool gameOver;
	bool printGameOverMessage;
	bool levelUp;
	int level;
	bool isAffected;
	int effect;

	// Environment
	std::vector <Platform::Platform*> lanes[5];
	const float PLATFORM_LIMIT_Z = 300;
	const float PLATFORM_WIDTH = 2.5f;
	const int MIN_LANES = 3;
	const int MAX_LANES = 5;
	const float MIN_SPEED_MOD = 0.1f;
	const float MAX_SPEED_MOD = 1.05f;
	const float SPEED_MOD = 3.5f;
	float minPlatformSpeed;
	float maxPlatformSpeed;
	int usedLanes;
	const float TIME_MAX_SPEED = 5.0f;
	float timeLeftAtMaxSpeed;
	float platformSpeed;

	/* PLAYER */
	// Lives
	const int MAX_LIVES = 3;
	int lives;

	// Fuel
	const float MAX_FUEL = 100.99f;
	const float FUEL_PENALTY_MOD = 0.2f;
	const float FUEL_BONUS_MOD = 0.1f;
	float fuel;

	// Sphere
	const float HALF_SPHERE_SIZE = 0.5f;
	const float SPEED_X = 5.5f;
	float speedY;
	float sphereX;
	float sphereY;
	bool isAbovePlatform;
};
