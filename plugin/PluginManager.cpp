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

#include "PluginManager.h"
#include "Plugin.h"
#include <iostream>

namespace MinVR {

PluginManager::PluginManager() : _interfaces() {
}


PluginManager::PluginManager(std::vector<PluginInterface*> interfaces) : _interfaces(interfaces) {
}

PluginManager::~PluginManager() {
	for (int f = 0; f < _plugins.size(); f++)
	{
		for (int i = 0; i < _interfaces.size(); i++)
		{
			_plugins[f]->unregisterPlugin(_interfaces[i]);
		}

		delete _plugins[f];
	}

	for (int f = 0; f < _libraries.size(); f++)
	{
		delete _libraries[f];
	}
}

void PluginManager::addInterface(PluginInterface* iface) {
	_interfaces.push_back(iface);
}

bool PluginManager::loadPlugin(const std::string& filePath, const std::string& name) {
#if defined(WIN32)
	std::string path = filePath + "/bin/" + name + ".dll";
#elif defined(__APPLE__)
	std::string path = filePath + "/lib/lib" + name + ".dylib";
#else
	std::string path = filePath + "/lib/lib" + name + ".so";
#endif

	SharedLibrary* lib = new SharedLibrary(path);
	if (lib->isLoaded())
	{
		typedef int version_t();
		version_t* getVersion = lib->loadSymbol<version_t>("getPluginFrameworkVersion");
		if (getVersion() != getPluginFrameworkVersion())
		{
			std::cout << "Cannot load plugin: " << path << " - Incorrect framework version." << std::endl;
			return false;
		}

		typedef FrameworkPlugin* load_t();
		load_t* loadPlugin = lib->loadSymbol<load_t>("loadPlugin");
		if (loadPlugin == NULL)
		{
			std::cout << "Cannot load plugin: " << path << " - loadPlugin funciton not found." << std::endl;
			return false;
		}

		FrameworkPlugin* plugin = loadPlugin();
		int countRegistered = 0;
		for (int f = 0; f < _interfaces.size(); f++)
		{
			if (plugin->registerPlugin(_interfaces[f]))
			{
				countRegistered++;
			}
		}

		if (countRegistered > 0)
		{
			_plugins.push_back(plugin);
			_libraries.push_back(lib);
		}
		else
		{
			delete plugin;
			delete lib;
		}

		return true;
	}

	return false;
}

} /* namespace extend */
