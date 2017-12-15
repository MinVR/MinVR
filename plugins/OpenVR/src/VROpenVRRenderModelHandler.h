/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 * code based on the hello_vr_opengl from openvr https://github.com/ValveSoftware/openvr
 * 
 * Code author(s): 
 * 		Ben Knorlein
 */



#ifndef VROPENVRRENDERMODELHANDLER_H_
#define VROPENVRRENDERMODELHANDLER_H_

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


#include <openvr.h>
#include <string>
#include <vector>
#include <queue>

namespace MinVR {
	class VRMatrix4;
	class VROpenVRRenderModel;
	class VROpenVRInputDevice;
	class VROpenVRRenderModelHandler {

public:
	VROpenVRRenderModelHandler(vr::IVRSystem *pHMD, VROpenVRInputDevice* inputDevice, bool hide_tracker);
	~VROpenVRRenderModelHandler();

	void queueModelForLoading(vr::TrackedDeviceIndex_t unTrackedDeviceIndex){ modelLoaderQueue.push(unTrackedDeviceIndex); }
	void initModels();
	void draw(VRMatrix4 projection, VRMatrix4 modelview);

private:
	vr::IVRSystem *m_pHMD;
	VROpenVRInputDevice * m_inputDevice;
	std::vector< VROpenVRRenderModel * > m_vecRenderModels;

	VROpenVRRenderModel *m_rTrackedDeviceToRenderModel[vr::k_unMaxTrackedDeviceCount];
	std::vector<VROpenVRRenderModel*> m_rTrackedDeviceToRenderModelComponents[vr::k_unMaxTrackedDeviceCount];
	std::string m_rDeviceName[vr::k_unMaxTrackedDeviceCount];
	std::vector<std::string> m_rComponentName[vr::k_unMaxTrackedDeviceCount];
	bool hasComponent[vr::k_unMaxTrackedDeviceCount];

	std::queue<vr::TrackedDeviceIndex_t> modelLoaderQueue;

	VROpenVRRenderModel* findOrLoadRenderModel(const char *pchRenderModelName);
	void setupRenderModelForTrackedDevice(vr::TrackedDeviceIndex_t unTrackedDeviceIndex);

	GLuint m_unRenderModelProgramID;
	GLint m_nRenderModelMatrixLocation;
	GLint m_nRenderModelState;

	bool m_hide_tracker;
};


} /* namespace MinVR */

#endif /* VROPENVRRENDERMODELHANDLER_H_ */
