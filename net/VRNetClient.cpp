#include <net/VRNetClient.h>
#include <math/VRMath.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef WINDOWS
  #include <ws2tcpip.h>
  #pragma comment (lib, "Ws2_32.lib")
  #pragma comment (lib, "Mswsock.lib")
  #pragma comment (lib, "AdvApi32.lib")
#else
  #include <unistd.h>
  #include <errno.h>
  #include <string.h>
  #include <netdb.h>
  #include <sys/types.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>
  #include <sys/socket.h>
  #include <arpa/inet.h>
#endif


// get sockaddr, IPv4 or IPv6:
void *get_in_addr2(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



VRNetClient::VRNetClient(const std::string &serverIP, const std::string &serverPort)
{

#ifdef WINDOWS  // WinSock implementation

  WSADATA wsaData;
  SOCKET sockfd = INVALID_SOCKET;
  struct addrinfo hints, *servinfo, *p;
  int rv;

  rv = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (rv != 0) {
    cerr << "WSAStartup failed with error: " << rv << endl;
    exit(1);
  }

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  if ((rv = getaddrinfo(serverIP.c_str(), serverPort.c_str(), &hints, &servinfo)) != 0) {
    cerr << "getaddrinfo() failed with error: " << rv << endl;
    WSACleanup();
    exit(1);
  }

  // loop through all the results and connect to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == INVALID_SOCKET) {
      cerr << "socket() failed with error: " << WSAGetLastError() << endl;
      continue;
    }
    
    if (connect(sockfd, p->ai_addr, (int)p->ai_addrlen) == SOCKET_ERROR) {
      closesocket(sockfd);
      sockfd = INVALID_SOCKET;
      cerr << "connect() to server socket failed" << endl;
      continue;
    }
    
    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    //return 2;
    exit(2);
  }
  
  inet_ntop(p->ai_family, get_in_addr2((struct sockaddr *)p->ai_addr), s, sizeof s);
  printf("client: connecting to %s\n", s);

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

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    //return 2;
    exit(2);
  }
  
  inet_ntop(p->ai_family, get_in_addr2((struct sockaddr *)p->ai_addr), s, sizeof s);
  printf("client: connecting to %s\n", s);

  freeaddrinfo(servinfo); // all done with this structure

  // Disable Nagle's algorithm
  char value = 1;
  setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

  _socketFD = sockfd;

#endif
}

VRNetClient::~VRNetClient()
{
#ifdef WINDOWS 
  closesocket(_socketFD);
  WSACleanup();
#else
  close(_socketFD);
#endif
}



void 
VRNetClient::synchronizeInputEventsAcrossAllNodes(std::vector<VREvent> &inputEvents) 
{
  // 1. send inputEvents to server
  sendInputEvents(_socketFD, inputEvents);

  // 2. receive and parse serverInputEvents
  std::vector<VREvent> serverInputEvents;
  waitForAndReceiveInputEvents(_socketFD, serverInputEvents);
  
  // 3. inputEvents = serverInputEvents
  inputEvents = serverInputEvents;
}

void 
VRNetClient::synchronizeSwapBuffersAcrossAllNodes() 
{
  // 1. send a swap_buffers_request message to the server
  sendSwapBuffersRequest(_socketFD);

  // 2. wait for and receive a swap_buffers_now message from the server
  waitForAndReceiveSwapBuffersNow(_socketFD);
}
