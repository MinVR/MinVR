#ifndef VRNETSERVER_H
#define VRNETSERVER_H

#include "VRNetInterface.h"

#ifndef WIN32
  #include <netinet/tcp.h>
  #include <netdb.h>
  #include <arpa/inet.h>
  #include <sys/wait.h>
  #include <signal.h>
#endif

#include <math/VRMath.h>
#include <config/VRDataIndex.h>

namespace MinVR {

class VRNetServer : public VRNetInterface {
 public:

  VRNetServer(const std::string &listenPort, int numExpectedClients);
  ~VRNetServer();

  VRDataQueue syncEventDataAcrossAllNodes(VRDataQueue eventQueue);

  void syncSwapBuffersAcrossAllNodes();

 private:

  std::vector<SOCKET> _clientSocketFDs;

};

}

#endif
