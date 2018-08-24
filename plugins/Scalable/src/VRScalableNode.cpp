/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0.
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Ben Knorlein
 *
 *  The software in this file uses a proprietary library from Scalable Display Technologies of Cambridge, Massachusetts (scalabledisplay.com).
*/

#include "VRScalableNode.h"
#include "VRScalableHeaders.h"
#include <math/VRMath.h>
#include <cmath>
#define DEGTORAD 3.14159/180.0

namespace MinVR {

  VRScalableNode::VRScalableNode(const std::string &name, const std::string _meshName,
				 double _near, double _far) :
    VRDisplayNode(name),  meshName(_meshName), isInitialized(false),
    isLeftEye(false), near(_near), far(_far) {

    _addValueNeeded("CameraMatrix");
    _addValueNeeded("Eye");

    _valuesAdded.push_back("ProjectionMatrix");
    _valuesAdded.push_back("ViewMatrix");
}

VRScalableNode::~VRScalableNode() {
	if(isInitialized){
		EasyBlendSDK_Uninitialize(gMSDK );
		delete gMSDK;
	}
}

void
VRScalableNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler)
{
	if(!isInitialized){
		isInitialized = true;
		gMSDK = new EasyBlendSDK_Mesh;
		EasyBlendSDKError msdkErr = EasyBlendSDK_Initialize(meshName.c_str(), gMSDK );
		if ( msdkErr != EasyBlendSDK_ERR_S_OK )
		{
			std::cout << "Error on Init: " << EasyBlendSDK_GetErrorMessage(msdkErr) << std::endl;
			std::cout << "File is: " << meshName <<std::endl;
		}

		if(renderState->exists("Eye", "/") && ((std::string)renderState->getValue("Eye", "/")).compare("Right") == 0){
			EasyBlendSDK_SetInputReadBuffer(gMSDK,  GL_BACK_RIGHT);
			EasyBlendSDK_SetOutputDrawBuffer(gMSDK,  GL_BACK_RIGHT);
			isLeftEye = false;
		}else{
			EasyBlendSDK_SetInputReadBuffer(gMSDK,  GL_BACK_LEFT);
			EasyBlendSDK_SetOutputDrawBuffer(gMSDK,  GL_BACK_LEFT);
			isLeftEye = true;
		}
	}

	renderState->pushState();

	VRPoint3 pe(0,0,0);
	if (renderState->exists("CameraMatrix","/")){
		VRMatrix4 cameraMatrix = renderState->getValue("CameraMatrix");
		pe = pe + cameraMatrix.getColumn(3);
	}

	EasyBlendSDK_SetEyepoint(gMSDK,pe[0],pe[1],pe[2]);

	EasyBlendSDK_Frustum Frustum = gMSDK->Frustum;

	double l = near*tan(DEGTORAD*Frustum.LeftAngle);
	double r = near*tan(DEGTORAD*Frustum.RightAngle);
	double b = near*tan(DEGTORAD*Frustum.BottomAngle);
	double t = near*tan(DEGTORAD*Frustum.TopAngle);

	VRMatrix4 projMat = VRMatrix4::projection(l, r, b, t, near, far);
	renderState->addData("/ProjectionMatrix", projMat);

	VRMatrix4 rotZ = VRMatrix4::rotationZ(DEGTORAD*Frustum.ViewAngleA);
	VRMatrix4 rotY = VRMatrix4::rotationY(DEGTORAD*Frustum.ViewAngleB);
	VRMatrix4 rotX = VRMatrix4::rotationX(DEGTORAD*(Frustum.ViewAngleC-90));
	VRMatrix4 Mtrans = VRMatrix4::translation(VRPoint3(0, 0, 0) - pe);
	VRMatrix4 viewMat = rotZ * rotY * rotX * Mtrans;

	renderState->addData("/ViewMatrix", viewMat);

	VRDisplayNode::render(renderState, renderHandler);

	renderState->popState();
}

void
VRScalableNode::displayFinishedRendering(VRDataIndex *renderState) {
	VRDisplayNode::displayFinishedRendering(renderState);

	glFinish();
	EasyBlendSDK_TransformInputToOutput(gMSDK);
}

VRDisplayNode* VRScalableNode::create(VRMainInterface *vrMain, VRDataIndex *config,  const std::string &nameSpace) {
	std::string nodeNameSpace = config->validateNameSpace(nameSpace);

	std::string meshName = config->getValue("ScalableWarpFile", nodeNameSpace);
	double nearClip = config->getValue("NearClip", nodeNameSpace);
	double farClip = config->getValue("FarClip", nodeNameSpace);

	VRDisplayNode *node = new VRScalableNode(nameSpace, meshName,nearClip,farClip);

	return node;
}


} // end namespace
