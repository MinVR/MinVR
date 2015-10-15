
#include <net/VRNetServer.h>


int main() {
  VRNetServer server("3490", 1);
  int i = 0;
  std::vector<VREvent> events;
  while (1) {
    std::cout << "in draw loop " << i << std::endl;
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
