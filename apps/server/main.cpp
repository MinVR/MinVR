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

inline std::string trimWhitespace(const std::string &s) {
  const std::string whitespace = " \t\f\v\n\r";
  int first = s.find_first_not_of(whitespace);
  int last = s.find_last_not_of(whitespace);
  return s.substr(first, (last-first+1));
}

int main(int argc, char **argv) {
  cout << "Registering plugins..." << endl;
  cout << "Plugin path: " << PLUGINPATH << endl;

  App app;

  VRDataIndex di;
  VREvent v("h", di);

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

  if(file.is_open()) {
    std::stringstream buffer;
    buffer << file.rdbuf();
    xml_string = buffer.rdbuf()->str();
      
    VRDataIndex di2;
      di2.addDataFromXML(xml_string);
      std::list<std::string> names = di2.getNames("/Config/Plugins/");
      for (std::list<std::string>::iterator it = names.begin(); it != names.end(); it++) {
          cout << *it << endl;
      }


    std::map<std::string, std::string> props;
    std::string dataIndexXML;
    std::string leftoverInput;

    while (XMLUtils::getXMLField(xml_string, "Plugins", props, dataIndexXML, leftoverInput))
    {
    	std::string pluginPath = std::string(PLUGINPATH);
    	if (props.find("path") != props.end())
    	{
    		pluginPath = props["path"];
    	}

        std::vector<std::string> plugins;
        bool allPlugins = false;

        std::vector<std::string> pluginDirs = listDirectory(pluginPath, true);

    	props = std::map<std::string, std::string>();
    	string innerLeftOverXML;
    	while(XMLUtils::getXMLField(dataIndexXML, "Plugin", props, dataIndexXML, innerLeftOverXML))
    	{
    		std::string plugin = dataIndexXML;
    		plugins.push_back(plugin);
    		if(plugin == "*")
    		{
    			allPlugins = true;
    		}

    		dataIndexXML = innerLeftOverXML;
    	}

        if (allPlugins)
        {
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
        	std::cout << plugins[f] << std::endl;
        	pluginManager.loadPlugin(pluginPath + "/" + plugins[f], plugins[f] + config);
        }

		props = std::map<std::string, std::string>();
		xml_string = leftoverInput;
    }

    props = std::map<std::string, std::string>();
    if (XMLUtils::getXMLField(xml_string, "VRInputDevices", props, dataIndexXML, leftoverInput))
    {
    	props = std::map<std::string, std::string>();
    	while(XMLUtils::getXMLField(dataIndexXML, "VRInputDevice", props, dataIndexXML, leftoverInput))
    	{
    		dataIndexXML = trimWhitespace(dataIndexXML);
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
    		}

    		props = std::map<std::string, std::string>();
    		dataIndexXML = leftoverInput;
    	}
    }
  }

  VRNetServer server("3490", 1);

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
