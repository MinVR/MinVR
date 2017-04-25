/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef MYAPPSHAREDCONTEXT_H_
#define MYAPPSHAREDCONTEXT_H_

#include <iostream>

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

#include <mutex>

// MinVR header
#include <api/MinVR.h>
using namespace MinVR;

#include "MyAppModel.h"

class MyAppSharedContext {
public:
	MyAppSharedContext(const MyAppModel& model, const VRGraphicsState &renderState);
	virtual ~MyAppSharedContext();

	void update(const VRGraphicsState &renderState, int version);

	GLuint getVbo() const {
		return vbo;
	}

private:
	GLuint vbo;
	const MyAppModel& model;
	std::mutex mutex;
	int version;
};

#endif

