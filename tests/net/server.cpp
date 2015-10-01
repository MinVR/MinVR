
#include <net/VRNetServer.h>


int main() {
  VRNetServer server("3490", 1);
  int i = 0;
  std::vector<VREvent> events;
  while (1) {
    std::cout << "in draw loop " << i << std::endl;
    server.synchronizeInputEventsAcrossAllNodes(events);
    sleep(2);
    server.synchronizeSwapBuffersAcrossAllNodes();
    i++;
  }
}
