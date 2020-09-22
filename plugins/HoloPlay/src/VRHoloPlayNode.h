/* ================================================================================

This file is part of the MinVR Open Source Project, which is developed and
maintained collaboratively by the University of Minnesota's Interactive
Visualization Lab and the Center for Computation and Visualization at Brown University

File: VRHoloPlayNode.h

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


/**
   \author     Benjamin Knorlein
   \maintainer Benjamin Knorlein

   \created 2020-09-04

   \file  VRHoloPlayNode.H

   \brief A displaynode which renders a holographic view to a LookingGlassFactory Display
*/


#ifndef VRHOLOPLAYNODE_H
#define VRHOLOPLAYNODE_H

#include <display/VRDisplayNode.h>
#include <main/VRFactory.h>
#include <plugin/VRPlugin.h>
#include <input/VRInputDevice.h>
#include "HoloPlayContext.hpp"


namespace MinVR {
  
/** OpenVR Implementation for the MinVR VRDisplayNode abstraction.
 */
class VRHoloPlayNode : public VRDisplayNode , public VRInputDevice{
public:
	PLUGIN_API VRHoloPlayNode(VRMainInterface *vrMain, const std::string &name, float camera_Size, float fov_angle, int devIndex, int quiver_quality, float nearPlane, float farPlane);
	
	PLUGIN_API virtual ~VRHoloPlayNode();

	PLUGIN_API virtual std::string getType() const { return "VRHoloPlayNode"; }

	PLUGIN_API virtual void render(VRDataIndex *renderState, VRRenderHandler *renderHandler);

	PLUGIN_API void appendNewInputEventsSinceLastCall(VRDataQueue* queue);
	
	PLUGIN_API static VRDisplayNode* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);
	
private:
	//Holoplay Context
	HoloPlayContext * m_holoplayContext;

	// create a viewTex for rendering
	unsigned int m_viewTex;
	//	// create framebuffer and texture color buffer
	unsigned int m_framebuffer, m_textureColorbuffer;
	// create a renderbuffer object for depth and stencil attachment
	unsigned int m_rbo;
	//size of the camera viewpoint, default 2
	float m_camera_Size;
	//field of view of the display - depending on the distance to the display, default 14
	float m_fov_angle;
	//device index of the holographic display, default 0
	int m_devIndex;
	//quality of the quiver // 0: 32 views
							// 1: 45 views, default
							// 2: 45 views for 8k display
	int m_quiver_quality;
	//Near Plane, default 0.01f
	float m_nearPlane;  
	//Far plane, default 100.9f
	float m_farPlane;	
	//button state from last loop
	int m_last_buttons[4];
};

} // end namespace

#endif
