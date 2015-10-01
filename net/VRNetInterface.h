#ifndef VRNETINTERFACE_H
#define VRNETINTERFACE_H

#include <vector>
#include <event/VREvent.h>

#ifdef WIN32
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

  static void sendSwapBuffersRequest(SOCKET socketID);
  static void sendSwapBuffersNow(SOCKET socketID);
  static void sendInputEvents(SOCKET socketID, std::vector<VREvent> &inputEvents);
  static int sendall(SOCKET s, const unsigned char *buf, int len);

  static void waitForAndReceiveMessageHeader(SOCKET socketID, unsigned char messageID);
  static void waitForAndReceiveSwapBuffersRequest(SOCKET socketID);
  static void waitForAndReceiveSwapBuffersNow(SOCKET socketID);
  static void waitForAndReceiveInputEvents(SOCKET socketID, std::vector<VREvent> &inputEvents);
  static int receiveall(SOCKET s, unsigned char *buf, int len);
};

#endif

