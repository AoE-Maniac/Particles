#include "pch.h"

#include "Rockets.h"

#include <cassert>

#include <Kore/Graphics/Graphics.h>
#include <Kore/Graphics/Shader.h>
#include <Kore/IO/FileReader.h>
#include <Kore/Math/Random.h>

#include "ObjLoader.h"
#include "RandomHelpers.h"
#include "RenderHelpers.h"

using namespace Kore;

namespace {
	const int MAX_ROCKETS = 100;
	const float MIN_HEIGHT = 7.5f;
	const float MAX_HEIGHT = 10;
	const float SCALING = 0.33f;

	Program* program;
	Shader* vertexShader;
	Shader* fragmentShader;

	TextureUnit tex;

	VertexStructure** structures;
	VertexBuffer** vbs;
	IndexBuffer* ib;
	Mesh* mesh;

	int currRockets;
	float* heights;
	float* currRot;
	float* yAngle;
	vec3* startPos;
	vec3* currPos;
	vec3* targetPos;
}

void initRockets() {
	heights = new float[MAX_ROCKETS];
	currRot = new float[MAX_ROCKETS];
	yAngle = new float[MAX_ROCKETS];
	startPos = new vec3[MAX_ROCKETS];
	currPos = new vec3[MAX_ROCKETS];
	targetPos = new vec3[MAX_ROCKETS];

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

	program = new Program;
	program->setVertexShader(vertexShader);
	program->setFragmentShader(fragmentShader);
	program->link(structures, 2);
	
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
}

void deleteRockets() {
	delete vbs;
	delete ib;

	delete heights;
	delete currRot;
	delete yAngle;
	delete startPos;
	delete currPos;
	delete targetPos;
}

void fireRocket(vec3 from, vec3 to) {
	assert(currRockets < MAX_ROCKETS);

	if (currRockets < MAX_ROCKETS) {
		heights[currRockets] = getRandom(MIN_HEIGHT, MAX_HEIGHT);
		currRot[currRockets] = 1.0f;
		startPos[currRockets] = from;
		currPos[currRockets] = from;
		targetPos[currRockets] = to;

		vec3 a = vec3(1, 0, 0);
		vec3 b = (to - from).normalize();
		float angle = Kore::acos(a.dot(b));
		if ((to - from).z() > 0) angle = -angle;
		yAngle[currRockets] = angle;

		++currRockets;
	}
}

void updateRockets(float deltaT) {
	for (int i = 0; i < currRockets; ++i) {
		if ((targetPos[i] - currPos[i]).getLength() <= 0.2f) {
			--currRockets;

			heights[i] = heights[currRockets];
			currRot[i] = currRot[currRockets];
			yAngle[i] = yAngle[currRockets];
			startPos[i] = startPos[currRockets];
			currPos[i] = currPos[currRockets];
			targetPos[i] = targetPos[currRockets];

			--i;
		}
		else {
			// Based on quadratic formula with two points given
			// To improve perforance, one could save d, x and toTarget separately
			currPos[i] = vec3(currPos[i].x(), 0, currPos[i].z());
			vec3 toTarget = targetPos[i] - startPos[i];
			float d = Kore::abs(toTarget.getLength());
			float x = Kore::abs((currPos[i] - startPos[i]).getLength()) + deltaT;
			float y = 4 * heights[i] * x * (1 - x / d) / d;

			vec3 nextPos = toTarget.normalize() * x;
			currPos[i] = vec3(nextPos.x(), y, nextPos.z());
			currRot[i] = (1 - 2 * x / d);
		}
	}
}

void renderRockets(mat4 V, mat4 P) {
	program->set();
	Graphics::setBlendingMode(SourceAlpha, InverseSourceAlpha);
	Graphics::setRenderState(BlendingState, true);
	Graphics::setRenderState(DepthTest, true);
	Graphics::setRenderState(DepthTestCompare, ZCompareLess);
	Graphics::setRenderState(DepthWrite, false);

	float* data = vbs[1]->lock();
	mat4 PV = P * V;
	for (int i = 0; i < currRockets; ++i) {
		mat4 M = mat4::Translation(currPos[i].x(), currPos[i].y(), currPos[i].z()) * mat4::Scale(SCALING, SCALING, SCALING);
		
		setMatrix(data, i, 0, 16, PV * M * mat4::RotationY(yAngle[i]) * mat4::RotationZ(-0.5f * pi + currRot[i] * 0.5f * pi));
	}
	vbs[1]->unlock();
	
	Graphics::setVertexBuffers(vbs, 2);
	Graphics::setIndexBuffer(*ib);
	Graphics::drawIndexedVerticesInstanced(currRockets);
}