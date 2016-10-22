#pragma once

#include <Kore/Graphics/Graphics.h>

void initRockets(void(*emptyFunc)());
void deleteRockets();

void fireRocketRaw(Kore::u8* params);
void fireRocket(Kore::vec3 from, Kore::vec3 to);
void updateRockets(float deltaTime);
void renderRockets(Kore::mat4 V, Kore::mat4 P);
