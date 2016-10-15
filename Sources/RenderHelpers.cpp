#include <Kore/pch.h>

#include "RenderHelpers.h"

using namespace Kore;

vec3 rotateAroundAxis(vec3 vector, vec3 normalizedAxis, float angle) {
	return vector * Kore::cos(angle) + normalizedAxis.cross(vector) * Kore::sin(angle) + normalizedAxis * normalizedAxis.dot(vector) * (1 - Kore::cos(angle));
}

void setVertex(float* data, int instanceIndex, int off, int size, float x, float y, float z, float u, float v) {
	int offset = off + instanceIndex * size;
	data[offset + 0] = x;
	data[offset + 1] = y;
	data[offset + 2] = z;
	data[offset + 3] = u;
	data[offset + 4] = v;
}

void setVertex(float* data, int instanceIndex, int off, int size, float x, float y, float z, float u, float v, float nx, float ny, float nz) {
	int offset = off + instanceIndex * size;
	data[offset + 0] = x;
	data[offset + 1] = y;
	data[offset + 2] = z;
	data[offset + 3] = u;
	data[offset + 4] = v;
	data[offset + 5] = nx;
	data[offset + 6] = ny;
	data[offset + 7] = nz;
}

void setVec2(float* data, int instanceIndex, int off, int size, vec2 v) {
	int offset = off + instanceIndex * size;
	data[offset + 0] = v.x();
	data[offset + 1] = v.y();
}

void setVec4(float* data, int instanceIndex, int off, int size, vec4 v) {
	int offset = off + instanceIndex * size;
	data[offset + 0] = v.x();
	data[offset + 1] = v.y();
	data[offset + 2] = v.z();
	data[offset + 3] = v.w();
}

void setMatrix(float* data, int instanceIndex, int off, int size, mat4 m) {
	int offset = off + instanceIndex * size;
	data[offset + 0] = m[0][0];
	data[offset + 1] = m[1][0];
	data[offset + 2] = m[2][0];
	data[offset + 3] = m[3][0];
	data[offset + 4] = m[0][1];
	data[offset + 5] = m[1][1];
	data[offset + 6] = m[2][1];
	data[offset + 7] = m[3][1];
	data[offset + 8] = m[0][2];
	data[offset + 9] = m[1][2];
	data[offset + 10] = m[2][2];
	data[offset + 11] = m[3][2];
	data[offset + 12] = m[0][3];
	data[offset + 13] = m[1][3];
	data[offset + 14] = m[2][3];
	data[offset + 15] = m[3][3];
}