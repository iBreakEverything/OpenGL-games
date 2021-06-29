#pragma once
#include <include/glm.h>
#include <include/math.h>

namespace Platform
{
	class Platform
	{
		public:
			Platform(int lane, std::string shaderName)
			{
				this->shaderName = shaderName;
				this->platformLength = 50;
				glm::mat4 modelMat = glm::mat4(1);
				glm::vec3 translate, scale;
				this->powerUp = -1;
				this->powerUpApplied = false;

				if (lane == 0) {
					this->platformLength = 50;
				}

				if ((lane == 1) || (lane == 2)) {
					this->platformLength = 40;
				}

				if ((lane == 3) || (lane == 4)) {
					this->platformLength = 30;
					this->platformCenterZ += this->PLATFORM_LIMIT_Z;
				}

				this->platformCenterZ = this->platformLength / 2 - 3;
				
				translate = glm::vec3(
					laneOffset[lane] * this->PLATFORM_WIDTH,
					-(this->PLATFORM_HEIGHT / 2),
					this->platformCenterZ
				);
				scale = glm::vec3(
					this->PLATFORM_WIDTH,
					this->PLATFORM_HEIGHT,
					this->platformLength
				);
				
				modelMat = glm::translate(modelMat, translate);
				this->scaleVector = scale;
				this->modelMatrix = modelMat;
			}

			Platform(Platform* lastPlatform, int lane, std::string shaderName)
			{
				this->shaderName = shaderName;

				int randomPowerUp = rand() % 100;
				if (randomPowerUp < 50) {
					this->powerUp = -1;
				}
				else if (randomPowerUp < 55) {
					this->powerUp = 1;
				}
				else if (randomPowerUp < 65) {
					this->powerUp = 2;
				}
				else if (randomPowerUp < 75) {
					this->powerUp = 3;
				}
				else if (randomPowerUp < 95) {
					this->powerUp = 4;
				}
				else if (randomPowerUp < 100) {
					this->powerUp = 5;
				}
				this->powerUpApplied = false;

				glm::mat4 modelMat = glm::mat4(1);
				glm::vec3 translate, scale;

				this->platformLength = (float) (rand() % (this->MAX_PLATFORM_LENGTH_MOD - this->MIN_PLATFORM_LENGTH_MOD + 1)) + this->MIN_PLATFORM_LENGTH_MOD;
				
				float gapSize = (float) (rand() % this->MAX_PLATFORM_GAP);
				this->platformCenterZ = lastPlatform->platformCenterZ + lastPlatform->platformLength / 2 + gapSize + this->platformLength / 2;

				translate = glm::vec3(
					laneOffset[lane] * this->PLATFORM_WIDTH,
					-(this->PLATFORM_HEIGHT / 2),
					this->platformCenterZ
				);
				scale = glm::vec3(
					this->PLATFORM_WIDTH,
					this->PLATFORM_HEIGHT,
					this->platformLength
				);

				modelMat = glm::translate(modelMat, translate);
				this->scaleVector = scale;
				this->modelMatrix = modelMat;
			}

			~Platform()
			{ }

			void Move(float deltaTime, float speed)
			{
				this->platformCenterZ -= deltaTime * speed;

				glm::vec3 translate = glm::vec3(
					0,
					0,
					- deltaTime * speed
				);

				this->modelMatrix = glm::translate(this->modelMatrix, translate);
			}

		public:
			std::string shaderName;
			glm::mat4 modelMatrix;
			glm::vec3 scaleVector;
			float platformCenterZ;
			float platformLength;
			/* powerUp Legend
			* Code	%		Description		Color
			* --------------------------------
			* -1	50%		Basic			(Blue)
			*  0	-%		Touched			(Purple)
			*  1	5%		Life--			(Red)
			*  2	10%		Fuel--			(Yellow)
			*  3	10%		Max Speed Lock	(Orange)
			*  4	20%		Fuel++			(Green)
			*  5	5%		Life++			(Pink)
			*/
			int powerUp;
			bool powerUpApplied;

		protected:
			const int laneOffset[5] = { 0, -1, 1, -2, 2 };
			const float PLATFORM_LIMIT_Z = 300;
			const float PLATFORM_WIDTH = 2.5f;
			const float PLATFORM_HEIGHT = 0.5f;
			const int MIN_PLATFORM_LENGTH_MOD = 10;
			const int MAX_PLATFORM_LENGTH_MOD = 30;
			const int MAX_PLATFORM_GAP = 15;
	};
}