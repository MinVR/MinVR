/*
 * Copyright Regents of the University of Brown, 2020.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0.
 *
 *
 * Code author(s):
 * 		 Camilo Diaz 
 */



 

#include <iostream>
#include <plugin/VRPlugin.h>
#include <display/VRWindowToolkit.h>




namespace MinVR {

  class VRVTKPlugin : public VRPlugin {
  public:
    PLUGIN_API VRVTKPlugin() {
      //std::cout << "OpenGLPlugin created." << std::endl;

    }
    PLUGIN_API virtual ~VRVTKPlugin() {
      //std::cout << "OpenGLPlugin destroyed." << std::endl;
    }

    PLUGIN_API void registerWithMinVR(VRMainInterface *vrMain)
    {
      //std::cout << "Registering VROpenGLPlugin." << std::endl;
      //vrMain->getFactory()->registerItemType<VRGraphicsToolkit, VROpenGLGraphicsToolkit>("VROpenGLGraphicsToolkit");
      /*vrMain->getFactory()->registerItemType<VRWindowToolkit, VRG3DWindowToolkit>("VRG3DWindowToolkit");
      vrMain->getFactory()->registerItemType<VRGraphicsToolkit, VRG3DGraphicsToolkit>("VRG3DGraphicsToolkit");
      vrMain->getFactory()->registerItemType<VRDisplayNode, VRG3DDisplayNode>("VRG3DDisplayNode");*/
                                                              
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
    return new MinVR::VRVTKPlugin();
  }
}

