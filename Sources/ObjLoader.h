/*
.obj importer written by Florian Mehm and Robert Konrad
*/

#pragma once

struct Mesh {
	int numFaces;
	int numVertices;
	int numUVs;
	int numNormals;

	float* vertices;
	int* indices;
	float* uvs;
	float * normals;

	// very private
	float* curVertex;
	int* curIndex;
	float* curUV;
	float* curNormal;
};

Mesh* loadObj(const char* filename);
