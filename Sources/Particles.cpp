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
	struct Emitter {
		bool Enabled;
		bool Paused;
		float Radius;
		float Spread;
		float MaxRot;
		float SpeedMin;
		float SpeedMax;
		float GravMin;
		float GravMax;
		float TTLMin;
		float TTLMax;
		float TTSMin;
		float TTSMax;
		float TTSNext;
		float SizeMin;
		float SizeMax;
		vec2 TexOffset;
		vec3 Pos;
		vec3 Dir;
		vec4 ColorSMin;
		vec4 ColorSMax;
		vec4 ColorEMin;
		vec4 ColorEMax;
	};

	struct Particle {
		float Grav;
		float TTLT;
		float TTLR;
		float Size;
		float RotS;
		float RotE;
		vec2 TexOffset;
		vec3 Pos;
		vec3 Vel;
		vec4 ColorS;
		vec4 ColorE;
	};

	const int MAX_EMITTERS = 360;
	const int MAX_PARTICLES = MAX_EMITTERS * 500;

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

	int actualMaxEmitters;
	Emitter* emitters;

	int currParticles;
	Particle* particles;

	void emitParticle(int id) {
		assert(currParticles < MAX_PARTICLES);

		if (currParticles < MAX_PARTICLES) {
			vec3 orthoVector = emitters[id].Dir.cross(vec3(0, 0, 1));
			orthoVector = rotateAroundAxis(orthoVector, emitters[id].Dir, getRandom(0, 2 * pi));
			vec3 nextPosition = emitters[id].Pos + orthoVector.normalize() * getRandom(0, emitters[id].Radius);

			float nextAngle = getRandom(-emitters[id].Spread / 2, emitters[id].Spread / 2);
			vec3 nextDirection = emitters[id].Dir + orthoVector * Kore::tan(nextAngle);
			if (Kore::abs(nextAngle) > 0.5f * pi) nextDirection = -nextDirection;
			nextDirection = rotateAroundAxis(nextDirection, emitters[id].Dir, getRandom(0, 2 * pi)).normalize();

			particles[currParticles].Grav = getRandom(emitters[id].GravMin, emitters[id].GravMax);
			particles[currParticles].Size = getRandom(emitters[id].SizeMin, emitters[id].SizeMax);
			particles[currParticles].TTLT = getRandom(emitters[id].TTLMin, emitters[id].TTLMax);
			particles[currParticles].TTLR = particles[currParticles].TTLT;
			particles[currParticles].RotS = getRandom(0, emitters[id].MaxRot);
			particles[currParticles].RotE = getRandom(0, emitters[id].MaxRot);
			particles[currParticles].TexOffset = emitters[id].TexOffset;
			particles[currParticles].Pos = nextPosition;
			particles[currParticles].Vel = nextDirection * getRandom(emitters[id].SpeedMin, emitters[id].SpeedMax);
			particles[currParticles].ColorS = getRandom(emitters[id].ColorSMin, emitters[id].ColorSMax);
			particles[currParticles].ColorE = getRandom(emitters[id].ColorEMin, emitters[id].ColorEMax);

			++currParticles;
		}
	}
}

void initParticleSystem() {
	actualMaxEmitters = 0;
	emitters = new Emitter[MAX_PARTICLES];

	currParticles = 0;
	particles = new Particle[MAX_PARTICLES];

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
	delete[] vbs[0];
	delete[] vbs[1];
	delete[] vbs;
	delete[] ib;

	delete[] emitters;
	delete[] particles;
}

int addParticleEmitter(vec3 emitPos, float radius, vec3 emitDir, float spread, float maxRot, float minSpeed, float maxSpeed, float gravMin, float gravMax, float rateMin, float rateMax, float minTtl, float maxTtl, float minSize, float maxSize, vec4 colorSMin, vec4 colorSMax, vec4 colorEMin, vec4 colorEMax, vec2 texOffset) {
	assert(actualMaxEmitters < MAX_EMITTERS);

	for (int i = 0; i < MAX_EMITTERS; ++i) {
		if (!emitters[i].Enabled || i > actualMaxEmitters) {
			emitters[i].Enabled = true;
			emitters[i].Paused = false;
			emitters[i].Radius = radius;
			emitters[i].Spread = spread;
			emitters[i].MaxRot = maxRot;
			emitters[i].SpeedMin = minSpeed;
			emitters[i].SpeedMax = maxSpeed;
			emitters[i].GravMin = gravMin;
			emitters[i].GravMax = gravMax;
			emitters[i].TTLMin = minTtl;
			emitters[i].TTLMax = maxTtl;
			emitters[i].TTSMin = rateMin;
			emitters[i].TTSMax = rateMax;
			emitters[i].TTSNext = getRandom(rateMin, rateMax);
			emitters[i].SizeMin = minSize;
			emitters[i].SizeMax = maxSize;
			emitters[i].TexOffset = texOffset;
			emitters[i].Pos = emitPos;
			emitters[i].Dir = emitDir;
			emitters[i].ColorSMin = colorSMin;
			emitters[i].ColorSMax = colorSMax;
			emitters[i].ColorEMin = colorEMin;
			emitters[i].ColorEMax = colorEMax;

			actualMaxEmitters = Kore::max(actualMaxEmitters, i + 1);

			return i;
		}
	}
	return -1;
}

void burstParticleEmitter(int id, int particles) {
	for (int i = 0; i < particles; ++i) {
		emitParticle(id);
	}
	emitters[id].Paused = true;
}

void pauseParticleEmitter(int id, bool pause) {
	emitters[id].Paused = pause;
}

void moveParticleEmitter(int id, vec3 emitPos, vec3 emitDir) {
	emitters[id].Pos = emitPos;
	emitters[id].Dir = emitDir;
}

void changeParticleEmission(int id, float spread, float minSpeed, float maxSpeed) {
	emitters[id].Spread = spread;
	emitters[id].SpeedMin = minSpeed;
	emitters[id].SpeedMax = maxSpeed;
}

void changeParticleTimes(int id, float rateMin, float rateMax, float minTtl, float maxTtl) {
	emitters[id].TTLMin = minTtl;
	emitters[id].TTLMax = maxTtl;
	emitters[id].TTSMin = rateMin;
	emitters[id].TTSMax = rateMax;
	emitters[id].TTSNext = getRandom(rateMin, rateMax);
}

void deleteParticleEmitter(int id) {
	emitters[id].Enabled = false;
}

void updateParticleSystem(float deltaTime) {
	for (int i = 0; i < actualMaxEmitters; ++i) {
		if (emitters[i].Enabled && !emitters[i].Paused) {
			emitters[i].TTSNext -= deltaTime;

			while (emitters[i].TTSNext <= 0) {
				emitParticle(i);

				emitters[i].TTSNext += getRandom(emitters[i].TTSMin, emitters[i].TTSMax);
			}
		}
	}

	for (int i = 0; i < currParticles; ++i) {
		if (particles[i].TTLR <= 0.0f) {
			--currParticles;

			particles[i] = particles[currParticles];

			--i;
		}
		else {
			particles[i].TTLR -= deltaTime;
			particles[i].Vel += vec3(0, particles[i].Grav * deltaTime, 0);
			particles[i].Pos += particles[i].Vel * deltaTime;
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
		float a = particles[i].TTLR / particles[i].TTLT;

		mat4 M = mat4::Translation(particles[i].Pos.x(), particles[i].Pos.y(), particles[i].Pos.z()) * mat4::Scale(particles[i].Size, particles[i].Size, particles[i].Size);
		setMatrix(data, alive, 0, 22, M * view * mat4::RotationZ(particles[i].RotS * a + particles[i].RotE * (1 - a)));
		
		setVec4(data, alive, 16, 22, particles[i].ColorS * a + particles[i].ColorE * (1 - a));

		setVec2(data, alive, 20, 22, particles[i].TexOffset);

		++alive;
	}
	vbs[1]->unlock();

	Graphics::setTexture(tex, texture);
	Graphics::setVertexBuffers(vbs, 2);
	Graphics::setIndexBuffer(*ib);
	Graphics::drawIndexedVerticesInstanced(alive);
}