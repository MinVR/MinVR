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

#ifdef _WIN32
#include "GL/glew.h"
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
#include "MyAppSharedContext.h"

/**
 * MyAppView represents a graphics window or context.  Contexts can share information via the sharedContext variable which
 * is passed in.  Views update and render the model based on the renderState that is passed in.
 */
class MyAppView {
public:
	/// Model and shared context are const because they should not be edited in the view.  This may cause synchronization
	/// issues for threaded displays.
	MyAppView(const MyAppModel& model, const MyAppSharedContext& sharedContext, const VRGraphicsState &renderState);
	virtual ~MyAppView();

	/// Update the window specific graphics objects
	void update(const VRGraphicsState &renderState);
	/// Render scene on window
	void render(const VRGraphicsState &renderState);

private:
	GLuint compileShader(const std::string& shaderText, GLuint shaderType);
	void linkShaderProgram(GLuint shaderProgram);

	GLuint vao, vshader, fshader, shaderProgram;
	const MyAppModel& model;
	const MyAppSharedContext& sharedContext;
};

#endif

