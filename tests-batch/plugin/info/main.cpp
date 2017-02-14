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

  VRDataIndex di;
  VREvent v("h", di);

  PluginManager pluginManager;
  pluginManager.addInterface(dynamic_cast<VRInputDeviceInterface*>(&app));

  std::vector<std::string> pluginDirs = listDirectory(PLUGINPATH, true);

  string config = "";
#ifdef MinVR_DEBUG
  config = "d";
#endif

  for (int f = 0; f < pluginDirs.size(); f++)
  {
	  if (pluginDirs[f][0] != '.')
	  {
		  cout << pluginDirs[f] << " " << (config == "d" ? "Debug" : "Release") << endl;
		  pluginManager.loadPlugin(string(PLUGINPATH) + "/" + pluginDirs[f], pluginDirs[f] + config);
	  }
  }

  std::string xml_string="";
  std::string fileName = string(PLUGINPATH) + "/MinVR_TUIO/share/vrsetup/tuio-devices.xml";
  ifstream file(fileName.c_str());
  std::cout << "Reading from file = " << fileName << std::endl;

  std::vector<VRInputDevice*> devices;

  if(file.is_open()) {
    std::stringstream buffer;
    buffer << file.rdbuf();
    xml_string = buffer.rdbuf()->str();
    //cout << xml_string << endl;

    std::map<std::string, std::string> props;
    std::string dataIndexXML;
    std::string leftoverInput;
    if (XMLUtils::getXMLField(xml_string, "VRInputDevices", props, dataIndexXML, leftoverInput))
    {
    	props = std::map<std::string, std::string>();
    	while(XMLUtils::getXMLField(dataIndexXML, "VRInputDevice", props, dataIndexXML, leftoverInput))
    	{
    		std::string type = props["type"];
    		std::string name = props["name"];
    		//cout << dataIndexXML << endl;

    		for (int f = 0; f < inputDeviceDrivers.size(); f++)
    		{
    			VRDataIndex di;
    			di.addDataFromXML(dataIndexXML);
    			VRInputDevice* device = inputDeviceDrivers[f]->create(type, name, di);
    			if (device)
    			{
    				devices.push_back(device);
    			}
    			else
    			{
    				std::cout << "Cannot find." << std::endl;
    			}
    		}

    	}
    }
  }

  while(true)
  {
	  for (int f = 0; f < devices.size(); f++)
	  {
		  std::vector<VREvent> events;
		  devices[f]->appendNewInputEventsSinceLastCall(events);
		  //delete devices[f];
		  for (std::vector<VREvent>::iterator it=events.begin(); it<events.end(); ++it) {
		        std::cout << it->toXML() << std::endl;
		  }
	  }
  }

  for (int f = 0; f < inputDeviceDrivers.size(); f++)
  {
	  delete inputDeviceDrivers[f];
  }

}


#if (defined(WIN64) || defined(WIN32) || defined(__APPLE__))
#else
#include <pthread.h>
void *pfun(void *threadid)
{
}

int pfun_call (int argc, char *argv[])
{
   pthread_t threads[1];
   pthread_create(&threads[0], NULL, pfun, (void *)0);
}
#endif

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
