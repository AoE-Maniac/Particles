#pragma once

#include <Kore/Graphics/Graphics.h>

void initParticleSystem();
void deleteParticleSystem();

int addParticleEmitter(Kore::vec3 emitPos, float radius, Kore::vec3 emitDir, float spread, float maxRot, float minSpeed, float maxSpeed, float gravMin, float gravMax, float rateMin, float rateMax, float minTtl, float maxTtl, float minSize, float maxSize, Kore::vec4 colorSMin, Kore::vec4 colorSMax, Kore::vec4 colorEMin, Kore::vec4 colorEMax, Kore::vec2 texOffset);
void setParticleEmitterActive(int id, bool active);
void burstParticleEmitter(int id, int particles);
void moveParticleEmitter(int id, Kore::vec3 emitPos, Kore::vec3 emitDir);
void changeParticleEmission(int id, float spread, float minSpeed, float maxSpeed);
void deleteParticleEmitter(int id);
void updateParticleSystem(float deltaTime);
void renderParticles(Kore::mat4 V, Kore::mat4 P);