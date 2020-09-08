//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.


// devices (optional)
/*

#include  "ICubeXDevice.h"
#include  "ISenseDirect.h"
#include  "SpaceNavDevice.h"
#include  "TrackDClient.h"
#include  "TUIOEvents.h"
#include  "PQLabsEvents.h"

// Vrpn headers (optional)
#include  "VRPNAnalogDevice.h"
#include  "VRPNButtonDevice.h"
#include  "VRPNTrackerDevice.h"


namespace MinVR {

using namespace G3D;

InputDevice  *InputDevice::newInputDevice( const std::string   name,
                                                      Log     *log,
                                           const ConfigMapRef  map )
{
  std::string type = map->get( name + "_Type", "" );
  InputDevice  *device = NULL;

  if (type == "ICubeX")
  { device = new ICubeXDevice( name, log, map );
  }
  else if (type == "VRPNAnalogDevice")
  { device = new VRPNAnalogDevice( name, log, map );
  }
  else if (type == "VRPNButtonDevice")
  { device = new VRPNButtonDevice( name, log, map );
  }
  else if (type == "VRPNTrackerDevice")
  { device = new VRPNTrackerDevice( name, log, map );
  }
  else if (type == "TrackD")
  { device = new TrackDClient( name, log, map );
  }
  else if (type == "ISense")
  { device = new ISenseDirect( name, log, map );
  }
  else if (type == "TUIO")
  { device = new TUIOEvents( name, log, map );
  }
  else if (type == "PQLabs")
  {
	  PQLabsEvents * pqLabsDevice = new PQLabsEvents( name, log, map );
#ifdef USE_PQLABS
	  pqLabsDevice->init();
#endif
	  device = pqLabsDevice;
  }
  else if (type == "SpaceNavDevice")
  { device = new SpaceNavDevice( name, log );
  }
  else
  { std::string  message = "ERROR:  Unknown device:  type=" + type
                         + "  name=" + name;

    log->println( message );
    cerr << message << endl;

    debugAssert( false );
  }

  return  device;
}



void  InputDevice::unsupportedDevice( const std::string   name,
                                                 Log     *log,
                                      const std::string   variable )
{
  std::string  message = "ERROR:  Support for the " + name
                       + " device was not built into vrg3d.";

  log->println( message );
  cerr << message << endl;

  message = "        Set the " + variable + " variable and rebuild.";
  log->println( message );
  cerr << message << endl;

  assert( false );
}
 

}  // end namespace VRG3D
*/