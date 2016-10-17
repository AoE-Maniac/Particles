#pragma once

#include <Kore/Graphics/Graphics.h>

void initGround();
void deleteGround();

Kore::vec3 getRandomGroundPosition();
Kore::vec3 getRandomGroundPosition(float angle);
void renderGround(Kore::mat4 V, Kore::mat4 P);