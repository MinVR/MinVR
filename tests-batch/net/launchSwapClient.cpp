#include "net/VRNetClient.h"

#ifdef WIN32
#include <process.h>
#define getpid _getpid
#endif

// Program to launch one VRNetClient that connects to server.  Created
// client executes syncSwapBuffersAcrossAllNodes.  The program is
// intended to be executed by a forked child process in the network
// tests.  We do this in a separate program because of limitations of
// execl() that are made less limiting when it simply starts up a new
// binary.
int main(int argc, char* argv[]) {

  MinVR::VRNetClient client = MinVR::VRNetClient("localhost", "3490");

  // Get the client number from the argv list and the process ID.
  std::stringstream clientIDstring;
  clientIDstring << "client " << std::string(argv[1]) << ", pid = " << getpid();

  // Make the swap buffers request.
	std::cout << "launchSwapClient: sync swap buffers request from "
            << clientIDstring.str() << std::endl;

  for (int i = 0; i < 10; i++) {
    client.syncSwapBuffersAcrossAllNodes();
    std::cout << "launchSwapClient: swap request " << i << " received, "
            << clientIDstring.str() << std::endl;
  }

	exit(0);
}
