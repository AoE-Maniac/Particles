#pragma once

#include <Kore/Graphics/Graphics.h>

void initGround();
void deleteGround();

Kore::vec3 getRandomPosition();
Kore::vec3 getRandomSidePosition(bool left);
Kore::vec3 getRandomCirclePosition();
Kore::vec3 getCirclePosition(float angle);
void renderGround(Kore::mat4 V, Kore::mat4 P);