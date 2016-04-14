//
//  OpenGLGraphics.h
//  MinVR
//
//  Created by Dan Orban on 1/7/16.
//
//

#ifndef MinVR_OpenGLGraphics_h
#define MinVR_OpenGLGraphics_h

#if defined(WIN32)
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#elif defined(__APPLE__)
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#endif


#endif
