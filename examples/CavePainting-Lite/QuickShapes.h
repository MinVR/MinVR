/**
 This file is part of the MinVR Open Source Project, which is developed and
 maintained collaboratively by the University of Minnesota and Brown University.
 
 Copyright (c) 2016-2017 Regents of the University of Minnesota and Brown University.
 This software is distributed under the BSD-3 Clause license, which can be found
 at: http://github.minvr.org/blob/master/LICENSE.txt
 
 Original Author(s) of this File:
	Dan Keefe, 2017, University of Minnesota
	
 Author(s) of Significant Updates/Modifications to the File:
	...
 */


#ifndef QUICKSHAPES_H
#define QUICKSHAPES_H


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


/** This class provides a quick way to draw shapes for use in debugging or
    as cursors for VR input devices etc. using the GLSL pipeline.  You can specify
    the color for each shape as part of the rendering call.  Other lighting
    parameters (the intensity of the light, advanced material properties) are 
    pre-set to reasonable defaults that apply to all of the shapes drawn.  You can
    edit these if you wish, but note that the intent of this class is just to provide
    a quick way to draw shapes -- this is not the right tool to use if you wish to
    do quality renderings and use multiple types of materials.
 */
class QuickShapes {
public:
    
    QuickShapes();
    virtual ~QuickShapes();
    

	/** Draws a cube with extents -1 to 1 given the model, view, and projection 
        matrices provided and using the supplied color as a material property.
	 */
	void drawCube(const float *modelMatrix, const float *viewMatrix,
                  const float *projectionMatrix, const float *color);


    /** Draws a cylinder with radius 1 and height y=-1 to 1 given the model, 
        view, and projection matrices provided and using the supplied color as a
        material property.
     */
    void drawCylinder(const float *modelMatrix, const float *viewMatrix,
                             const float *projectionMatrix, const float *color);

    
    /** Draws a cylinder with radius 1 and height y=-1 to 1 given the model,
     view, and projection matrices provided and using the supplied color as a
     material property.
     */
    void drawSphere(const float *modelMatrix, const float *viewMatrix,
                           const float *projectionMatrix, const float *color);
    
    
    /** Draws the classic 3D paintbrush cursor from the 2001 Keefe et al.
        CavePainting paper. The tip of the brush is at (0,0,0), the front
        flat edge runs along the X axis, and the handle runs in the +Z direction.
     */
    void drawBrush(const float *modelMatrix, const float *viewMatrix,
                          const float *projectionMatrix, const float *color);
    
    
    /** Draws a cubbie with extents -1 to 1 given the model, view, and projection
     matrices provided and using the supplied color as a material property.
     */
    void drawCubbie(const float *modelMatrix, const float *viewMatrix,
                           const float *projectionMatrix, const float *color);
    

	void setLightPos(const float *pos) {
		s_lightPos[0] = pos[0];  s_lightPos[1] = pos[1]; s_lightPos[2] = pos[2];
	}

	void setLightIntensityAmbient(const float *i) {
		s_lightAmb[0] = i[0];  s_lightAmb[1] = i[1]; s_lightAmb[2] = i[2];
	}

	void setLightIntensityDiffuse(const float *i) {
		s_lightDiff[0] = i[0];  s_lightDiff[1] = i[1]; s_lightDiff[2] = i[2];
	}

	void setLightIntensitySpecular(const float *i) {
		s_lightSpec[0] = i[0];  s_lightSpec[1] = i[1]; s_lightSpec[2] = i[2];
	}

	void setMaterialReflectanceSpecular(const float *r) {
		s_matSpec[0] = r[0];  s_matSpec[1] = r[1]; s_matSpec[2] = r[2];
	}

	void setMaterialReflectanceShinniness(float s) {
		s_matShin = s;
	}

private:

    void freeGPUMemory();
    
	void initCube();
	GLuint s_cubeVAO;
	GLuint s_cubeVBO;

    void initCyl();
    GLuint s_cylVAO;
    GLuint s_cylVBO;
    int s_cylNVerts;
    
    void initSph();
    GLuint s_sphVAO;
    GLuint s_sphVBO;
    int s_sphNVerts;
    
    void initBrush();
    GLuint s_brushVAO;
    GLuint s_brushVBO;

    void initCubbie();
    GLuint s_cubbieVAO;
    GLuint s_cubbieVBO;
    
	float s_lightPos[3];
	float s_lightAmb[3];
	float s_lightDiff[3];
	float s_lightSpec[3];
	float s_matSpec[3];
	float s_matShin;
};

#endif