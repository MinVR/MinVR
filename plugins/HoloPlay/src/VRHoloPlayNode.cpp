/* ================================================================================

This file is part of the MinVR Open Source Project, which is developed and
maintained collaboratively by the University of Minnesota's Interactive
Visualization Lab and the Center for Computation and Visualization at Brown University

File: VRHoloPlayNode.cpp

Original Author(s) of this File:
	9/4/2020 - Benjamin Knorlein - Brown Universiy

Author(s) of Significant Updates/Modifications to the File:

-----------------------------------------------------------------------------------
Copyright (c) 2020 Brown University
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* Neither the name of the University of Minnesota, Brown University, nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================ */

#include "VRHoloPlayNode.h"

#include <glm/gtc/type_ptr.inl>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glError.hpp"
#include <GL/glew.h>
#include "api/VRButtonEvent.h"

namespace MinVR {

	VRHoloPlayNode::VRHoloPlayNode(VRMainInterface *vrMain, const std::string &name, float camera_Size, float fov_angle, int devIndex, int quiver_quality, float nearPlane, float farPlane) 
		: VRDisplayNode(name), m_holoplayContext{nullptr}, m_camera_Size{ camera_Size }, m_fov_angle{ fov_angle }, m_devIndex{ devIndex }, m_quiver_quality{ quiver_quality }
		, m_nearPlane{ nearPlane }, m_farPlane{ farPlane }, m_last_buttons{0,0,0,0}{
	 _addValueNeeded("WindowHeight");
	 _addValueNeeded("WindowWidth");
	 _addValueNeeded("CameraMatrix");

	 _valuesAdded.push_back("ProjectionMatrix");
	 _valuesAdded.push_back("ViewMatrix");
	 _valuesAdded.push_back("Eye");
	
	 vrMain->addInputDevice(this);
	}

	VRHoloPlayNode::~VRHoloPlayNode() {
		if (m_holoplayContext) {
			delete m_holoplayContext;
			glDeleteFramebuffers(1, &m_framebuffer);
			glDeleteTextures(1, &m_textureColorbuffer);
			glDeleteRenderbuffers(1, &m_rbo);
			glDeleteTextures(1, &m_viewTex);
		}
	}
	
	void
	VRHoloPlayNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler)
	{
		if (!m_holoplayContext) {
			m_holoplayContext = new HoloPlayContext(m_camera_Size,m_fov_angle,m_devIndex,m_quiver_quality,m_nearPlane,m_farPlane);
			int width = renderState->getValue("WindowWidth");
			int height = renderState->getValue("WindowHeight");
			m_holoplayContext->setupViewTextureAndFrameBuffer(m_viewTex, m_framebuffer, m_textureColorbuffer, m_rbo, width, height);
		}

		VRMatrix4 cameraMatrix = renderState->getValue("CameraMatrix");
		glm::mat4 viewMatrix_glm = glm::make_mat4(cameraMatrix.inverse().getArray());
		for (int viewIndex = 0; viewIndex < m_holoplayContext->getNumberViews(); viewIndex++)
		{
			// 1. bind to framebuffer and draw scene as we normally would to color texture
			glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

			// 2. set up the camera rotation and position for current view
			m_holoplayContext->setupVirtualCameraForView(viewIndex, viewMatrix_glm);
			renderState->pushState();
				// 3. render the scene according to the view
				renderState->addData("ProjectionMatrix", VRMatrix4(glm::value_ptr(m_holoplayContext->GetProjectionMatrixOfCurrentView())));
				renderState->addData("ViewMatrix",  VRMatrix4(glm::value_ptr(m_holoplayContext->GetViewMatrixOfCurrentView())));
				renderState->addData("Eye", "LKview" + std::to_string(viewIndex));
				if (_children.size() == 0) {
					renderHandler->onVRRenderScene(*renderState);
				}
				else {
					VRDisplayNode::render(renderState, renderHandler);
				}
			renderState->popState();

			// 4. now bind back to default framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			GLboolean isDepthEnabled;
			glGetBooleanv(GL_DEPTH_TEST, &isDepthEnabled);
			glDisable(GL_DEPTH_TEST); 
		
			// 5. now render on view textureglActiveTexture(GL_TEXTURE0)
			//glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_textureColorbuffer); // use the color attachment texture as the texture

			// 6. copy current view texture to the responding view grid in quilt
			m_holoplayContext->copyViewToQuilt(viewIndex);
			
			if (isDepthEnabled)
				glEnable(GL_DEPTH_TEST);
		}
			
		GLboolean isDepthEnabled;
		glGetBooleanv(GL_DEPTH_TEST, &isDepthEnabled);
		glDisable(GL_DEPTH_TEST);

		// draw the light field image
		m_holoplayContext->drawLightField();

		if (isDepthEnabled)
			glEnable(GL_DEPTH_TEST);

		glFinish();
	}

	void VRHoloPlayNode::appendNewInputEventsSinceLastCall(VRDataQueue* queue) {
		//std::cerr << "Append " << std::endl;
		int m_buttons[4];
		for (int i = 0; i < 4;  i++) {
			std::string desc = "/buttons/" + std::to_string(i);
			m_buttons[i] = hpc_GetDevicePropertyInt(m_devIndex, desc.c_str());
		}

		for (int i = 0; i < 4; i++) {
			if (m_last_buttons[i] != m_buttons[i]) {
				
				std::string event_name = "HoloPlay_Button_";
				switch (i) { 
					case 0: 
						event_name += "Square";
						break;
					case 1:
						event_name += "Left";
						break;
					case 2:
						event_name += "Right";
						break;
					case 3:
						event_name += "Circle";
						break;
				}
				if (m_buttons > 0) {
					event_name = event_name + "_Up";
					VRDataIndex di = VRButtonEvent::createValidDataIndex(event_name, 1);
					queue->push(di);
				}
				else {
					event_name = event_name + "_Down";
					VRDataIndex di = VRButtonEvent::createValidDataIndex(event_name, 0);
					queue->push(di);
				}
			}
			m_last_buttons[i] = m_buttons[i];
		}
	}

	VRDisplayNode* VRHoloPlayNode::create(VRMainInterface *vrMain, VRDataIndex *config,  const std::string &nameSpace) {
		std::string nodeNameSpace = config->validateNameSpace(nameSpace);

		float camera_Size = 2.0f;
		if (config->exists("HoloCameraSize", nameSpace)) {
			camera_Size = config->getValue("HoloCameraSize", nameSpace);
		}

		float fov_angle = 14.0f;
		if (config->exists("HoloFieldOfView", nameSpace)) {
			fov_angle = config->getValue("HoloFieldOfView", nameSpace);
		}

		int devIndex = 0;
		if (config->exists("HoloDeviceIndex", nameSpace)) {
			devIndex = config->getValue("HoloDeviceIndex", nameSpace);
		}

		int quiver_quality = 1;
		if (config->exists("HoloQuiverQuality", nameSpace)) {
			quiver_quality = config->getValue("HoloQuiverQuality", nameSpace);
		}

		float nearPlane = 0.01f;
		if (config->exists("NearClip", nameSpace)) {
			nearPlane = config->getValue("NearClip", nameSpace);
		}
		float farPlane = 100.0f;
		if (config->exists("FarClip", nameSpace)) {
			farPlane = config->getValue("FarClip", nameSpace);
		}

		VRDisplayNode *node = new VRHoloPlayNode(vrMain, nameSpace, camera_Size, fov_angle, devIndex, quiver_quality, nearPlane, farPlane);
		return node;
	}
} // end namespace

