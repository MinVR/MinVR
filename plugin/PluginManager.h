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

#ifndef PLUGINMANAGER_H_
#define PLUGINMANAGER_H_

#include <string>
#include <vector>
#include "SharedLibrary.h"
#include "PluginInterface.h"

namespace extend {

class PluginManager {
public:
	PluginManager(PluginInterface *interface);
	virtual ~PluginManager();

	void loadPlugin(const std::string& filePath);

private:
	PluginInterface *_interface;
	std::vector<SharedLibraryRef> _plugins;
};

PluginManager::PluginManager(PluginInterface *interface) : _interface(interface) {
}

PluginManager::~PluginManager() {
}

void PluginManager::loadPlugin(const std::string& filePath) {
	SharedLibraryRef lib = SharedLibraryRef(new SharedLibrary(filePath));
	if (lib->isLoaded())
	{
		typedef int version_t();
		version_t* getVersion = lib->loadSymbol<version_t>("getPluginFrameworkVersion");
		if (getVersion() != getMinVRPluginFrameworkVersion())
		{
			//Logger::getInstance().assertMessage(false, "Cannot load plugin: " + filePath + " - Incorrect framework version");
			return;
		}

		typedef PluginRef load_t();
		load_t* loadPlugin = lib->loadSymbol<load_t>("loadPlugin");
		if (loadPlugin == NULL)
		{
			return;
		}
		PluginRef plugin = loadPlugin();
		if (!plugin->registerPlugin(_interface))
		{
			//Logger::getInstance().assertMessage(false, "Failed registering plugin: " + filePath);
			return;
		}

		_plugins.push_back(lib);
	}
}

} /* namespace extend */

#endif /* PLUGINMANAGER_H_ */
