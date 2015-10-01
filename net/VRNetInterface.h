#ifndef VRNETINTERFACE_H
#define VRNETINTERFACE_H

#include <vector>
#include <event/VREvent.h>

#ifdef WINDOWS
  #include <winsock2.h>
  #include <windows.h>
#else
  #define SOCKET int
#endif


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
  int sendall(SOCKET s, const unsigned char *buf, int len);

  void waitForAndReceiveMessageHeader(SOCKET socketID, unsigned char messageID);
  void waitForAndReceiveSwapBuffersRequest(SOCKET socketID);
  void waitForAndReceiveSwapBuffersNow(SOCKET socketID);
  void waitForAndReceiveInputEvents(SOCKET socketID, std::vector<VREvent> &inputEvents);
  int receiveall(SOCKET s, unsigned char *buf, int len);
};

#endif

