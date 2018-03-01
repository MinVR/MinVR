#ifndef VRMAININTERFACE_H
#define VRMAININTERFACE_H

#include <display/VRGraphicsToolkit.h>
#include <display/VRWindowToolkit.h>
#include <input/VRInputDevice.h>
#include <main/VREventHandler.h>
#include <main/VRRenderHandler.h>
#include <main/VRModelHandler.h>
#include <main/VRError.h>



namespace MinVR {

class VRFactory;

/** The hooks needed for plugins to communicate with VRMain are defined
    here in an abstract base class that should have only minimal changes
    as MinVR evolves.  This is to avoid versioning issues where even a
    small change in the implementation of VRMain or VRFactory might
    force all the plugins to need to be recompiled.
*/
class VRMainInterface {
public:
  virtual void addEventHandler(VREventHandler *eHandler) = 0;
  virtual void addRenderHandler(VRRenderHandler *rHandler) = 0;
  virtual void addModelHandler(VRModelHandler* modelHandler) = 0;
  virtual void addInputDevice(VRInputDevice *dev) = 0;
  virtual VRDataIndex* getConfig() = 0;
  virtual VRGraphicsToolkit* getGraphicsToolkit(const std::string &name) = 0;
  virtual VRWindowToolkit* getWindowToolkit(const std::string &name) = 0;
  virtual VRFactory* getFactory() = 0;
  virtual void addPluginSearchPath(const std::string& path) = 0;
  virtual int getArgc() = 0;
  virtual char** getArgv() = 0;
};


} // end namespace

#endif
