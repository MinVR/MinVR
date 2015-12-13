#ifndef VRNETCLIENT_H
#define VRNETCLIENT_H

#include "VRNetInterface.h"

#ifndef WIN32
  #include <netinet/tcp.h>
  #include <arpa/inet.h>
#endif



class VRNetClient : public VRNetInterface {
 public:

  VRNetClient(const std::string &serverIP, const std::string &serverPort);
  virtual ~VRNetClient();

  VRDataQueue::serialData
    syncEventDataAcrossAllNodes(VRDataQueue::serialData eventData);

  void syncSwapBuffersAcrossAllNodes();

 private:

  SOCKET _socketFD;

};





#endif
