
#include <net/VRNetServer.h>
#include <math/VRMath.h>
#ifndef WIN32
#include <unistd.h>
#endif

int main() {
  VRNetServer server("3490", 1);
  int i = 0;
  while (1) {
    std::cout << "in draw loop " << i << std::endl;
    std::vector<VREvent> events;
    
    VRMatrix4 m = VRMatrix4::translation(VRVector3(0,0,i));
    VRDataIndex di;
    di.addData("Transform", m);
    VREvent e("Head_Move", di);
    events.push_back(e);
    
    server.synchronizeInputEventsAcrossAllNodes(events);
    #ifdef WIN32
	  Sleep(2000);
    #else
	  sleep(2);
    #endif
    server.synchronizeSwapBuffersAcrossAllNodes();
    i++;
  }
}



/*
TODO:
 
 - cleanup / merge VRCoreTypes.h and VRDatum.h
 
- simulate a head tracker in this server to test sending events across the network
- implement a VRNetClient in Unity
- create some basic support for VRInputDevices, copy VRPN* from VRG3D
- create a very simple, no graphics server
  - VRPN only for input devices
  - some configuration support because will need to know what VRPN devices to create
 
----
 
- Critique the design so far
  - Do we have to include this external XML lib?  Requires redistributing a license, etc.
  - VRDataIndex implementation seems to require a lot of shit, can we simplify somehow?
  - VRCoreTypes seems to work.. but is there too much magic in the addData, getValue calls?
  - Still not totally sold on the idea of serializing to XML rather than Bytes for network transport
  - Could VRDataIndex itself be a VRCoreType, so when you call getData you would either get a core type or a collection of core types.  I guess this is kind of what is happening with VRCollection, but I don't know how to get any data out of a collection.  Seems like this could be recursive, so that if I want to get the data out of a namespace within a dataindex and I ask for everything within that namespace what I get back is a DataIndex.
*/
