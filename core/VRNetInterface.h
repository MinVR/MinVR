#ifndef VRNETINTERFACE_H
#define VRNETINTERFACE_H

#include <vector>
#include "VREvent.h"

class VRNetInterface {
 public:
  virtual void synchronizeInputEventsAcrossAllNodes(std::vector<VREvent> &inputEvents) = 0;
  virtual void synchronizeSwapBuffersAcrossAllNodes() = 0;

 protected:
  // unique identifiers for different network messages sent as a 1-byte header for each msg
  static const unsigned char INPUT_EVENTS_MSG;
  static const unsigned char SWAP_BUFFERS_REQUEST_MSG;
  static const unsigned char SWAP_BUFFERS_NOW_MSG;

  void sendSwapBuffersRequest(int socketID);
  void sendSwapBuffersNow(int socketID);
  void sendInputEvents(int socketID, std::vector<VREvent> &inputEvents);
  int sendall(int s, const unsigned char *buf, int len);

  void waitForAndReceiveMessageHeader(int socketID, unsigned char messageID);
  void waitForAndReceiveSwapBuffersRequest(int socketID);
  void waitForAndReceiveSwapBuffersNow(int socketID);
  void waitForAndReceiveInputEvents(int socketID, std::vector<VREvent> &inputEvents);
  int receiveall(int s, unsigned char *buf, int len);
};

#endif

