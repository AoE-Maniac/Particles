#pragma once

#include <Kore/Graphics/Graphics.h>

void initRockets();
void deleteRockets();

void fireRocket(Kore::vec3 from, Kore::vec3 to);
void updateRockets(float deltaTime);
void renderRockets(Kore::mat4 V, Kore::mat4 P);
