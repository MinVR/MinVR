/* ================================================================================

This file is part of the MinVR Open Source Project, which is developed and
maintained collaboratively by the University of Minnesota's Interactive
Visualization Lab and the Center for Computation and Visualization at Brown University

File: VRPhotonDevice.h

Original Author(s) of this File:
	8/4/2020 - Benjamin Knorlein - Brown Universiy

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
   
   \created 2020-08-04
   
   \file  VRPhotonDevice.H
   
   \brief A device that connects to Photonengine server to share events across multiple minvr applications
*/

#ifndef VRPHOTONDEVICE_H
#define VRPHOTONDEVICE_H

// Note: This include ordering is important!
#include <vector>
#include <iostream>
#include <unordered_set>
#include <unordered_map> 

#include <config/VRDataIndex.h>
#include <input/VRInputDevice.h>
#include <main/VRFactory.h>
#include <plugin/VRPlugin.h>

#include "Photon_lib.h"
#include "StdIO_UIListener.h"

namespace MinVR {

/**
*/
class VRPhotonDevice : public VRInputDevice
{

public:
	PLUGIN_API VRPhotonDevice(std::string appName, std::string appID, std::string appVersion, std::string playerName
		, bool receiveOnly, std::vector<std::string> blacklist, std::vector<std::string> replacements);
	PLUGIN_API virtual ~VRPhotonDevice();

	PLUGIN_API void appendNewInputEventsSinceLastCall(VRDataQueue *inputEvents);
	PLUGIN_API static VRInputDevice* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);

private:
    std::vector<VRDataIndex>   _pendingEvents;
	PhotonLib * m_photon;
	StdIO_UIListener * m_output_listener;

	bool m_receiveOnly;
	std::unordered_set <std::string> m_blacklist;
	std::unordered_map <std::string, std::string> m_replacements;
	int m_lastsend;
	int m_lastreceived;
};




} // end namespace

#endif




