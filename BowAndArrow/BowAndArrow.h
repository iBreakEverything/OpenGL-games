#pragma once

#include <Component/SimpleScene.h>
#include <string>
#include <Core/Engine.h>

class BowAndArrow : public SimpleScene
{
public:
	BowAndArrow();
	~BowAndArrow();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;
	void checkCollision();
	void increaseDiff();
	void printScore();
	
protected:  // TODO FIXME
	const float DELTA_MULT = 100;
	const int INITIAL_ITEMS = 10;
	const float PLAYER_SIZE = 100;
	const float PLAYER_SPACE = 0.2f;
	const float BALLOON_OFFSET = 100;
	const float MAX_POWER = 100;
	const float ARROW_LENGTH = 100;
	glm::mat3 modelMatrix;
	float rotation;
	float power;
	bool powerUp;
	int score;
	int maxItems;
	int lastScore;
	int level;
	bool pause;
};
