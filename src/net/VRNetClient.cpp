#include <net/VRNetClient.h>
//#include <math/VRMath.h>

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
	printf("client: connecting...\n");
#ifdef WIN32  // WinSock implementation

  WSADATA wsaData;
  SOCKET sockfd = INVALID_SOCKET;
  struct addrinfo hints, *servinfo, *p;
  int rv;

  rv = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (rv != 0) {
	std::cerr << "WSAStartup failed with error: " << rv << std::endl;
    exit(1);
  }

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  if ((rv = getaddrinfo(serverIP.c_str(), serverPort.c_str(), &hints, &servinfo)) != 0) {
	std::cerr << "getaddrinfo() failed with error: " << rv << std::endl;
    WSACleanup();
    exit(1);
  }

  //This is a temporary fix to ensure the client can connect and that the connection is not refused
  p = NULL;
  while(p == NULL){
    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
      if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == INVALID_SOCKET) {
	    std::cerr << "socket() failed with error: " << WSAGetLastError() << std::endl;
        continue;
      }
    
      if (connect(sockfd, p->ai_addr, (int)p->ai_addrlen) == SOCKET_ERROR) {
        closesocket(sockfd);
        sockfd = INVALID_SOCKET;
	    std::cerr << "connect() to server socket failed" << std::endl;
        continue;
      }
    
      break;
    }
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    //return 2;
    exit(2);
  }
  
  //inet_ntop(p->ai_family, get_in_addr2((struct sockaddr *)p->ai_addr), s, sizeof s);
  //printf("client: connecting to %s\n", s);
  printf("client: connected\n");

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
  
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(serverIP.c_str(), serverPort.c_str(), &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    //return 1;
    exit(1);
  }
  
  //This is a temporary fix to ensure the client can connect and that the connection is not refused
  p = NULL;
  while(p == NULL){
    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
      if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
        perror("client: socket");
        continue;
      }
    
      if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        close(sockfd);
        perror("client: connect");
        continue;
      }
    
      break;
    }
  }
  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    //return 2;
    exit(2);
  }
  
  inet_ntop(p->ai_family, get_in_addr2((struct sockaddr *)p->ai_addr), s, sizeof(s));
  printf("client: connected to %s\n", s);

  freeaddrinfo(servinfo); // all done with this structure

  // Disable Nagle's algorithm
  char value = 1;
  setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

  _socketFD = sockfd;

#endif

}

VRNetClient::~VRNetClient()
{
#ifdef WIN32 
  closesocket(_socketFD);
  WSACleanup();
#else
  close(_socketFD);
#endif
}


VRDataQueue::serialData
VRNetClient::syncEventDataAcrossAllNodes(VRDataQueue::serialData eventData) {
  
  // 1. send inputEvents to server
  sendEventData(_socketFD, eventData);

  // 2. receive all events from the server
  VRDataQueue::serialData allEvents = waitForAndReceiveEventData(_socketFD);
  
  return allEvents;
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
