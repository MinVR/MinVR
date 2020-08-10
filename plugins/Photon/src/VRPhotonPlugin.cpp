/* ================================================================================

This file is part of the MinVR Open Source Project, which is developed and
maintained collaboratively by the University of Minnesota's Interactive
Visualization Lab and the Center for Computation and Visualization at Brown University 

File: VRPhotonPlugin.cpp

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

#include <iostream>
#include <plugin/VRPlugin.h>
#include "VRPhotonDevice.h"

// special: include this only once in one .cpp file per plugin
#include <plugin/VRPluginVersion.h>


namespace MinVR {

class VRPhotonPlugin : public VRPlugin {
public:
	PLUGIN_API VRPhotonPlugin() {
		//std::cout << "VrpnPlugin created." << std::endl;
	}

	PLUGIN_API virtual ~VRPhotonPlugin() {
		//std::cout << "VrpnPlugin destroyed." << std::endl;
	}

	PLUGIN_API void registerWithMinVR(VRMainInterface *vrMain)
	{
        //std::cout << "Registering VRVRPNPlugin." << std::endl;
		vrMain->getFactory()->registerItemType<VRInputDevice, VRPhotonDevice>("VRPhotonDevice");
	}

	PLUGIN_API void unregisterWithMinVR(VRMainInterface *vrMain)
	{
        //std::cout << "Unregistering VRVRPNlugin." << std::endl;
		// TODO
	}
};

} // end namespace

extern "C"
{
	PLUGIN_API MinVR::VRPlugin* createPlugin() {
		return new MinVR::VRPhotonPlugin();
	}
}

