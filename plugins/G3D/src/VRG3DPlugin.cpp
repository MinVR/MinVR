/*
 * Copyright Regents of the University of Brown, 2020.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0.
 *
 *
 * Code author(s):
 * 		Dan Camilo Diaz 
 */



 

#include <iostream>
#include <plugin/VRPlugin.h>
#include <display/VRWindowToolkit.h>
#include "VRG3DWindowToolkit.h"

 // special: include this only once in one .cpp file per plugin
#include <plugin/VRPluginVersion.h>

namespace MinVR {

  class VRG3DPlugin : public VRPlugin {
  public:
    PLUGIN_API VRG3DPlugin() {
      //std::cout << "OpenGLPlugin created." << std::endl;
    }
    PLUGIN_API virtual ~VRG3DPlugin() {
      //std::cout << "OpenGLPlugin destroyed." << std::endl;
    }

    PLUGIN_API void registerWithMinVR(VRMainInterface *vrMain)
    {
      //std::cout << "Registering VROpenGLPlugin." << std::endl;
      //vrMain->getFactory()->registerItemType<VRGraphicsToolkit, VROpenGLGraphicsToolkit>("VROpenGLGraphicsToolkit");
      vrMain->getFactory()->registerItemType<VRWindowToolkit, VRG3DWindowToolkit>("VRG3DWindowToolkit");
                                                              
    }

    PLUGIN_API void unregisterWithMinVR(VRMainInterface *vrMain)
    {
      //std::cout << "Unregistering GlfwPlugin." << std::endl;
    // TODO
    }
  };

} // end namespace

extern "C"
{
  PLUGIN_API MinVR::VRPlugin* createPlugin() {
    return new MinVR::VRG3DPlugin();
  }
}

