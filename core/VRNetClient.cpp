#include "VRNetClient.h"
#include "VRMath.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



VRNetClient::VRNetClient(const std::string &serverIP, const std::string &serverPort)
{
  int sockfd, numbytes;  
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
  
  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
  printf("client: connecting to %s\n", s);

  freeaddrinfo(servinfo); // all done with this structure

  _socketFD = sockfd;
}

VRNetClient::~VRNetClient()
{
  close(_socketFD);
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


int main() {
  VRNetClient client("localhost", "3490");
  int i = 0;
  std::vector<VREvent> events;
  while (1) {
    std::cout << "in draw loop " << i << std::endl;
    client.synchronizeInputEventsAcrossAllNodes(events);
    sleep(2);
    client.synchronizeSwapBuffersAcrossAllNodes();
    i++;
  }
}
