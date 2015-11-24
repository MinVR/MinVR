
#include <net/VRNetServer.h>
#include <math/VRMath.h>
#ifndef WIN32
#include <unistd.h>
#endif

int main() {
  
  if (VRNetInterface::isLittleEndian()) {
    std::cout << "little endian" << std::endl;
  }
  else {
    std::cout << "big endian" << std::endl;
  }
  
  VRNetServer server("3490", 1);
  int i = 0;
  while (1) {
    std::cout << "in draw loop " << i << std::endl;
    std::vector<VREvent> events;
    
    VRMatrix4 m = VRMatrix4::translation(VRVector3(0,1,-10) + VRVector3(0,0,i));
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

