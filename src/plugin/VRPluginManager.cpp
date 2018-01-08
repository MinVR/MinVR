/* ================================================================================

This file is part of the MinVR Open Source Project.

File: plugin/PluginManager.cpp

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

#include <iostream>

#include <main/VRMainInterface.h>
#include <plugin/VRPluginManager.h>
#include <main/VRLog.h>

// special: include this only once here and once in one .cpp file per plugin
#include <plugin/VRPluginVersion.h>


namespace MinVR {

VRPluginManager::VRPluginManager(VRMainInterface *vrMain) : _vrMain(vrMain) {
}

VRPluginManager::~VRPluginManager() {
	for (size_t f = 0; f < _plugins.size(); f++)
	{
		_plugins[f]->unregisterWithMinVR(_vrMain);
		delete _plugins[f];
	}

	for (size_t f = 0; f < _libraries.size(); f++)
	{
		delete _libraries[f];
	}
}

bool VRPluginManager::loadPlugin(const std::string& filePath, const std::string& name) {
#if defined(WIN32)
	std::string path = filePath + "/" + name + ".dll";
#elif defined(__APPLE__)
	std::string path = filePath + "/lib" + name + ".dylib";
#else
	std::string path = filePath + "/lib" + name + ".so";
#endif
    
    //std::cerr << "Calling loadPlugin with: " << path << std::endl;

	return loadPlugin(path);
}

bool VRPluginManager::loadPlugin(const std::string& pluginFilePath) {
    //std::cerr << "Trying to load: " << pluginFilePath << std::endl;
	VRSharedLibrary* lib = new VRSharedLibrary(pluginFilePath);
	if (lib->isLoaded())
	{
		typedef int version_t();
		version_t* getVersion = lib->loadSymbol<version_t>("getPluginFrameworkVersion");
		if (getVersion() != getPluginFrameworkVersion())
		{
			//std::cerr << "Cannot load plugin: " << pluginFilePath << " - Incorrect framework version." << std::endl;
			delete lib;
			return false;
		}

		typedef VRPlugin* create_t();
		create_t* createVRPlugin = lib->loadSymbol<create_t>("createPlugin");
		if (createVRPlugin == NULL)
		{
			//std::cerr << "Cannot load plugin: " << pluginFilePath << " - createVRPlugin funciton not found." << std::endl;
			delete lib;
			return false;
		}

		VRPlugin* plugin = createVRPlugin();
		
		plugin->registerWithMinVR(_vrMain);

		_plugins.push_back(plugin);
		_libraries.push_back(lib);

		return true;
	}
	else {
		delete lib;
	}

	return false;
}


} /* namespace extend */
