#include "MyAppModel.h"

// Creates all the values necessary for drawing a rotating cube
MyAppModel::MyAppModel() {
	// Create VBO
	float verts[] = { 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,      // v0-v1-v2 (front)
			-1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,      // v2-v3-v0

			1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,      // v0-v3-v4 (right)
			1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,      // v4-v5-v0

			1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,      // v0-v5-v6 (top)
			-1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,      // v6-v1-v0

			-1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,      // v1-v6-v7 (left)
			-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,      // v7-v2-v1.0

			-1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f,      // v7-v4-v3 (bottom)
			1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f,      // v3-v2-v7

			1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f,      // v4-v7-v6 (back)
			-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f };    // v6-v5-v4

	// normal array
	float norms[] = { 0, 0, 1, 0, 0, 1, 0, 0, 1,      // v0-v1-v2 (front)
			0, 0, 1, 0, 0, 1, 0, 0, 1,      // v2-v3-v0

			1, 0, 0, 1, 0, 0, 1, 0, 0,      // v0-v3-v4 (right)
			1, 0, 0, 1, 0, 0, 1, 0, 0,      // v4-v5-v0

			0, 1, 0, 0, 1, 0, 0, 1, 0,      // v0-v5-v6 (top)
			0, 1, 0, 0, 1, 0, 0, 1, 0,      // v6-v1-v0

			-1, 0, 0, -1, 0, 0, -1, 0, 0,      // v1-v6-v7 (left)
			-1, 0, 0, -1, 0, 0, -1, 0, 0,      // v7-v2-v1

			0, -1, 0, 0, -1, 0, 0, -1, 0,      // v7-v4-v3 (bottom)
			0, -1, 0, 0, -1, 0, 0, -1, 0,      // v3-v2-v7

			0, 0, -1, 0, 0, -1, 0, 0, -1,      // v4-v7-v6 (back)
			0, 0, -1, 0, 0, -1, 0, 0, -1 };    // v6-v5-v4

	// color array
	float cols[] = { 1, 1, 1, 1, 1, 0, 1, 0, 0,      // v0-v1-v2 (front)
			1, 0, 0, 1, 0, 1, 1, 1, 1,      // v2-v3-v0

			1, 1, 1, 1, 0, 1, 0, 0, 1,      // v0-v3-v4 (right)
			0, 0, 1, 0, 1, 1, 1, 1, 1,      // v4-v5-v0

			1, 1, 1, 0, 1, 1, 0, 1, 0,      // v0-v5-v6 (top)
			0, 1, 0, 1, 1, 0, 1, 1, 1,      // v6-v1-v0

			1, 1, 0, 0, 1, 0, 0, 0, 0,      // v1-v6-v7 (left)
			0, 0, 0, 1, 0, 0, 1, 1, 0,      // v7-v2-v1

			0, 0, 0, 0, 0, 1, 1, 0, 1,      // v7-v4-v3 (bottom)
			1, 0, 1, 1, 0, 0, 0, 0, 0,      // v3-v2-v7

			0, 0, 1, 0, 0, 0, 0, 1, 0,      // v4-v7-v6 (back)
			0, 1, 0, 0, 1, 1, 0, 0, 1 };    // v6-v5-v4

	vertices = std::vector<float>(&verts[0], &verts[0] + sizeof(verts));
	normals = std::vector<float>(&norms[0], &norms[0] + sizeof(norms));
	colors = std::vector<float>(&cols[0], &cols[0] + sizeof(cols));
}

MyAppModel::~MyAppModel() {
}
