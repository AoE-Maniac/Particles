#include <Kore/pch.h>

#include "RandomHelpers.h"

using namespace Kore;

float getRandom(float minValue, float maxValue) {
	float a = (float)Kore::Random::get(0, 1000) / 1000;
	return a * minValue + (1 - a) * maxValue;
}

vec4 getRandom(vec4 minValue, vec4 maxValue) {
	float a = (float)Kore::Random::get(0, 1000) / 1000;
	return a * minValue * a + (1 - a) * maxValue;
}