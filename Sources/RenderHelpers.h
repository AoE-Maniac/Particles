#pragma once

#include "Kore/Math/Matrix.h"
#include "Kore/Math/Vector.h"

Kore::vec3 rotateAroundAxis(Kore::vec3 vector, Kore::vec3 normalizedAxis, float angle);

void setVertex(float* data, int instanceIndex, int off, int size, float x, float y, float z, float u, float v);
void setVertex(float* data, int instanceIndex, int off, int size, float x, float y, float z, float u, float v, float nx, float ny, float nz);
void setVec2(float* data, int instanceIndex, int off, int size, Kore::vec2 v);
void setVec4(float* data, int instanceIndex, int off, int size, Kore::vec4 v);
void setMatrix(float* data, int instanceIndex, int off, int size, Kore::mat4 m);