#include "../include/VRG3DBaseApp.h"
#include "../include/ConfigVal.H"

namespace MinVR
{
	

  VRG3DBaseApp::VRG3DBaseApp(int argc, char *argv[])
    :VRG3DApp(argc,argv), _clearColor(G3D::Color4(0, 0, 0, 1))
  {
    _log = new G3D::Log("log.txt");

    std::string vrSetupStr = "desktop";
    if (argc >= 2) {
      std::string argStr = std::string(argv[1]);
      if (argStr[0] != '-') {
        vrSetupStr = argStr;
      }
    }

    // initialize the VRApp
    //init(vrSetupStr, _log);

    // Startup ConfigVals
    ConfigValMap::map = new ConfigMap(argc, argv, _log, false);

    // Startup the gfx mgr
    _gfxMgr = new GfxMgr(myRenderDevice, _cameras[0]);
    G3D::Color3 defaultLtCol = G3D::Color3(0.35f, 0.35f, 0.35f);
    _gfxMgr->getLighting()->ambientTop = ConfigVal("AmbientTop", defaultLtCol, false);
    _gfxMgr->getLighting()->ambientBottom = ConfigVal("AmbientBottom", defaultLtCol, false);
    _gfxMgr->getLighting()->lightArray.append(G3D::GLight::directional(G3D::Vector3(0, 1, 1).unit(), G3D::Color3(0.5, 0.5, 0.5)));
    _gfxMgr->loadTexturesFromConfigVal("LoadTextures", _log);

    // Startup the event mgr
    //_eventMgr = new EventMgr(_log);

    // startup a mouse-based standin for 6DOF trackers if we're on a desktop system
    /*if (G3D::beginsWith(vrSetupStr, "desk")) {
      _mouseToTracker = new MouseToTracker(_gfxMgr->getCamera(), ConfigVal("MouseToTracker_NumTrackers", 2, false));
    }*/
  }

  VRG3DBaseApp::~VRG3DBaseApp()
  {

  }

  PLUGIN_API void VRG3DBaseApp::onRenderGraphicsScene(const VRGraphicsState& state)
  {
    _gfxMgr->drawFrame();
    //_gfxMgr->drawStats();
    //vrg3dSleepMsecs(ConfigVal("VRBaseApp_SleepTime", 0.0, false));
  }

}






