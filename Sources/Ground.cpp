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
	const int GROUND_SIZE = 400;
	const float INNER_GROUND_SIZE = GROUND_SIZE / 16.0f;

	Program* program;
	Shader* vertexShader;
	Shader* fragmentShader;

	TextureUnit tex;

	VertexStructure* structures[2];
	VertexBuffer* vbs[2];
	IndexBuffer* ib;
	Texture* texture;
}

void initGround() {
	FileReader vs("meshes.vert");
	vertexShader = new Shader(vs.readAll(), vs.size(), VertexShader);
	FileReader fs("meshes.frag");
	fragmentShader = new Shader(fs.readAll(), fs.size(), FragmentShader);

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
	delete program;
	delete vertexShader;
	delete fragmentShader;

	delete[] vbs[0];
	delete[] vbs[1];
	delete[] ib;

	delete texture;
}

vec3 getRandomPosition() {
	return vec3(getRandom(-INNER_GROUND_SIZE, INNER_GROUND_SIZE), 0, getRandom(-INNER_GROUND_SIZE, INNER_GROUND_SIZE));
}

vec3 getRandomSidePosition(bool left) {
	return vec3((left ? -INNER_GROUND_SIZE : INNER_GROUND_SIZE), 0, getRandom(-INNER_GROUND_SIZE, INNER_GROUND_SIZE));
}

vec3 getRandomCirclePosition() {
	return getCirclePosition(getRandom(0, 2 * pi));
}

vec3 getCirclePosition(float angle) {
	return rotateAroundAxis(vec3(1, 0, 0), vec3(0, 1, 0), angle) * INNER_GROUND_SIZE;
}

void renderGround(mat4 V, mat4 P) {
	program->set();
	Graphics::setBlendingMode(SourceAlpha, InverseSourceAlpha);
	Graphics::setRenderState(BlendingState, true);
	Graphics::setRenderState(DepthTest, true);
	Graphics::setRenderState(DepthTestCompare, ZCompareLess);
	Graphics::setRenderState(DepthWrite, true);

	float* data = vbs[1]->lock();
	
	mat4 M = mat4::Scale(GROUND_SIZE, 1, GROUND_SIZE);
	setMatrix(data, 0, 0, 32, P * V * M);

	// Optimization: Since this is just a scaling op, instead of mat4 MIT = M.Invert().Transpose();
	mat4 MIT = mat4::Scale(1.0f / GROUND_SIZE, 1, 1.0f / GROUND_SIZE);
	setMatrix(data, 0, 16, 32, MIT);
	
	vbs[1]->unlock();

	Graphics::setTexture(tex, texture);
	Graphics::setVertexBuffers(vbs, 2);
	Graphics::setIndexBuffer(*ib);
	Graphics::drawIndexedVerticesInstanced(1);
}