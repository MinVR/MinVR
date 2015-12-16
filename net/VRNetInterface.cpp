#include <net/VRNetInterface.h>

// unique identifiers for different network messages
const unsigned char VRNetInterface::EVENTS_MSG = 1;
const unsigned char VRNetInterface::SWAP_BUFFERS_REQUEST_MSG = 2;
const unsigned char VRNetInterface::SWAP_BUFFERS_NOW_MSG = 3;

// assuming 32-bit ints, note that VRNetInterface::pack/unpackint()
// use the int32_t type
const unsigned char VRNetInterface::VRNET_SIZEOFINT = 4;


void VRNetInterface::sendSwapBuffersRequest(SOCKET socketID) {
  // this message consists only of a 1-byte header
  sendall(socketID, &SWAP_BUFFERS_REQUEST_MSG, 1);
}

void VRNetInterface::sendSwapBuffersNow(SOCKET socketID) {
  // this message consists only of a 1-byte header
  sendall(socketID, &SWAP_BUFFERS_NOW_MSG, 1);
}

void VRNetInterface::sendEventData(SOCKET socketID,
                                   VRDataQueue::serialData eventData) {    
  // 1. send 1-byte message header
  sendall(socketID, &EVENTS_MSG, 1);
  
  // 2. send the size of the message data so receive will know how
  // many bytes to expect.
  unsigned char buf[VRNET_SIZEOFINT];
  packInt(buf, eventData.size());
  sendall(socketID, buf, VRNET_SIZEOFINT);
  
  // 3. send the chars that make up the eventData.
  sendall(socketID,
          (const unsigned char*)eventData.c_str(),
          eventData.size());
}

int VRNetInterface::sendall(SOCKET s, const unsigned char *buf, int len) {
  int total = 0;        // how many bytes we've sent
  int bytesleft = len;  // how many we have left to send
  int n;    
  while (total < len) {
#ifdef WIN32
	  n = send(s, (char *)(buf + total), bytesleft, 0);
#else
	  n = send(s, buf + total, bytesleft, 0);
#endif
    if (n == -1) { break; }
    total += n;
    bytesleft -= n;
  }
  return n==-1?-1:total; // return -1 on failure, total on success
} 

// Meant to receive a message of a single byte.
void VRNetInterface::waitForAndReceiveOneByte(SOCKET socketID,
                                              unsigned char messageID) {
  unsigned char receivedID = 0x0;
  while (receivedID != messageID) {
    int status = receiveall(socketID, &receivedID, 1);
    if (status == -1) {
      std::cerr << "NetInterface error: receiveall failed." << std::endl;
      exit(1);
    }
    else if ((status == 1) && (receivedID != messageID)) {
      std::cerr << "NetInterface error, unexpected message.  Expected: " <<
        (int)messageID << " Received: " << (int)receivedID << std::endl;
    }
  }
}

void 
VRNetInterface::waitForAndReceiveSwapBuffersRequest(SOCKET socketID) {
  // this message consists only of a 1-byte header
  waitForAndReceiveOneByte(socketID, SWAP_BUFFERS_REQUEST_MSG);
}

void 
VRNetInterface::waitForAndReceiveSwapBuffersNow(SOCKET socketID) {
  // this message consists only of a 1-byte header
  waitForAndReceiveOneByte(socketID, SWAP_BUFFERS_NOW_MSG);
}


VRDataQueue::serialData
VRNetInterface::waitForAndReceiveEventData(SOCKET socketID) {
  // 1. receive 1-byte message header
  waitForAndReceiveOneByte(socketID, EVENTS_MSG);
  
  // 2. receive int that tells us the size of the data portion of the
  // message in bytes
  unsigned char buf1[VRNET_SIZEOFINT];
  int status = receiveall(socketID, buf1, VRNET_SIZEOFINT);
  if (status == -1) {
    std::cerr << "NetInterface error: receiveall failed." << std::endl;
    exit(1);
  }
  int dataSize = unpackInt(buf1);
  
  //std::cout << "dataSize = " << dataSize << std::endl;
  
  // 3. receive dataSize bytes, then decode these as InputEvents
  unsigned char *buf2 = new unsigned char[dataSize+1];
  status = receiveall(socketID, buf2, dataSize);
  if ((status == -1) || (status != dataSize)) {
    std::cerr << "NetInterface error: receiveall failed." << std::endl;
    exit(1);
  }
  
  buf2[dataSize] = '\0';
  return VRDataQueue::serialData(reinterpret_cast<const char *>(buf2));
}

int VRNetInterface::receiveall(SOCKET s, unsigned char *buf, int len) {
  int total = 0;        // how many bytes we've received
  int bytesleft = len; // how many we have left to receive
  int n;    
  while (total < len) {
#ifdef WIN32
	  n = recv(s, (char *)(buf + total), bytesleft, 0);
#else
	  n = recv(s, buf + total, bytesleft, 0);
#endif
    if (n == -1) { break; }
    total += n;
    bytesleft -= n;
  }
  return n==-1?-1:total; // return -1 on failure, total on success
}
