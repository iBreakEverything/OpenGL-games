#include "PlatformJump.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>

using namespace std;

PlatformJump::PlatformJump()
{
	gameOver = false;
	levelUp = true;
	level = 1;
	usedLanes = MIN_LANES;
	isAbovePlatform = true;
	sphereY = HALF_SPHERE_SIZE;
	lastY = sphereY;
	bounces = 0;
	speedY = 0.0f;
	isBouncing = false;
	hasJumped = false;
	fuel = MAX_FUEL;
	lives = MAX_LIVES;
	platformSpeed = 0.0f;
	minPlatformSpeed = 0.0f;
	maxPlatformSpeed = 20.0f;
	distance = 0.0f;
	nextLevelDist = LEVEL_DIST;
	sphereX = 0.0f;
	fow = 75;
	firstPerson = false;
	timeLeftAtMaxSpeed = -69.0f;
	isAffected = false;
	effect = -1;
	printGameOverMessage = false;
}

PlatformJump::~PlatformJump()
{
}

void PlatformJump::Init()
{
	fow = FOW_DEFAULT;
	camera = new Camera::Camera();
	camera->RotateFirstPerson_OY(RADIANS(180));
	camera->Set(glm::vec3(0, 2, -3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));


	{
		Mesh* mesh = new Mesh("player");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Shader* shader = new Shader("ShaderPlatformJump");
		shader->AddShader("Source/Laboratoare/PlatformJump/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/PlatformJump/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	for (int i = 0; i < usedLanes; i++) {
		lanes[i].push_back(new Platform::Platform(i, "ShaderPlatformJump"));
	}

	//std::vector <Platform::Platform*> 

	projectionMatrix = glm::perspective(RADIANS(fow), window->props.aspectRatio, 0.01f, 200.0f);
}

void PlatformJump::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void PlatformJump::Update(float deltaTimeSeconds)
{
	CheckGameOver();

	CheckLevelUp();

	IsAbovePlatform();

	if (sphereY < -5) {
		SecondWind();
	}

	ApplyGravity(deltaTimeSeconds);

	// speed powerup logic
	if (timeLeftAtMaxSpeed > 0) {
		timeLeftAtMaxSpeed -= deltaTimeSeconds;
	}
	else if (timeLeftAtMaxSpeed <= 0 && timeLeftAtMaxSpeed > -69) {
		platformSpeed = (minPlatformSpeed + maxPlatformSpeed) / 2;
		timeLeftAtMaxSpeed = -69;
		isAffected = false;
		effect = -1;
	}

	// camera update
	if (firstPerson) {
		camera->position = glm::vec3(sphereX, sphereY, 0.0f);
	}
	else {
		camera->RotateFirstPerson_OY(RADIANS(180));
		camera->Set(glm::vec3(0, 5, -7), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
		camera->TranslateRight(-sphereX);
		camera->TranslateUpword(sphereY - HALF_SPHERE_SIZE);
		{
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(sphereX, sphereY, 0));
			RenderSimpleMesh(meshes["player"], shaders["ShaderPlatformJump"], modelMatrix, 0, false, false);
		}
	}

	// render GUI
	{  // Fuel
		RenderSimpleMesh(meshes["box"], shaders["ShaderPlatformJump"], glm::mat4(1), 100, false, true);
		RenderSimpleMesh(meshes["box"], shaders["ShaderPlatformJump"], glm::mat4(1), 101, false, true);
	}
	{  // Lives
		if (lives) {
			RenderSimpleMesh(meshes["player"], shaders["ShaderPlatformJump"], glm::mat4(1), 69, false, true);
		}
		if (lives >= 2) {
			RenderSimpleMesh(meshes["player"], shaders["ShaderPlatformJump"], glm::mat4(1), 70, false, true);
		}
		if (lives >= 3) {
			RenderSimpleMesh(meshes["player"], shaders["ShaderPlatformJump"], glm::mat4(1), 71, false, true);
		}
	}

	// Add more platforms
	for (int i = 0; i < usedLanes; i++) {
		if (lanes[i].empty()) {
			lanes[i].push_back(new Platform::Platform(i, "ShaderPlatformJump"));
		}
		else {
			if (lanes[i].back()->platformCenterZ + lanes[i].back()->platformLength / 2 < PLATFORM_LIMIT_Z) {
				lanes[i].push_back(new Platform::Platform(lanes[i].back(), i, "ShaderPlatformJump"));
			}
		}
	}

	// Remove "out of view" platforms
	for (int i = 0; i < MAX_LANES; i++) {
		if (!lanes[i].empty()) {
			for (Platform::Platform* platform : lanes[i]) {
				if (platform->platformCenterZ + platform->platformLength / 2 < -5) {
					platform->~Platform();
					lanes[i].erase(lanes[i].begin());
				}
			}
		}
	}

	// Render and move platforms
	for (int i = 0; i < MAX_LANES; i++) {
		if (!lanes[i].empty()) {
			for (Platform::Platform* platform : lanes[i]) {
				glm::mat4 model = platform->modelMatrix;
				model = glm::scale(model, platform->scaleVector);
				RenderSimpleMesh(meshes["box"], shaders[platform->shaderName], model, platform->powerUp, true, false);
				platform->Move(deltaTimeSeconds, platformSpeed);
			}
		}
	}

	// Increment distance
	distance += deltaTimeSeconds * platformSpeed;
	fuel -= platformSpeed * (deltaTimeSeconds * 2 / (platformSpeed + 1));
}

void PlatformJump::FrameEnd()
{
}

void PlatformJump::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int powerUp, bool platform, bool isGuiElement)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// get shader location for uniform mat4 "Model"
	int ModelLocation = glGetUniformLocation(shader->program, "Model");

	// set shader uniform "Model" to modelMatrix
	glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// get shader location for uniform mat4 "View"
	int ViewLocation = glGetUniformLocation(shader->program, "View");

	// set shader uniform "View" to viewMatrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	glUniformMatrix4fv(ViewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// get shader location for uniform mat4 "Projection"
	int ProjectionLocation = glGetUniformLocation(shader->program, "Projection");

	// set shader uniform "Projection" to projectionMatrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	glUniformMatrix4fv(ProjectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glm::vec4 color = glm::vec4(1);

	if (platform) {
		switch (powerUp) {
		case -1:
			color = glm::vec4(0.1f, 0.325f, 1.0f, 1.0f);  // Blue
			break;
		case 0:
			color = glm::vec4(0.6f, 0.0f, 0.6f, 1.0f);  // Purple
			break;
		case 1:
			color = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f);  // Red
			break;
		case 2:
			color = glm::vec4(1.0f, 0.8f, 0.1f, 1.0f);  // Yellow
			break;
		case 3:
			color = glm::vec4(1.0f, 0.4f, 0.0f, 1.0f);  // Orange
			break;
		case 4:
			color = glm::vec4(0.0f, 0.6f, 0.2f, 1.0f);  // Green
			break;
		case 5:
			color = glm::vec4(1.0f, 0.5f, 0.83f, 1.0f);  // Pink
			break;
		default:
			break;
		}
	}
	else {
		float col = cos(Engine::GetElapsedTime() / 10.0f) / 2.0f + 1.0f;
		color = glm::vec4(0.2, col, 0.7f, 1.0f);
		if (isAffected) {
			if (effect) {
				color = glm::vec4(1.0f, 0.4f, 0.0f, 1.0f);  // Orange
			}
			else {
				color = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f);  // Red
			}
		}
	}

	int vertexColorLocation = glGetUniformLocation(shader->program, "Color");
	glUniform4fv(vertexColorLocation, 1, glm::value_ptr(color));

	int IsPlatformLocation = glGetUniformLocation(shader->program, "IsPlatform");
	glUniform1i(IsPlatformLocation, platform);

	int uTimeLocation = glGetUniformLocation(shader->program, "u_time");
	glUniform1f(uTimeLocation, Engine::GetElapsedTime());

	int effectLocation = glGetUniformLocation(shader->program, "effect");
	if (isGuiElement) {
		glUniform1f(effectLocation, powerUp);
	}
	else {
		glUniform1f(effectLocation, (float)effect);
	}

	glm::ivec2 resolution = window->GetResolution();
	int resVectorLocation = glGetUniformLocation(shader->program, "res");
	glUniform2fv(resVectorLocation, 1, glm::value_ptr(glm::vec2(resolution.x, resolution.y)));

	int FuelLocation = glGetUniformLocation(shader->program, "fuel");
	glUniform2fv(FuelLocation, 1, glm::value_ptr(glm::vec2(fuel, MAX_FUEL)));

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void PlatformJump::OnInputUpdate(float deltaTime, int mods)
{
	{  // FOV controls
		if (window->KeyHold(GLFW_KEY_1)) {
			if (fow > FOW_MIN) {
				fow--;
				projectionMatrix = glm::perspective(RADIANS(fow), window->props.aspectRatio, 0.01f, 200.0f);
			}
		}

		if (window->KeyHold(GLFW_KEY_2)) {
			if (fow < FOW_MAX) {
				fow++;
				projectionMatrix = glm::perspective(RADIANS(fow), window->props.aspectRatio, 0.01f, 200.0f);
			}
		}

		if (window->KeyHold(GLFW_KEY_3)) {
			if (fow != FOW_DEFAULT) {
				fow = FOW_DEFAULT;
				projectionMatrix = glm::perspective(RADIANS(fow), window->props.aspectRatio, 0.01f, 200.0f);
			}
		}
	}

	// Control sphere speed on OZ (platform speed) using W and S
	if (timeLeftAtMaxSpeed <= 0 && !gameOver) {
		if (window->KeyHold(GLFW_KEY_W)) {
			if (platformSpeed < maxPlatformSpeed) {
				platformSpeed += deltaTime * SPEED_MOD;
			}
			else {
				platformSpeed = maxPlatformSpeed;
			}
		}
		if (window->KeyHold(GLFW_KEY_S)) {
			if (platformSpeed > minPlatformSpeed) {
				platformSpeed -= deltaTime * SPEED_MOD;
			}
			else {
				platformSpeed = minPlatformSpeed;
			}
		}
	}

	// Control sphere position on OX using A and D
	if (!gameOver) {
		if (window->KeyHold(GLFW_KEY_A)) {
			sphereX += deltaTime * SPEED_X;
		}
		if (window->KeyHold(GLFW_KEY_D)) {
			sphereX -= deltaTime * SPEED_X;
		}
	}
}

void PlatformJump::OnKeyPress(int key, int mods)
{
	// handle jumping
	if (key == GLFW_KEY_SPACE && !isBouncing) {
		isBouncing = true;
		speedY = JUMP_VELOCITY;
		hasJumped = true;
		bounces = 0;
	}
	if (key == GLFW_KEY_C) {
		firstPerson = !firstPerson;
		if (!firstPerson) {
			camera->RotateFirstPerson_OY(RADIANS(180));
			camera->Set(glm::vec3(0, 2, -3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
			camera->TranslateRight(-sphereX);
			camera->TranslateUpword(sphereY - HALF_SPHERE_SIZE);
		}
		else {
			camera->position = glm::vec3(sphereX, sphereY, 0.0f);
		}
	}
}

void PlatformJump::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void PlatformJump::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		if (window->GetSpecialKeyState() == 0) {
			camera->RotateFirstPerson_OX(-sensivityOX * deltaY);
			camera->RotateFirstPerson_OY(-sensivityOY * deltaX);
		}

		if (window->GetSpecialKeyState() && GLFW_MOD_CONTROL) {
			camera->RotateThirdPerson_OX(-sensivityOX * deltaY);
			camera->RotateThirdPerson_OY(-sensivityOY * deltaX);
		}

	}
}

void PlatformJump::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void PlatformJump::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void PlatformJump::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void PlatformJump::OnWindowResize(int width, int height)
{
}

/* Check for level up */
void PlatformJump::CheckLevelUp()
{
	// Distance based level uping
	if (distance > nextLevelDist) {
		nextLevelDist = LEVEL_DIST * pow(LEVEL_DIST_MOD, level);
		OnLevelUp();
	}
}

/* Sets variables on level up */
void PlatformJump::OnLevelUp()
{
	// Increment level
	level++;

	// Set new speed values
	minPlatformSpeed = maxPlatformSpeed * MIN_SPEED_MOD;
	maxPlatformSpeed *= MAX_SPEED_MOD;
	if (platformSpeed < minPlatformSpeed) {
		platformSpeed = minPlatformSpeed;
	}

	// 1UP
	if (lives < MAX_LIVES) {
		lives++;
	}

	// Set platform number
	if (level == 2) {
		usedLanes = (MAX_LANES + MIN_LANES) / 2;
	}
	else if (level == 3) {
		usedLanes = MAX_LANES;
	}
	else {
		usedLanes = (rand() % (MAX_LANES - MIN_LANES + 1)) + MIN_LANES;
	}
}

/* Sphere gravity logic */
void PlatformJump::ApplyGravity(float deltaTime)
{
	float acc = GRAVITY * deltaTime;

	// Jump from 0 height
	if (hasJumped) {
		hasJumped = false;
		sphereY += (speedY * deltaTime) + (acc * deltaTime * deltaTime / 2);
	}

	// Lands on platform
	if ((sphereY - HALF_SPHERE_SIZE != 0) || (lastY - HALF_SPHERE_SIZE != 0)) {
		lastY = sphereY;
		if ((sphereY - HALF_SPHERE_SIZE == 0) && isAbovePlatform) {
			speedY = -pow(ENERGY_RETAIN, bounces + 1) * speedY;
			bounces++;
		}
		speedY += acc;
		sphereY += (speedY * deltaTime) + (acc * deltaTime * deltaTime / 2);
		if (sphereY - HALF_SPHERE_SIZE < 0 && isAbovePlatform) {
			sphereY = HALF_SPHERE_SIZE;
		}
	}

	if ((sphereY - HALF_SPHERE_SIZE == 0) && (lastY - HALF_SPHERE_SIZE == 0) && !isAbovePlatform) {
		speedY += acc;
		sphereY += (speedY * deltaTime) + (acc * deltaTime * deltaTime / 2);
	}

	// Resets variables
	if ((sphereY - HALF_SPHERE_SIZE == 0) && (lastY - HALF_SPHERE_SIZE == 0) && !hasJumped) {
		bounces = 0;
		isBouncing = false;
	}
}

/* Checks if sphere is on top of a platform */
void PlatformJump::IsAbovePlatform()
{
	if (sphereY < 0) {
		return;
	}
	int currentLane = -1;
	if (sphereX > -PLATFORM_WIDTH * 5 / 2) {
		if (sphereX < -PLATFORM_WIDTH * 3 / 2) {
			currentLane = 3;
		}
		else if (sphereX < -PLATFORM_WIDTH / 2) {
			currentLane = 1;
		}
		else if (sphereX < PLATFORM_WIDTH / 2) {
			currentLane = 0;
		}
		else if (sphereX < PLATFORM_WIDTH * 3 / 2) {
			currentLane = 2;
		}
		else if (sphereX < PLATFORM_WIDTH * 5 / 2) {
			currentLane = 4;
		}
	}


	if (currentLane == -1) {
		isAbovePlatform = false;
		return;
	}

	int platformCount = lanes[currentLane].size();
	float lowerBound2 = 1;

	if (platformCount <= 0) {
		isAbovePlatform = false;
		return;
	}

	Platform::Platform* platform1 = lanes[currentLane].at(0);

	float lowerBound1 = platform1->platformCenterZ - platform1->platformLength / 2;
	float upperBound1 = platform1->platformCenterZ + platform1->platformLength / 2;

	Platform::Platform* platform2 = lanes[currentLane].at(0);;
	if (platformCount >= 2) {
		platform2 = lanes[currentLane].at(1);
		lowerBound2 = platform2->platformCenterZ - platform2->platformLength / 2;
	}

	// Case 1: first platform is in from of the sphere
	if (lowerBound1 > 0) {
		isAbovePlatform = false;
	}
	else {
		// Case 2: above platform1
		if (upperBound1 > 0) {
			isAbovePlatform = true;
			ApplyPowerUp(platform1);
		}
		else {
			if (platformCount >= 2) {
				// Case 3: between platform1 and platform2
				if (lowerBound2 > 0) {
					isAbovePlatform = false;
				}
				// Case 4: above platform2
				else {
					isAbovePlatform = true;
					ApplyPowerUp(platform2);
				}
			}
		}
	}
}

/* Applies power up to sphere */
void PlatformJump::ApplyPowerUp(Platform::Platform* platform) {
	if (platform->powerUpApplied || sphereY != HALF_SPHERE_SIZE) {
		return;
	}
	switch (platform->powerUp) {
	case 1:
		lives--;
		break;
	case 2:
		fuel -= MAX_FUEL * FUEL_PENALTY_MOD;
		break;
	case 3:
		timeLeftAtMaxSpeed = TIME_MAX_SPEED;
		platformSpeed = maxPlatformSpeed;
		isAffected = true;
		effect = 1;
		break;
	case 4:
		fuel = (MAX_FUEL > (fuel + MAX_FUEL * FUEL_BONUS_MOD)) ? (fuel + MAX_FUEL * FUEL_BONUS_MOD) : MAX_FUEL;
		break;
	case 5:
		lives = (MAX_LIVES > (lives + 1)) ? (lives + 1) : MAX_LIVES;
		break;
	default:
		break;
	}
	platform->powerUp = 0;
	platform->powerUpApplied = true;
}

/* FIGHT FOR YOUR LIFE! */
void PlatformJump::SecondWind() {
	if (!gameOver) {
		lives--;
		sphereY = 30;
	}
}

/* Check if you died */
void PlatformJump::CheckGameOver() {
	if (lives == 0 || fuel <= 0) {
		gameOver = true;
		OnGameOver();
	}
}

/* You Died! */
void PlatformJump::OnGameOver() {
	if (!printGameOverMessage) {
		printGameOverMessage = true;
		sphereX = 10.0f;
		sphereY = HALF_SPHERE_SIZE;
		speedY = 0.0f;
		platformSpeed = 0.0f;
		isAffected = true;
		effect = 0;
		printf("\n\tGAME OVER\n");
		printf("\t Level: %d\n", level);
		printf("\t Distance: %f\n", distance);
	}
}