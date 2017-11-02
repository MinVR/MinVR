#ifndef VRNETINTERFACE_H
#define VRNETINTERFACE_H

#include <config/VRDataIndex.h>
#include <config/VRDataQueue.h>

#include <vector>
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#include <stdint.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#else
#define SOCKET int
#include "stdint.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

namespace MinVR {

class VRNetInterface {
public:
  virtual VRDataQueue syncEventDataAcrossAllNodes(VRDataQueue eventQueue) = 0;
	virtual void syncSwapBuffersAcrossAllNodes() = 0;

	virtual ~VRNetInterface() {};
protected:
	// unique identifiers for different network messages sent as a
	// 1-byte header for each msg
	static const unsigned char EVENTS_MSG;
	static const unsigned char SWAP_BUFFERS_REQUEST_MSG;
	static const unsigned char SWAP_BUFFERS_NOW_MSG;

	static const unsigned char VRNET_SIZEOFINT;

	static void sendSwapBuffersRequest(SOCKET socketID);
	static void sendSwapBuffersNow(SOCKET socketID);
	static void sendEventData(SOCKET socketID, VRDataQueue::serialData eventData);
	static int sendall(SOCKET socketID, const unsigned char *buf, int len);

	static void waitForAndReceiveOneByte(SOCKET socketID,
		unsigned char messageID);
	static void waitForAndReceiveSwapBuffersRequest(SOCKET socketID);
	static void waitForAndReceiveSwapBuffersNow(SOCKET socketID);
	static VRDataQueue::serialData waitForAndReceiveEventData(SOCKET socketID);
	static int receiveall(SOCKET socketID, unsigned char *buf, int len);


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

}

#endif

