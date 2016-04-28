/* ================================================================================

This file is part of the MinVR Open Source Project.

File: extend/Plugin.h

Original Author(s) of this File:
	Dan Orban, 2015, University of Minnesota

Author(s) of Significant Updates/Modifications to the File:
	...

-----------------------------------------------------------------------------------
Copyright (c) 2015 Regents of the University of Minnesota
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* The name of the University of Minnesota, nor the names of its
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

#ifndef PLUGIN_H_
#define PLUGIN_H_

#include <memory>

#include <main/VRMainInterface.h>

namespace MinVR {

#if defined(WIN32)
#define PLUGIN_API __declspec(dllexport)
#else
#define PLUGIN_API
#endif
  

/** Abstract interface for VRPlugin objects.  The convention is that each
    shared lib will define just one VRPlugin object, which is returned
    to the VRPluginManager after the shared library is loaded and the
    shared lib's extern "C" createVRPlugin() function is called.
*/
class VRPlugin {
public:

  virtual ~VRPlugin() {}
  
	/// When a plugin is loaded it must somehow register itself with MinVR
	/// so that MinVR can call functions within the plugin.  In the case of
	/// InputDevices and DisplayDevices, the registration involves adding
	/// a factory method to VRMain for creating the InputDevice or 
	/// DisplayDevice provided by the plugin.
	virtual void registerWithMinVR(VRMainInterface *vrMain) = 0;

	/// Needed?
	virtual void unregisterWithMinVR(VRMainInterface *vrMain) = 0;
};

}/* namespace MinVR */


#endif /* PLUGIN_H_ */
