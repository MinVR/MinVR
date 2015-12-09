/* ================================================================================

This file is part of the MinVR Open Source Project.

File: main.cpp

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
#include "plugin/PluginManager.h"
#include <vector>
#include "event/VREvent.h"
#include "event/VRInputDevice.h"
#include "data/XMLUtils.h"
#include "net/VRNetServer.h"

#include <sstream>
#include <fstream>

#if defined(WIN32)
#include <Windows.h>
#else
#include <dirent.h>
#endif

using namespace MinVR;
using namespace std;

std::vector<std::string> listDirectory(const std::string& path, bool directories);

std::vector<VRInputDeviceDriver*> inputDeviceDrivers;

class App : public VRInputDeviceInterface
{
public:
	void addInputDeviceDriver(VRInputDeviceDriver* driver)
	{
		inputDeviceDrivers.push_back(driver);
	}
};

int main(int argc, char **argv) {
  cout << "Registering plugins..." << endl;
  cout << "Plugin path: " << PLUGINPATH << endl;

  App app;

  PluginManager pluginManager;
  pluginManager.addInterface(dynamic_cast<VRInputDeviceInterface*>(&app));

  string config = "";
#ifdef MinVR_DEBUG
  config = "d";
#endif

  std::string xml_string="";
  std::string fileName = argv[1];
  ifstream file(fileName.c_str());
  std::cout << "Reading from file = " << fileName << std::endl;

  std::vector<VRInputDevice*> devices;
  VRDataIndex cfg;

  if(file.is_open()) {
    std::stringstream buffer;
    buffer << file.rdbuf();
    xml_string = buffer.rdbuf()->str();
      
      cfg.addDataFromXML(xml_string);
      
      string nspace = "/VRPlugins/";
      std::list<std::string> pluginDirs = cfg.getNames(nspace, false, true);
      for (std::list<std::string>::iterator dir = pluginDirs.begin(); dir != pluginDirs.end(); dir++) {
          cout << *dir << endl;
          string pluginPath = std::string(PLUGINPATH);
          vector<string> plugins;
          bool allPlugins = false;
          std::list<std::string> pluginInfo = cfg.getNames((*dir)+"/", false, false);
          for (std::list<std::string>::iterator info = pluginInfo.begin(); info != pluginInfo.end(); info++) {
              if (*info == "Path")
              {
                  pluginPath = (string)cfg.getValue(*info, (*dir)+"/");
              }
              if (*info == "Names")
              {
                  plugins = cfg.getValue(*info, (*dir)+"/");
                  for (int f = 0; f < plugins.size(); f++)
                  {
                      if (plugins[f] == "*")
                      {
                          allPlugins = true;
                      }
                  }
                  
              }
              
          }
          
          if (allPlugins)
          {
              std::vector<std::string> pluginDirs = listDirectory(pluginPath, true);
              plugins.clear();
              for (int f = 0; f < pluginDirs.size(); f++)
              {
                  if (pluginDirs[f][0] != '.')
                  {
                      plugins.push_back(pluginDirs[f]);
                  }
              }
          }
          
          for (int f = 0; f < plugins.size(); f++)
          {
              //std::cout << plugins[f] << std::endl;
              pluginManager.loadPlugin(pluginPath + "/" + plugins[f], plugins[f] + config);
          }
      }
      
      nspace = "/VRInputDevices/";
      std::list<std::string> deviceNames = cfg.getNames(nspace, false, false);
      for (std::list<std::string>::iterator it = deviceNames.begin(); it != deviceNames.end(); it++)
      {
    	  if (*it != "Comment")
    	  {
              string deviceName = *it;
              string type = cfg.getValue("DeviceType", nspace+deviceName);
              //cout << *it << " " << type << endl;
              cfg.setDefaultNamespace(nspace+deviceName);
              for (int f = 0; f < inputDeviceDrivers.size(); f++)
              {
                  VRInputDevice* device = inputDeviceDrivers[f]->create(type, deviceName, cfg);
                  if (device)
                  {
                      devices.push_back(device);
                  }
              }
              cfg.resetDefaultNamespace();
    	  }
      }
  }

  VRNetServer server((string)cfg.getValue("/Server/Port"), (int)cfg.getValue("/Server/NumClients"));
//  VRNetServer server("3490", 1);

  while(true)
  {
	  std::vector<VREvent> events;

	  for (int f = 0; f < devices.size(); f++)
	  {
		  devices[f]->appendNewInputEventsSinceLastCall(events);
		  //delete devices[f];
		 /* for (std::vector<VREvent>::iterator it=events.begin(); it<events.end(); ++it) {
		        std::cout << it->toXML() << std::endl;
		  }*/
	  }

	  server.synchronizeInputEventsAcrossAllNodes(events);
	  server.synchronizeSwapBuffersAcrossAllNodes();
  }

  for (int f = 0; f < inputDeviceDrivers.size(); f++)
  {
	  delete inputDeviceDrivers[f];
  }

}

std::vector<std::string> listDirectory(
		const std::string& path, bool directories) {
	std::vector<std::string> files;

#if defined(WIN32)
    char search_path[300];
    sprintf(search_path, "%s/*.*", path.c_str());
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(search_path, &fd);
    if(hFind != INVALID_HANDLE_VALUE) {
        do {
            if(directories && (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                files.push_back(fd.cFileName);
            }
            else if ((fd.dwFileAttributes & FILE_ATTRIBUTE_NORMAL))
            {
                files.push_back(fd.cFileName);
            }
        }while(::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }
#else
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (path.c_str())) != NULL) {
	  /* print all the files and directories within directory */
	  while ((ent = readdir (dir)) != NULL) {
		  if (directories && ent->d_type == DT_DIR)
		  {
			  files.push_back(ent->d_name);
		  }
		  else if (ent->d_type == DT_REG)
		  {
			  files.push_back(ent->d_name);
		  }
	  }
	  closedir (dir);
	} else {
	  /* could not open directory */
	  //perror ("");
	}
#endif

	return files;
}
