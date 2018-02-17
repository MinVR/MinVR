/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include "MyAppSharedContext.h"

MyAppSharedContext::MyAppSharedContext(const MyAppModel& model, const VRGraphicsState &renderState) : model(model), version(0) {
	// Initialize Graphics Context and variables
    #ifndef __APPLE__
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
            std::cout << "Error initializing GLEW." << std::endl;
        }
    #endif
    
	// Allocate space and send Vertex Data
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.vertices.size() + sizeof(GLfloat)*model.normals.size() + sizeof(GLfloat)*model.colors.size(), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*model.vertices.size(), &model.vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.vertices.size(), sizeof(GLfloat)*model.normals.size(), &model.normals[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.vertices.size() + sizeof(GLfloat)*model.normals.size(), sizeof(GLfloat)*model.colors.size(), &model.colors[0]);
}

MyAppSharedContext::~MyAppSharedContext() {
	// Delete vbo
	glDeleteBuffers(1, &vbo);
}

void MyAppSharedContext::update(const VRGraphicsState &renderState, int version) {
	// In this application, only one window will update the shared context at a time, hence the locking
	if (version != this->version) {
		std::unique_lock<std::mutex> lock(mutex);
		if (version != this->version) {

			// Update context

			this->version = version;
		}
	}
}

