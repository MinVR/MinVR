
#include <net/VRNetClient.h>
#ifndef WIN32
#include <unistd.h>
#endif

int main() {
  VRNetClient client("localhost", "3490");
  int i = 0;
  std::vector<VREvent> events;
  while (1) {
    std::cout << "in draw loop " << i << std::endl;
    client.synchronizeInputEventsAcrossAllNodes(events);
    #ifdef WIN32
	  Sleep(2000);
    #else
      sleep(2);
    #endif
	client.synchronizeSwapBuffersAcrossAllNodes();
    i++;
  }
}
