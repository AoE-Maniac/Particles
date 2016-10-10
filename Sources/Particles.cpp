#include "pch.h"

#include "Particles.h"

#include <cassert>

#include <Kore/Graphics/Graphics.h>
#include <Kore/Graphics/Shader.h>
#include <Kore/IO/FileReader.h>
#include <Kore/Math/Random.h>

#include "RandomHelpers.h"
#include "RenderHelpers.h"

using namespace Kore;

namespace {
	const int MAX_EMITTERS = 200;
	const int MAX_PARTICLES = 10000;

	Program* program;
	Shader* vertexShader;
	Shader* fragmentShader;

	TextureUnit tex;
	ConstantLocation pLocation;
	ConstantLocation vLocation;

	VertexStructure** structures;
	VertexBuffer** vbs;
	IndexBuffer* ib;
	Texture* texture;

	int currEmitters;
	float* emitterRadius;
	float* emitterSpread;
	float* emitterMaxRot;
	float* emitterSpeedMin;
	float* emitterSpeedMax;
	float* emitterGravMin;
	float* emitterGravMax;
	float* emitterTTLMin;
	float* emitterTTLMax;
	float* emitterTTSMin;
	float* emitterTTSMax;
	float* emitterTTSNext;
	float* emitterSizeMin;
	float* emitterSizeMax;
	vec2* emitterTexOffset;
	vec3* emitterPos;
	vec3* emitterDir;
	vec4* emitterColorSMin;
	vec4* emitterColorSMax;
	vec4* emitterColorEMin;
	vec4* emitterColorEMax;

	int currParticles;
	float* particleGrav;
	float* particleTTLT;
	float* particleTTLR;
	float* particleSize;
	float* particleRotS;
	float* particleRotE;
	vec2* particleTexOffset;
	vec3* particlePos;
	vec3* particleVel;
	vec4* particleColorS;
	vec4* particleColorE;
}

void initParticleSystem() {
	currEmitters = 0;
	emitterRadius = new float[MAX_EMITTERS];
	emitterSpread = new float[MAX_EMITTERS];
	emitterMaxRot = new float[MAX_EMITTERS];
	emitterSpeedMin = new float[MAX_EMITTERS];
	emitterSpeedMax = new float[MAX_EMITTERS];
	emitterGravMin = new float[MAX_EMITTERS];
	emitterGravMax = new float[MAX_EMITTERS];
	emitterTTLMin = new float[MAX_EMITTERS];
	emitterTTLMax = new float[MAX_EMITTERS];
	emitterTTSMin = new float[MAX_EMITTERS];
	emitterTTSMax = new float[MAX_EMITTERS];
	emitterTTSNext = new float[MAX_EMITTERS];
	emitterSizeMin = new float[MAX_EMITTERS];
	emitterSizeMax = new float[MAX_EMITTERS];
	emitterTexOffset = new vec2[MAX_EMITTERS];
	emitterPos = new vec3[MAX_EMITTERS];
	emitterDir = new vec3[MAX_EMITTERS];
	emitterColorSMin = new vec4[MAX_EMITTERS];
	emitterColorSMax = new vec4[MAX_EMITTERS];
	emitterColorEMin = new vec4[MAX_EMITTERS];
	emitterColorEMax = new vec4[MAX_EMITTERS];

	currParticles = 0;
	particleGrav = new float[MAX_PARTICLES];
	particleTTLT = new float[MAX_PARTICLES];
	particleTTLR = new float[MAX_PARTICLES];
	particleSize = new float[MAX_PARTICLES];
	particleRotS = new float[MAX_PARTICLES];
	particleRotE = new float[MAX_PARTICLES];
	particleTexOffset = new vec2[MAX_PARTICLES];
	particlePos = new vec3[MAX_PARTICLES];
	particleVel = new vec3[MAX_PARTICLES];
	particleColorS = new vec4[MAX_PARTICLES];
	particleColorE = new vec4[MAX_PARTICLES];

	FileReader vs("particles.vert");
	vertexShader = new Shader(vs.readAll(), vs.size(), VertexShader);
	FileReader fs("particles.frag");
	fragmentShader = new Shader(fs.readAll(), fs.size(), FragmentShader);

	structures = new VertexStructure*[2];
	structures[0] = new VertexStructure();
	structures[0]->add("pos", Float3VertexData);
	structures[0]->add("tex", Float2VertexData);

	structures[1] = new VertexStructure();
	structures[1]->add("M", Float4x4VertexData);
	structures[1]->add("tint", Float4VertexData);
	structures[1]->add("texOffset", Float2VertexData);

	program = new Program;
	program->setVertexShader(vertexShader);
	program->setFragmentShader(fragmentShader);
	program->link(structures, 2);

	pLocation = program->getConstantLocation("P");
	vLocation = program->getConstantLocation("V");

	tex = program->getTextureUnit("text");
	Graphics::setTextureAddressing(tex, U, Repeat);
	Graphics::setTextureAddressing(tex, V, Repeat);

	vbs = new VertexBuffer*[2];
	vbs[0] = new VertexBuffer(4, *structures[0], 0);
	float* vertices = vbs[0]->lock();
	setVertex(vertices, 0, 0, 5, -1, -1, 0, 0, 0);
	setVertex(vertices, 1, 0, 5, -1, 1, 0, 0, 1);
	setVertex(vertices, 2, 0, 5, 1, 1, 0, 1, 1);
	setVertex(vertices, 3, 0, 5, 1, -1, 0, 1, 0);
	vbs[0]->unlock();

	vbs[1] = new VertexBuffer(MAX_PARTICLES, *structures[1], 1);

	ib = new IndexBuffer(6);
	int* indices = ib->lock();
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;
	ib->unlock();

	texture = new Texture("particle_atlas.png", true);
}

void deleteParticleSystem() {
	delete vbs;
	delete ib;

	delete emitterRadius;
	delete emitterSpread;
	delete emitterMaxRot;
	delete emitterSpeedMin;
	delete emitterSpeedMax;
	delete emitterGravMin;
	delete emitterGravMax;
	delete emitterTTLMin;
	delete emitterTTLMax;
	delete emitterTTSMin;
	delete emitterTTSMax;
	delete emitterTTSNext;
	delete emitterSizeMin;
	delete emitterSizeMax;
	delete emitterTexOffset;
	delete emitterPos;
	delete emitterDir;
	delete emitterColorSMin;
	delete emitterColorSMax;
	delete emitterColorEMin;
	delete emitterColorEMax;

	delete particleGrav;
	delete particleTTLT;
	delete particleTTLR;
	delete particleSize;
	delete particleRotS;
	delete particleRotE;
	delete particleTexOffset;
	delete particlePos;
	delete particleVel;
	delete particleColorS;
	delete particleColorE;
}

void addParticleEmitter(vec3 emitPos, float radius, vec3 emitDir, float spread, float maxRot, float minSpeed, float maxSpeed, float gravMin, float gravMax, float rateMin, float rateMax, float minTtl, float maxTtl, float minSize, float maxSize, vec4 colorSMin, vec4 colorSMax, vec4 colorEMin, vec4 colorEMax, vec2 texOffset) {
	assert(currEmitters < MAX_EMITTERS);

	if (currEmitters < MAX_EMITTERS) {
		emitterRadius[currEmitters] = radius;
		emitterSpread[currEmitters] = spread;
		emitterMaxRot[currEmitters] = maxRot;
		emitterSpeedMin[currEmitters] = minSpeed;
		emitterSpeedMax[currEmitters] = maxSpeed;
		emitterGravMin[currEmitters] = gravMin;
		emitterGravMax[currEmitters] = gravMax;
		emitterTTLMin[currEmitters] = minTtl;
		emitterTTLMax[currEmitters] = maxTtl;
		emitterTTSMin[currEmitters] = rateMin;
		emitterTTSMax[currEmitters] = rateMax;
		emitterTTSNext[currEmitters] = getRandom(rateMin, rateMax);
		emitterSizeMin[currEmitters] = minSize;
		emitterSizeMax[currEmitters] = maxSize;
		emitterTexOffset[currEmitters] = texOffset;
		emitterPos[currEmitters] = emitPos;
		emitterDir[currEmitters] = emitDir;
		emitterColorSMin[currEmitters] = colorSMin;
		emitterColorSMax[currEmitters] = colorSMax;
		emitterColorEMin[currEmitters] = colorEMin;
		emitterColorEMax[currEmitters] = colorEMax;

		++currEmitters;
	}
}

void updateParticleSystem(float deltaTime) {
	for (int i = 0; i < currEmitters; ++i) {
		emitterTTSNext[i] -= deltaTime;

		if (emitterTTSNext[i] <= 0) {
			assert(currParticles < MAX_PARTICLES);

			if (currParticles < MAX_PARTICLES) {
				
				vec3 orthoVector = emitterDir[i].cross(vec3(0, 0, 1));
				orthoVector = rotateAroundAxis(orthoVector, emitterDir[i], getRandom(0, 2 * pi));
				vec3 nextPosition = emitterPos[i] + orthoVector.normalize() * getRandom(0, emitterRadius[i]);

				float nextAngle = getRandom(-emitterSpread[i] / 2, emitterSpread[i] / 2);
				vec3 nextDirection = emitterDir[i]  + orthoVector * Kore::tan(nextAngle);
				nextDirection = rotateAroundAxis(nextDirection, emitterDir[i], getRandom(0, 2 * pi)).normalize();
				
				particleGrav[currParticles] = getRandom(emitterGravMin[i], emitterGravMax[i]);
				particleSize[currParticles] = getRandom(emitterSizeMin[i], emitterSizeMax[i]);
				particleTTLT[currParticles] = getRandom(emitterTTLMin[i], emitterTTLMax[i]);
				particleTTLR[currParticles] = particleTTLT[currParticles];
				particleRotS[currParticles] = getRandom(0, emitterMaxRot[i]);
				particleRotE[currParticles] = getRandom(0, emitterMaxRot[i]);
				particleTexOffset[currParticles] = emitterTexOffset[i];
				particlePos[currParticles] = nextPosition;
				particleVel[currParticles] = nextDirection * getRandom(emitterSpeedMin[i], emitterSpeedMax[i]);
				particleColorS[currParticles] = getRandom(emitterColorSMin[i], emitterColorSMax[i]);
				particleColorE[currParticles] = getRandom(emitterColorEMin[i], emitterColorEMax[i]);

				++currParticles;
			}

			emitterTTSNext[i] = getRandom(emitterTTSMin[i], emitterTTSMax[i]);
		}
	}

	for (int i = 0; i < currParticles; ++i) {
		if (particleTTLR[i] <= 0.0f) {
			--currParticles;

			particleGrav[i] = particleGrav[currParticles];
			particleSize[i] = particleSize[currParticles];
			particleTTLT[i] = particleTTLT[currParticles];
			particleTTLR[i] = particleTTLR[currParticles];
			particleRotS[i] = particleRotS[currParticles];
			particleRotE[i] = particleRotE[currParticles];
			particleTexOffset[i] = particleTexOffset[currParticles];
			particlePos[i] = particlePos[currParticles];
			particleVel[i] = particleVel[currParticles];
			particleColorS[i] = particleColorS[currParticles];
			particleColorE[i] = particleColorE[currParticles];

			--i;
		}
		else {
			particleTTLR[i] -= deltaTime;
			particleVel[i] += vec3(0, particleGrav[i] * deltaTime, 0);
			particlePos[i] += particleVel[i] * deltaTime;
		}
	}
}

void renderParticles(mat4 V, mat4 P) {
	program->set();
	Graphics::setBlendingMode(SourceAlpha, InverseSourceAlpha);
	Graphics::setRenderState(BlendingState, true);
	Graphics::setRenderState(DepthTest, true);
	Graphics::setRenderState(DepthTestCompare, ZCompareLess);
	Graphics::setRenderState(DepthWrite, false);

	Graphics::setMatrix(pLocation, P);
	Graphics::setMatrix(vLocation, V);

	mat4 view = V.Invert();
	view.Set(0, 3, 0.0f);
	view.Set(1, 3, 0.0f);
	view.Set(2, 3, 0.0f);

	int alive = 0;
	float* data = vbs[1]->lock();
	for (int i = 0; i < currParticles; ++i) {
		float a = particleTTLR[i] / particleTTLT[i];

		mat4 M = mat4::Translation(particlePos[i].x(), particlePos[i].y(), particlePos[i].z()) * mat4::Scale(particleSize[i], particleSize[i], particleSize[i]);
		setMatrix(data, alive, 0, 22, M * view * mat4::RotationZ(particleRotS[i] * a + particleRotE[i] * (1 - a)));
		
		setVec4(data, alive, 16, 22, particleColorS[i] * a + particleColorE[i] * (1 - a));

		setVec2(data, alive, 20, 22, particleTexOffset[i]);

		++alive;
	}
	vbs[1]->unlock();

	Graphics::setTexture(tex, texture);
	Graphics::setVertexBuffers(vbs, 2);
	Graphics::setIndexBuffer(*ib);
	Graphics::drawIndexedVerticesInstanced(alive);
}