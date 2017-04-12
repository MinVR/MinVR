/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef MYAPPVIEW_H_
#define MYAPPVIEW_H_

#include <iostream>

#include "GL/glew.h"
#ifdef _WIN32
#include "GL/wglew.h"
#elif (!defined(__APPLE__))
#include "GL/glxew.h"
#endif

// OpenGL Headers
#if defined(WIN32)
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#elif defined(__APPLE__)
#define GL_GLEXT_PROTOTYPES
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

// MinVR header
#include <api/MinVR.h>
using namespace MinVR;

#include "MyAppModel.h"

class MyAppView {
public:
	MyAppView(MyAppModel& model, const VRGraphicsState &renderState);
	virtual ~MyAppView();

	void update(const VRGraphicsState &renderState);
	void render(const VRGraphicsState &renderState);

private:
	GLuint compileShader(const std::string& shaderText, GLuint shaderType);
	void linkShaderProgram(GLuint shaderProgram);

	GLuint vbo, vao, vshader, fshader, shaderProgram;
	MyAppModel& model;
};

#endif

