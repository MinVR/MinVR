#include "net/VRNetClient.h"
#include "config/VRDataIndex.h"

#ifdef WIN32
#include <process.h>
#define getpid _getpid
#endif

// Program to launch one VRNetClient that connects to server.  Created
// client executes syncEventDataAcrossAllNodes.  The program is
// intended to be executed by a forked child process in the network
// tests.  We do this in a separate program because of limitations of
// fork() that are made less limiting when it simply starts up a new
// binary using execl().
int main(int argc, char* argv[]) {

  int clientNumber;
  sscanf(argv[1], "%d", &clientNumber);

  int numberOfSends;
  sscanf(argv[2], "%d", &numberOfSends);

  MinVR::VRNetClient client = MinVR::VRNetClient("localhost", "3490");

  // Get the client number from the argv list and the process ID.
  std::stringstream clientIDstring;
  clientIDstring << "client " << std::string(argv[1]) << ", pid = " << getpid();

  // Make the event request.
	std::cout << "launchEventClient: sync event data request from "
            << clientIDstring.str() << std::endl;

  for (int i = 0; i < numberOfSends; i++) {
    std::cout << clientIDstring.str() << " listening... ("
              << i << ")" << std::endl;

    MinVR::VRDataQueue queue;
    MinVR::VRRawEvent e = MinVR::VRRawEvent("anotherTest");
    e.addData("client", clientNumber);
    queue.push(e);
    std::cout << "*******" << e << std::endl;
    queue = client.syncEventDataAcrossAllNodes(queue);

    std::cout << "queue name: " << queue.getFirst().getName() << std::endl;

    std::cout << "launchEventClient: event " << i+1 << " received by "
              << clientIDstring.str() << ": " << queue.getFirst() << std::endl;
  }

  std::cout << "Process " << getpid() << " exiting normally." << std::endl;
	exit(0);
}
