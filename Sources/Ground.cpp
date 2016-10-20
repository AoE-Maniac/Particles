#include "pch.h"

#include "Ground.h"

#include <cassert>

#include <Kore/Graphics/Graphics.h>
#include <Kore/Graphics/Shader.h>
#include <Kore/IO/FileReader.h>

#include "RenderHelpers.h"
#include "RandomHelpers.h"

using namespace Kore;

namespace {
	const int GROUND_SIZE = 100;

	Program* program;
	Shader* vertexShader;
	Shader* fragmentShader;

	TextureUnit tex;

	VertexStructure** structures;
	VertexBuffer** vbs;
	IndexBuffer* ib;
	Texture* texture;
}

void initGround() {
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

	vbs = new VertexBuffer*[2];
	vbs[0] = new VertexBuffer(4, *structures[0], 0);
	float* vertices = vbs[0]->lock();
	setVertex(vertices, 0, 0, 8, -1, 0, -1, 0, 0, 0, 0, -1);
	setVertex(vertices, 1, 0, 8, -1, 0, 1, 0, 1, 0, 0, -1);
	setVertex(vertices, 2, 0, 8, 1, 0, 1, 1, 1, 0, 0, -1);
	setVertex(vertices, 3, 0, 8, 1, 0, -1, 1, 0, 0, 0, -1);
	vbs[0]->unlock();

	vbs[1] = new VertexBuffer(GROUND_SIZE * GROUND_SIZE, *structures[1], 1);

	ib = new IndexBuffer(6);
	int* indices = ib->lock();
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;
	ib->unlock();

	texture = new Texture("ground.png", true);
}

void deleteGround() {
	delete[] vbs[0];
	delete[] vbs[1];
	delete[] vbs;
	delete[] ib;
}

vec3 getRandomGroundPosition() {
	return getRandomGroundPosition(getRandom(0, 2 * pi));
	//return vec3(getRandom(-GROUND_SIZE / 2.0f, GROUND_SIZE / 2.0f), 0, getRandom(-GROUND_SIZE / 2.0f, GROUND_SIZE / 2.0f));
}

vec3 getRandomGroundPosition(float angle) {
	return rotateAroundAxis(vec3(1, 0, 0), vec3(0, 1, 0), angle) * (GROUND_SIZE / 4.0f);
}

void renderGround(mat4 V, mat4 P) {
	program->set();
	Graphics::setBlendingMode(SourceAlpha, InverseSourceAlpha);
	Graphics::setRenderState(BlendingState, true);
	Graphics::setRenderState(DepthTest, true);
	Graphics::setRenderState(DepthTestCompare, ZCompareLess);
	Graphics::setRenderState(DepthWrite, true);

	mat4 PV = P * V;
	float* data = vbs[1]->lock();
	for (int x = 0; x < GROUND_SIZE; ++x) {
		for (int z = 0; z < GROUND_SIZE; ++z) {
			mat4 M = mat4::Translation(x - GROUND_SIZE / 2.0f, 0, z - GROUND_SIZE / 2.0f);
			
			//mat4 MIT = M.Invert().Transpose();
			mat4 MIT = mat4::Identity();
			MIT.Set(3, 0, - (x - GROUND_SIZE / 2.0f));
			MIT.Set(3, 2, - (z - GROUND_SIZE / 2.0f));
			
			int i = x * GROUND_SIZE + z;
			setMatrix(data, i, 0, 32, PV * M);
			setMatrix(data, i, 16, 32, MIT);
		}
	}
	vbs[1]->unlock();

	Graphics::setTexture(tex, texture);
	Graphics::setVertexBuffers(vbs, 2);
	Graphics::setIndexBuffer(*ib);
	Graphics::drawIndexedVerticesInstanced(GROUND_SIZE * GROUND_SIZE);
}