/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Ben Knorlein
 */

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


#include "VROpenVRNode.h"
#include "VROpenVRRenderModelHandler.h"
#include <cmath>


namespace MinVR {

	VROpenVRNode::VROpenVRNode(VRMainInterface *vrMain, const std::string &name, double _near, double _far, bool draw_controller, bool hide_tracker, bool draw_HMD_Only, unsigned int MSAA_buffers, float deviceUnitsToRoomUnits, VRMatrix4 deviceToRoom) : VRDisplayNode(name), isInitialized(false), m_fNearClip(_near), m_fFarClip(_far), m_draw_controller(draw_controller), m_draw_HMD_Only(draw_HMD_Only), m_rendermodelhandler(NULL), m_MSAA_buffers(MSAA_buffers), deviceUnitsToRoomUnits(deviceUnitsToRoomUnits), deviceToRoom(deviceToRoom) {
	vr::EVRInitError eError = vr::VRInitError_None;
	m_pHMD = vr::VR_Init( &eError, vr::VRApplication_Scene );
	int idx = (int)name.find_last_of('/');
	std::cerr << name.substr(idx + 1) << std::endl;
	_inputDev = new VROpenVRInputDevice(m_pHMD, name.substr(idx + 1), this, deviceUnitsToRoomUnits, deviceToRoom);

	vrMain->addInputDevice(_inputDev);
	if ( eError != vr::VRInitError_None )
	{
		m_pHMD = NULL;
		std::cerr<< "Unable to init VR runtime: " <<vr::VR_GetVRInitErrorAsEnglishDescription( eError ) << std::endl;

		exit(0);
	}

  _addValueNeeded("WindowHeight");
  _addValueNeeded("WindowWidth");

  _valuesAdded.push_back("ProjectionMatrix");
  _valuesAdded.push_back("ViewMatrix");
  _valuesAdded.push_back("Eye");

	if (m_draw_controller)
		m_rendermodelhandler = new VROpenVRRenderModelHandler(m_pHMD, _inputDev, hide_tracker);

	vr::EVRInitError peError = vr::VRInitError_None;

	if ( !vr::VRCompositor() )
	{
		std::cerr <<   "Compositor initialization failed. See log file for details" <<std::endl;
		exit(0);
	}
}

VROpenVRNode::~VROpenVRNode() {
	if (m_rendermodelhandler){
		delete  m_rendermodelhandler;
		m_rendermodelhandler = NULL;
	}
	if( m_pHMD )
	{
		vr::VR_Shutdown();
		m_pHMD = NULL;
	}

	if(isInitialized){
		glDeleteRenderbuffers( 1, &leftEyeDesc.m_nDepthBufferId );
		if (m_MSAA_buffers > 1) glDeleteTextures(1, &leftEyeDesc.m_nRenderTextureId);
		glDeleteFramebuffers( 1, &leftEyeDesc.m_nRenderFramebufferId );
		glDeleteTextures( 1, &leftEyeDesc.m_nResolveTextureId );
		if (m_MSAA_buffers > 1) glDeleteFramebuffers(1, &leftEyeDesc.m_nResolveFramebufferId);

		glDeleteRenderbuffers( 1, &rightEyeDesc.m_nDepthBufferId );
		if (m_MSAA_buffers > 1) glDeleteTextures(1, &rightEyeDesc.m_nRenderTextureId);
		glDeleteFramebuffers( 1, &rightEyeDesc.m_nRenderFramebufferId );
		glDeleteTextures( 1, &rightEyeDesc.m_nResolveTextureId );
		if (m_MSAA_buffers > 1) glDeleteFramebuffers(1, &rightEyeDesc.m_nResolveFramebufferId);
	}


}

void
VROpenVRNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler)
{
	//return;
	if(!isInitialized){
		isInitialized = true;
#ifndef __APPLE__
		glewExperimental = true;
		GLenum nGlewError = glewInit();
		if (nGlewError != GLEW_OK)
		{
			std::cerr << __FUNCTION__ << " - Error initializing GLEW! \n" << glewGetErrorString( nGlewError ) << std::endl;
			exit(0);
		}
#endif
		glGetError(); // to clear the error caused deep in GLEW

		m_mat4ProjectionLeft = GetHMDMatrixProjectionEye( vr::Eye_Left );
		m_mat4ProjectionRight = GetHMDMatrixProjectionEye( vr::Eye_Right );
		m_mat4eyePosLeft = GetHMDMatrixPoseEye( vr::Eye_Left );
		m_mat4eyePosRight = GetHMDMatrixPoseEye( vr::Eye_Right );

		SetupStereoRenderTargets();
	}

	if (m_rendermodelhandler)
		m_rendermodelhandler->initModels();

	_inputDev->updatePoses();

	VRMatrix4 head_pose = _inputDev->getPose(vr::k_unTrackedDeviceIndex_Hmd).inverse();

	// Left Eye
	renderState->pushState();
	if (m_MSAA_buffers > 1) glEnable(GL_MULTISAMPLE);
	glBindFramebuffer( GL_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId );
 	glViewport(0, 0, m_nRenderWidth, m_nRenderHeight );
 	glClearColor( 0, 0, 0, 1 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	VRMatrix4 view_left = m_mat4eyePosLeft * head_pose;
	renderState->addData("ProjectionMatrix", m_mat4ProjectionLeft);
	renderState->addData("ViewMatrix", view_left * VRMatrix4::scale(VRVector3(1.0f / deviceUnitsToRoomUnits, 1.0f / deviceUnitsToRoomUnits, 1.0f / deviceUnitsToRoomUnits)) *deviceToRoom);
	renderState->addData("Eye", "Left");
	if (_children.size() == 0) {
		renderHandler->onVRRenderScene(*renderState);
	}
	else {
		VRDisplayNode::render(renderState, renderHandler);
	}

	if (m_rendermodelhandler)
		m_rendermodelhandler->draw(m_mat4ProjectionLeft, view_left);

 	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	if (m_MSAA_buffers > 1) {
		glDisable(GL_MULTISAMPLE);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, leftEyeDesc.m_nResolveFramebufferId);
		glBlitFramebuffer(0, 0, m_nRenderWidth, m_nRenderHeight, 0, 0, m_nRenderWidth, m_nRenderHeight,
			GL_COLOR_BUFFER_BIT,
			GL_LINEAR);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}
	renderState->popState();

	// Right Eye
	renderState->pushState();
	if (m_MSAA_buffers > 1) glEnable(GL_MULTISAMPLE);
	glBindFramebuffer( GL_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId );
 	glViewport(0, 0, m_nRenderWidth, m_nRenderHeight );
	glClearColor( 0, 0, 0, 1 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	VRMatrix4 view_right = m_mat4eyePosRight * head_pose;

	renderState->addData("ProjectionMatrix",m_mat4ProjectionRight);
	renderState->addData("ViewMatrix", view_right * VRMatrix4::scale(VRVector3(1.0f / deviceUnitsToRoomUnits, 1.0f / deviceUnitsToRoomUnits, 1.0f / deviceUnitsToRoomUnits)) *deviceToRoom);
	renderState->addData("Eye", "Right");
	if (_children.size() == 0) {
		renderHandler->onVRRenderScene(*renderState);
	}
	else {
		VRDisplayNode::render(renderState, renderHandler);
	}

	if (m_rendermodelhandler)
		m_rendermodelhandler->draw(m_mat4ProjectionRight, view_right);

 	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	if (m_MSAA_buffers > 1) {
		glDisable(GL_MULTISAMPLE);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rightEyeDesc.m_nResolveFramebufferId);
		glBlitFramebuffer(0, 0, m_nRenderWidth, m_nRenderHeight, 0, 0, m_nRenderWidth, m_nRenderHeight,
			GL_COLOR_BUFFER_BIT,
			GL_LINEAR);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}
	renderState->popState();

	vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)leftEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	vr::EVRCompositorError error =  vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture );

	vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)rightEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	error = vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture  );


	glFlush();
	vr::VRCompositor()->PostPresentHandoff();

	if (!m_draw_HMD_Only){
		renderState->pushState();
		int width = renderState->getValue("WindowWidth");
		int height = renderState->getValue("WindowHeight");

		glViewport(0, 0, width, height);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderState->addData("ProjectionMatrix", m_mat4ProjectionRight);
		renderState->addData("ViewMatrix", view_right);
		renderState->addData("Eye", "Cyclops");
		if (_children.size() == 0) {
			renderHandler->onVRRenderScene(*renderState);
		}
		else {
			VRDisplayNode::render(renderState, renderHandler);
		}

		if (m_rendermodelhandler)
			m_rendermodelhandler->draw(m_mat4ProjectionRight, view_right);

		renderState->popState();
	}
}

VRDisplayNode* VROpenVRNode::create(VRMainInterface *vrMain, VRDataIndex *config,  const std::string &nameSpace) {
	std::string nodeNameSpace = config->validateNameSpace(nameSpace);

	int drawController = true;
	if (config->exists("DrawController", nameSpace))
	{
		drawController = config->getValue("DrawController", nameSpace);
	}
	int hide_tracker = false;
	if (config->exists("HideTracker", nameSpace))
	{
		hide_tracker = config->getValue("HideTracker", nameSpace);
	}

	bool draw_HMD_Only = false;
	if (config->exists("DrawHMDOnly", nameSpace) && ((int)config->getValue("DrawHMDOnly", nameSpace)))
	{
		draw_HMD_Only = true;
	}
	int MSAA_buffers = 4;
	if (config->exists("MSAA_buffers", nameSpace))
	{
		MSAA_buffers = config->getValue("MSAA_buffers", nameSpace);
		MSAA_buffers = (MSAA_buffers < 1) ? 1 : MSAA_buffers;
	}

	float deviceUnitsToRoomUnits = 1.0f;
	if (config->exists("DeviceUnitsToRoomUnitsScale", nameSpace)) {
		deviceUnitsToRoomUnits = config->getValue("DeviceUnitsToRoomUnitsScale", nameSpace);
	}

	VRMatrix4 deviceToRoom = VRMatrix4();
	if (config->exists("DeviceToRoom", nameSpace)) {
		deviceToRoom = config->getValue("DeviceToRoom", nameSpace);
		deviceToRoom = deviceToRoom.orthonormal();
	}

	VRDisplayNode *node = new VROpenVRNode(vrMain, nameSpace, 0.1f, 100000.0f, drawController, hide_tracker, draw_HMD_Only, MSAA_buffers, deviceUnitsToRoomUnits, deviceToRoom);
	return node;
}

bool VROpenVRNode::CreateFrameBuffer( int nWidth, int nHeight, FramebufferDesc &framebufferDesc )
{
	glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId );
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);

	glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
	if (m_MSAA_buffers > 1){
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight);
	}
	else
	{
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, nWidth, nHeight);
	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,	framebufferDesc.m_nDepthBufferId );

	if (m_MSAA_buffers > 1){
		glGenTextures(1, &framebufferDesc.m_nRenderTextureId);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, true);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);

		glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId);
	}

	glGenTextures(1, &framebufferDesc.m_nResolveTextureId );
	glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0);

	// check FBO status
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	return true;
}

bool VROpenVRNode::SetupStereoRenderTargets()
{
	if ( !m_pHMD )
		return false;

	m_pHMD->GetRecommendedRenderTargetSize( &m_nRenderWidth, &m_nRenderHeight );

	CreateFrameBuffer( m_nRenderWidth, m_nRenderHeight, leftEyeDesc );
	CreateFrameBuffer( m_nRenderWidth, m_nRenderHeight, rightEyeDesc );

	return true;
}

VRMatrix4 VROpenVRNode::GetHMDMatrixProjectionEye( vr::Hmd_Eye nEye )
{
	if ( !m_pHMD )
		return VRMatrix4();

	vr::HmdMatrix44_t mat = m_pHMD->GetProjectionMatrix( nEye, (float)m_fNearClip, (float)m_fFarClip);

    VRMatrix4 out = VRMatrix4::fromRowMajorElements(
		mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
		mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
		mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
		mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
	);
	return out.transpose();
}

VRMatrix4 VROpenVRNode::GetHMDMatrixPoseEye( vr::Hmd_Eye nEye )
{
	if ( !m_pHMD )
		return VRMatrix4();

	vr::HmdMatrix34_t matEyeRight = m_pHMD->GetEyeToHeadTransform( nEye );
    VRMatrix4 matrixObj = VRMatrix4::fromRowMajorElements(
		matEyeRight.m[0][0], matEyeRight.m[1][0], matEyeRight.m[2][0], 0.0,
		matEyeRight.m[0][1], matEyeRight.m[1][1], matEyeRight.m[2][1], 0.0,
		matEyeRight.m[0][2], matEyeRight.m[1][2], matEyeRight.m[2][2], 0.0,
		matEyeRight.m[0][3], matEyeRight.m[1][3], matEyeRight.m[2][3], 1.0f
		);

	return matrixObj.transpose().inverse();
}


} // end namespace
