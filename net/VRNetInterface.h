#ifndef VRNETINTERFACE_H
#define VRNETINTERFACE_H

#include <vector>
#include <event/VREvent.h>

#ifdef WIN32
  #include <winsock2.h>
  #include <windows.h>
	#include <stdint.h>
	//typedef int int32_t;
	//typedef unsigned int uint32_t;
#else
  #define SOCKET int
  #include "stdint.h"
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
  
  static const unsigned char VRNET_SIZEOFINT;

  static void sendSwapBuffersRequest(SOCKET socketID);
  static void sendSwapBuffersNow(SOCKET socketID);
  static void sendInputEvents(SOCKET socketID, std::vector<VREvent> &inputEvents);
  static int sendall(SOCKET s, const unsigned char *buf, int len);

  static void waitForAndReceiveMessageHeader(SOCKET socketID, unsigned char messageID);
  static void waitForAndReceiveSwapBuffersRequest(SOCKET socketID);
  static void waitForAndReceiveSwapBuffersNow(SOCKET socketID);
  static void waitForAndReceiveInputEvents(SOCKET socketID, std::vector<VREvent> &inputEvents);
  static int receiveall(SOCKET s, unsigned char *buf, int len);
  
  
public:
  /// return 0 for big endian, 1 for little endian.
  /// http://stackoverflow.com/questions/12791864/c-program-to-check-little-vs-big-endian
  static bool isLittleEndian() {
    volatile uint32_t i=0x01234567;
    return (*((uint8_t*)(&i))) == 0x67;
  }
  
  static void packInt(unsigned char *bytePtr, int32_t toPack) {
    unsigned char *p = (unsigned char *) &toPack;
    for (int i=0;i<VRNET_SIZEOFINT;i++) {
      int index = i;
      if (!isLittleEndian()) {
        index = VRNET_SIZEOFINT - i - 1;
      }
      bytePtr[i] = p[index];
    }
  }
  
  static int32_t unpackInt(unsigned char *bytePtr) {
    int toReturn = 0;
    unsigned char *p = (unsigned char *) &toReturn;
    for (int i=0;i<VRNET_SIZEOFINT;i++) {
      int index = i;
      if (!isLittleEndian()) {
        index = VRNET_SIZEOFINT - i - 1;
      }
      p[i] = bytePtr[index];
    }
    return toReturn;
  }

};

#endif

