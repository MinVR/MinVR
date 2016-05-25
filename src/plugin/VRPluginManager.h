/* ================================================================================

This file is part of the MinVR Open Source Project.

File: extend/PluginManager.h

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

#ifndef VRPLUGINMANAGER_H_
#define VRPLUGINMANAGER_H_

#include <string>
#include <vector>

#include <plugin/VRPlugin.h>
#include <plugin/VRSharedLibrary.h>

namespace MinVR {

class VRMainInterface;
  
class VRPluginManager {
public:
	VRPluginManager(VRMainInterface *_vrMain);
	virtual ~VRPluginManager();

	/// Argument provides the path to the shared lib that contains the plugin
	bool loadPlugin(const std::string& filePath, const std::string& baseFileName);
	bool loadPlugin(const std::string& pluginFilePath);

private:
	VRMainInterface *_vrMain;
	std::vector<VRPlugin*> _plugins;
	std::vector<VRSharedLibrary*> _libraries;
};

} /* namespace MinVR */

#endif /* VRPLUGINMANAGER_H_ */
