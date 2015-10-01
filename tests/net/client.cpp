
#include <net/VRNetClient.h>


int main() {
  VRNetClient client("localhost", "3490");
  int i = 0;
  std::vector<VREvent> events;
  while (1) {
    std::cout << "in draw loop " << i << std::endl;
    client.synchronizeInputEventsAcrossAllNodes(events);
    sleep(2);
    client.synchronizeSwapBuffersAcrossAllNodes();
    i++;
  }
}