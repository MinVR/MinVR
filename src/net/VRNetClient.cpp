#include <net/VRNetClient.h>
#include <main/VRLog.h>
#include <main/VRError.h>

#include <iostream>
#include <sstream>

namespace MinVR {

// get sockaddr, IPv4 or IPv6:
void *get_in_addr2(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



VRNetClient::VRNetClient(const std::string &serverIP, const std::string &serverPort)
{
  VRLOG_STATUS("VRNetClient connecting...");

#ifdef WIN32  // WinSock implementation

  WSADATA wsaData;
  SOCKET sockfd = INVALID_SOCKET;
  struct addrinfo hints, *servinfo, *p;
  int rv;

  rv = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (rv != 0) {
    stringstream s;
    s << "WSAStartup failed with error: " << rv;
    VRERROR(s.str(), "Check for a problem with Windows networking.");
    exit(1);
  }

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  if ((rv = getaddrinfo(serverIP.c_str(), serverPort.c_str(), &hints, &servinfo)) != 0) {
    stringstream s;
    s << "getaddrinfo() failed with error: " << rv;
    VRERROR(s.str(), "Check for a problem with Windows networking.");
    WSACleanup();
    exit(1);
  }

  //This is a temporary fix to ensure the client can connect and that the connection is not refused
  p = NULL;
  while(p == NULL){
    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
      if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == INVALID_SOCKET) {
	      stringstream s;
        s << "socket() failed with error " << WSAGetLastError() << "; will retry.";
        VRLOG_STATUS(s.str());
        continue;
      }

      if (connect(sockfd, p->ai_addr, (int)p->ai_addrlen) == SOCKET_ERROR) {
        closesocket(sockfd);
        sockfd = INVALID_SOCKET;
	      stringstream s;
        s << "connect() to server socket failed; will retry.";
        VRLOG_STATUS(s.str());
        continue;
      }

      break;
    }
  }

  if (p == NULL) {
    VRERROR("VRNetClient failed to connect -- exiting.", "Check for a problem with Windows networking.");
    exit(2);
  }

  //inet_ntop(p->ai_family, get_in_addr2((struct sockaddr *)p->ai_addr), s, sizeof s);
  //printf("client: connecting to %s\n", s);
  VRLOG_STATUS("VRNetClient connected.");

  freeaddrinfo(servinfo); // all done with this structure

  // Disable Nagle's algorithm
  char value = 1;
  setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

  _socketFD = sockfd;


#else  // BSD sockets implementation

  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  char problemString[50];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(serverIP.c_str(), serverPort.c_str(), &hints, &servinfo)) != 0) {
    stringstream s;
    s << "getaddrinfo() failed with error: " << rv;
    VRERROR(s.str(), "Check for a problem with networking.");
    exit(1);
  }

  //This is a temporary fix to ensure the client can connect and that the connection is not refused
  p = NULL;
  while(p == NULL){
    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
      if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
        stringstream s;
        s << "socket() failed; will retry.";
        VRLOG_STATUS(s.str());
        continue;
      }

      if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        close(sockfd);
        stringstream s;
        s << "client (pid=" << getpid() << "): connect() to server socket failed; will retry.";
        VRLOG_STATUS(s.str());
        continue;
      }

      break;
    }
  }
  if (p == NULL) {
    VRERROR("VRNetClient failed to connect -- exiting.", "Check for a problem with networking.");
    exit(2);
  }

  inet_ntop(p->ai_family, get_in_addr2((struct sockaddr *)p->ai_addr), s, sizeof(s));
  VRLOG_STATUS("VRNetClient connected to " + std::string(s));

  freeaddrinfo(servinfo); // all done with this structure

  // Disable Nagle's algorithm
  char value = 1;
  setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

  _socketFD = sockfd;

#endif

}

VRNetClient::~VRNetClient()
{
  VRLOG_STATUS("VRNetClient closing socket.");
#ifdef WIN32
  closesocket(_socketFD);
  WSACleanup();
#else
  close(_socketFD);
#endif
}


VRDataQueue VRNetClient::syncEventDataAcrossAllNodes(VRDataQueue eventQueue) {

  // 1. send inputEvents to server
  sendEventData(_socketFD, eventQueue.serialize());

  // 2. receive all events from the server
  VRDataQueue::serialData allEventData = waitForAndReceiveEventData(_socketFD);

  return VRDataQueue(allEventData);
}

void
VRNetClient::syncSwapBuffersAcrossAllNodes()
{
  // 1. send a swap_buffers_request message to the server
  sendSwapBuffersRequest(_socketFD);

  // 2. wait for and receive a swap_buffers_now message from the server
  waitForAndReceiveSwapBuffersNow(_socketFD);
}

} // end namespace MinVR
