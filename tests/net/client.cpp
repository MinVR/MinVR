
#include <net/VRNetClient.h>
#include <event/VREvent.h>
#include <math/VRMath.h>
#ifndef WIN32
#include <unistd.h>
#endif

int main() {
  VRNetClient client("localhost", "3490");
  int i = 0;
  while (1) {
    std::cout << "in draw loop " << i << std::endl;
    
    std::vector<VREvent> events;
    client.synchronizeInputEventsAcrossAllNodes(events);
    for (std::vector<VREvent>::iterator it=events.begin(); it<events.end(); ++it) {
      std::cout << it->toXML() << std::endl;
    }
    
    #ifdef WIN32
	  Sleep(2000);
    #else
      sleep(2);
    #endif
	client.synchronizeSwapBuffersAcrossAllNodes();
    i++;
  }
}