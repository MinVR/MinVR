#ifndef VRNETSERVER_H
#define VRNETSERVER_H

#include "VRNetInterface.h"


class VRNetServer : public VRNetInterface {
 public:

  VRNetServer(const std::string &listenPort, int numExpectedClients);
  virtual ~VRNetServer();

  void synchronizeInputEventsAcrossAllNodes(std::vector<VREvent> &inputEvents);

  void synchronizeSwapBuffersAcrossAllNodes();

 private:

  std::vector<int> _clientSocketFDs;

};



#endif
