/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "MyAppSharedContext.h"

MyAppSharedContext::MyAppSharedContext(const MyAppModel& model, const VRGraphicsState &renderState) : model(model) {
	// Create VBO
	GLfloat vertices[] = { 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,      // v0-v1-v2 (front)
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
	GLfloat normals[] = { 0, 0, 1, 0, 0, 1, 0, 0, 1,      // v0-v1-v2 (front)
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
	GLfloat colors[] = { 1, 1, 1, 1, 1, 0, 1, 0, 0,      // v0-v1-v2 (front)
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

	// Allocate space and send Vertex Data
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.vertices.size() + sizeof(GLfloat)*model.normals.size() + sizeof(GLfloat)*model.colors.size(), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*model.vertices.size(), &vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.vertices.size(), sizeof(GLfloat)*model.normals.size(), &normals[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.vertices.size() + sizeof(GLfloat)*model.normals.size(), sizeof(GLfloat)*model.colors.size(), &colors[0]);
}

MyAppSharedContext::~MyAppSharedContext() {
	glDeleteBuffers(1, &vbo);
}

void MyAppSharedContext::update(const VRGraphicsState &renderState) {
}

