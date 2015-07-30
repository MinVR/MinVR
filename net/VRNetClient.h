#ifndef VRNETCLIENT_H
#define VRNETCLIENT_H

#include "VRNetInterface.h"


class VRNetClient : public VRNetInterface {
 public:

  VRNetClient(const std::string &serverIP, const std::string &serverPort);
  virtual ~VRNetClient();

  void synchronizeInputEventsAcrossAllNodes(std::vector<VREvent> &inputEvents);

  void synchronizeSwapBuffersAcrossAllNodes();

 private:

  int _socketFD;

};





#endif
