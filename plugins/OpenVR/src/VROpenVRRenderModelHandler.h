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

#include <GL/glew.h>
#include <openvr.h>
#include <string>
#include <vector>
#include <queue>

namespace MinVR {
	class VROpenVRRenderModel;
	class VROpenVRInputDevice;
	class VROpenVRRenderModelHandler {

public:
	VROpenVRRenderModelHandler(vr::IVRSystem *pHMD, VROpenVRInputDevice* inputDevice);
	~VROpenVRRenderModelHandler();

	void queueModelForLoading(vr::TrackedDeviceIndex_t unTrackedDeviceIndex){ modelLoaderQueue.push(unTrackedDeviceIndex); }
	void initModels();
	void draw();

private:
	vr::IVRSystem *m_pHMD;
	VROpenVRInputDevice * m_inputDevice;
	std::vector< VROpenVRRenderModel * > m_vecRenderModels;
	VROpenVRRenderModel *m_rTrackedDeviceToRenderModel[vr::k_unMaxTrackedDeviceCount];
	std::queue<vr::TrackedDeviceIndex_t> modelLoaderQueue;

	VROpenVRRenderModel* findOrLoadRenderModel(const char *pchRenderModelName);
	void setupRenderModelForTrackedDevice(vr::TrackedDeviceIndex_t unTrackedDeviceIndex);

	GLuint m_unRenderModelProgramID;
};


} /* namespace MinVR */

#endif /* VROPENVRRENDERMODELHANDLER_H_ */
