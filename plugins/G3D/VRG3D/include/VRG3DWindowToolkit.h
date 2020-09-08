/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRG3DWINDOWTOOLKIT_H_
#define VRG3DWINDOWTOOLKIT_H_


#include <vector>

#include <display/VRWindowToolkit.h>
#include <main/VRFactory.h>
#include <main/VRMainInterface.h>
#include <map>
#include <plugin/VRPlugin.h>
#include <G3D/G3D.h>
#include <GLG3D/OSWindow.h>
#include "VRG3DInputDevice.h"


namespace MinVR {
  
  class G3D::OSWindow;
  class G3D::RenderDevice;

  class VRMain;
  class VRG3DInputDevice;
  
  /** Knows how to create windows using the GLFW library for cross-platform
      graphics window support and creates a virtual VRInputDevice to collect
      keyboard and mouse input from these windows.
   */
  class VRG3DWindowToolkit : public VRWindowToolkit {
  public:
    PLUGIN_API VRG3DWindowToolkit(VRMainInterface *vrMain);
    PLUGIN_API virtual ~VRG3DWindowToolkit();

    PLUGIN_API std::string getName() const { return "VR3GDWindowToolkit"; }

    PLUGIN_API virtual int createWindow(VRWindowSettings settings);


    PLUGIN_API void makeWindowCurrent(int windowID);

    PLUGIN_API void destroyWindow(int windowID);

    PLUGIN_API void swapBuffers(int windowID);

    PLUGIN_API static VRWindowToolkit* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);

    PLUGIN_API void getFramebufferSize(int windowID, int& width, int& height);

    PLUGIN_API G3D::OSWindow* getG3DWindow(int windId);

    PLUGIN_API void setG3DRenderDevice(G3D::RenderDevice*);

    PLUGIN_API G3D::RenderDevice*  getG3DRenderDevice( );
   
  private:
    
    G3D::RenderDevice* g3dRenderDevice;

  protected:
    
    

    VRMainInterface *_vrMain;
    
    VRG3DInputDevice *_inputDev;
    G3D::int64                 _frameCounter;

    std::vector<G3DWindow*> _windows;
    
    
    
  private:
    PLUGIN_API int createG3DWindow(const G3DWindow::Settings& settings);
  };





} /* namespace MinVR */

#endif  VRG3DWINDOWTOOLKIT_H_
