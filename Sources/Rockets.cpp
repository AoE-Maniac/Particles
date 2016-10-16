#include "pch.h"

#include "Rockets.h"

#include <cassert>

#include <Kore/Graphics/Graphics.h>
#include <Kore/Graphics/Shader.h>
#include <Kore/IO/FileReader.h>
#include <Kore/Math/Random.h>

#include "ObjLoader.h"
#include "Particles.h"
#include "RandomHelpers.h"
#include "RenderHelpers.h"

using namespace Kore;

namespace {

	struct Rocket {
		int particleID;
		int phase;
		float height;
		float currRot;
		float yAngle;
		float timer;
		vec3 startPos;
		vec3 currPos;
		vec3 targetPos;
	};

	const int MAX_ROCKETS = 100;
	const float MIN_HEIGHT = 10; // 7.5f;
	const float MAX_HEIGHT = 10;
	const float SCALING = 0.33f;

	Program* program;
	Shader* vertexShader;
	Shader* fragmentShader;

	TextureUnit tex;

	VertexStructure** structures;
	VertexBuffer** vbs;
	IndexBuffer* ib;
	Texture* texture;
	Mesh* mesh;

	int currRockets;
	Rocket* rockets;
}

void initRockets() {
	rockets = new Rocket[MAX_ROCKETS];

	FileReader vs("meshes.vert");
	vertexShader = new Shader(vs.readAll(), vs.size(), VertexShader);
	FileReader fs("meshes.frag");
	fragmentShader = new Shader(fs.readAll(), fs.size(), FragmentShader);

	structures = new VertexStructure*[2];
	structures[0] = new VertexStructure();
	structures[0]->add("pos", Float3VertexData);
	structures[0]->add("tex", Float2VertexData);
	structures[0]->add("nor", Float3VertexData);

	structures[1] = new VertexStructure();
	structures[1]->add("MVP", Float4x4VertexData);
	structures[1]->add("MIT", Float4x4VertexData);

	program = new Program;
	program->setVertexShader(vertexShader);
	program->setFragmentShader(fragmentShader);
	program->link(structures, 2);

	tex = program->getTextureUnit("text");
	Graphics::setTextureAddressing(tex, U, Repeat);
	Graphics::setTextureAddressing(tex, V, Repeat);
	
	mesh = loadObj("rocket.obj");

	vbs = new VertexBuffer*[2];
	vbs[0] = new VertexBuffer(mesh->numVertices, *structures[0], 0);
	float* vertices = vbs[0]->lock();
	for (int i = 0; i < mesh->numVertices; ++i) {
		vertices[i * 8 + 0] = mesh->vertices[i * 8 + 0];
		vertices[i * 8 + 1] = mesh->vertices[i * 8 + 1];
		vertices[i * 8 + 2] = mesh->vertices[i * 8 + 2];
		vertices[i * 8 + 3] = mesh->vertices[i * 8 + 3];
		vertices[i * 8 + 4] = 1.0f - mesh->vertices[i * 8 + 4];
		vertices[i * 8 + 5] = mesh->vertices[i * 8 + 5];
		vertices[i * 8 + 6] = mesh->vertices[i * 8 + 6];
		vertices[i * 8 + 7] = mesh->vertices[i * 8 + 7];
	}
	vbs[0]->unlock();

	vbs[1] = new VertexBuffer(MAX_ROCKETS, *structures[1], 1);

	ib = new IndexBuffer(mesh->numFaces * 3);
	int* indices = ib->lock();
	for (int i = 0; i < mesh->numFaces * 3; i++) {
		indices[i] = mesh->indices[i];
	}
	ib->unlock();

	texture = new Texture("rocket.png", true);
}

void deleteRockets() {
	delete[] vbs[0];
	delete[] vbs[1];
	delete[] vbs;
	delete[] ib;

	delete[] rockets;
}

void fireRocketRaw(u8* params) {
	fireRocket(vec3(*(float*)&params[0], *(float*)&params[4], *(float*)&params[8]),
		vec3(*(float*)&params[12], *(float*)&params[16], *(float*)&params[20]));
}

void fireRocket(vec3 from, vec3 to) {
	assert(currRockets < MAX_ROCKETS);

	if (currRockets < MAX_ROCKETS) {
		rockets[currRockets].phase = 0;
		rockets[currRockets].height = getRandom(MIN_HEIGHT, MAX_HEIGHT);
		rockets[currRockets].currRot = 1.0f;
		rockets[currRockets].timer = 0;
		rockets[currRockets].startPos = from;
		rockets[currRockets].currPos = from - vec3(0, 7.0f * SCALING, 0);
		rockets[currRockets].targetPos = to;

		vec3 a = vec3(1, 0, 0);
		vec3 b = (to - from).normalize();
		float angle = Kore::acos(a.dot(b));
		if ((to - from).z() > 0) angle = -angle;
		rockets[currRockets].yAngle = angle;

		rockets[currRockets].particleID = addParticleEmitter(from, 0.25f, vec3(0, 0, 0), 0.5f * pi, pi, 0.1f, 0.15f, -0.05f, -0.1f, 0.005f, 0.01f, 2.0f, 2.5f, 0.9f * SCALING, SCALING, vec4(0.5f, 0.5f, 0.5f, 0.5f), vec4(0.5f, 0.5f, 0.5f, 1), vec4(0.5f, 0.5f, 0.5f, 0), vec4(0.5f, 0.5f, 0.5f, 0), vec2(1, 0));
		pauseParticleEmitter(rockets[currRockets].particleID, true);

		++currRockets;
	}
}

void updateRockets(float deltaT) {
	for (int i = 0; i < currRockets; ++i) {
		switch (rockets[i].phase) {
		case 0:
			rockets[i].currPos += vec3(0, 14.0f * SCALING * deltaT * 0.25f, 0);
			if (rockets[i].currPos.y() >= 9.0f * SCALING) {
				rockets[i].phase++;
				pauseParticleEmitter(rockets[i].particleID, false);

				vec3 down = vec3(0, -1, 0);
				moveParticleEmitter(rockets[i].particleID, rockets[i].currPos + down * 7.0f * SCALING, down);
				changeParticleEmission(rockets[i].particleID, pi, 1.0f, 2.0f);
			}
			break;
		case 1:
			rockets[i].timer += deltaT;
			if (rockets[i].timer > 1.5f) {
				rockets[i].phase++;
				changeParticleEmission(rockets[i].particleID, 0.5f * pi, 0.1f, 0.15f);
			}
			break;
		case 2: {
			// Based on quadratic formula with two points given
			// To improve performance, one could save d, x and toTarget separately
			vec3 projPos = vec3(rockets[i].currPos.x(), 0, rockets[i].currPos.z());
			vec3 toTarget = rockets[i].targetPos - rockets[i].startPos;
			float d = Kore::abs(toTarget.getLength());
			float x = Kore::abs((projPos - rockets[i].startPos).getLength()) + deltaT;
			float y = 4 * rockets[i].height * x * (1 - x / d) / d;

			vec3 nextPos = toTarget.normalize() * x;
			rockets[i].currPos = vec3(nextPos.x(), y + 9.0f * SCALING, nextPos.z());
			rockets[i].currRot = (1 - 2 * x / d);

			// TODO: Cleanup, duplicate code with rendering
			vec4 dir = (mat4::RotationY(rockets[i].yAngle) * mat4::RotationZ(-0.5f * pi + rockets[i].currRot * 0.5f * pi) * vec4(0, 1, 0));
			dir = dir / dir.w();
			vec3 dir3 = vec3(dir.x(), dir.y(), dir.z()).normalize();
			moveParticleEmitter(rockets[i].particleID, rockets[i].currPos + dir3 * 7.0f * SCALING, dir3);
			
			if ((rockets[i].targetPos - projPos).getLength() <= 0.2f && rockets[i].currPos.y() <= 8.0f * SCALING) {
				rockets[i].timer = 0.0f;
				moveParticleEmitter(rockets[i].particleID, rockets[i].currPos, vec3(0, 1, 0));
				changeParticleEmission(rockets[i].particleID, 2 * pi, 3.0f, 4.0f);
				burstParticleEmitter(rockets[i].particleID, 500);
				rockets[i].currPos = vec3(rockets[i].currPos.x(), -100, rockets[i].currPos.z());

				rockets[i].phase++;
			}
			break;
		}
		case 3:
			rockets[i].timer += deltaT;
			if (rockets[i].timer > 1.5f) {
				--currRockets;

				deleteParticleEmitter(rockets[i].particleID);
				rockets[i] = rockets[currRockets];

				--i;
			}
			break;
		}
	}
}

void renderRockets(mat4 V, mat4 P) {
	program->set();
	Graphics::setBlendingMode(SourceAlpha, InverseSourceAlpha);
	Graphics::setRenderState(BlendingState, true);
	Graphics::setRenderState(DepthTest, true);
	Graphics::setRenderState(DepthTestCompare, ZCompareLess);
	Graphics::setRenderState(DepthWrite, true);

	mat4 PV = P * V;
	float* data = vbs[1]->lock();
	for (int i = 0; i < currRockets; ++i) {
		mat4 M = mat4::Translation(rockets[i].currPos.x(), rockets[i].currPos.y(), rockets[i].currPos.z())
			* mat4::Scale(SCALING, SCALING, SCALING)
			* mat4::RotationY(rockets[i].yAngle)
			* mat4::RotationZ(-0.5f * pi + rockets[i].currRot * 0.5f * pi);
		
		setMatrix(data, i, 0, 32, PV * M);
		setMatrix(data, i, 16, 32, M.Invert().Transpose());
	}
	vbs[1]->unlock();

	Graphics::setTexture(tex, texture);
	Graphics::setVertexBuffers(vbs, 2);
	Graphics::setIndexBuffer(*ib);
	Graphics::drawIndexedVerticesInstanced(currRockets);
}