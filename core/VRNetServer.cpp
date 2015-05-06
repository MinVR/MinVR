#include "VRNetServer.h"
#include "VRMath.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold

void sigchld_handler(int s) {
  while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }  
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



VRNetServer::VRNetServer(const std::string &listenPort, int numExpectedClients) 
{
  int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  struct sigaction sa;
  int yes=1;
  char s[INET6_ADDRSTRLEN];
  int rv;
  
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP
  
  if ((rv = getaddrinfo(NULL, listenPort.c_str(), &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    //return 1;
    exit(1);
  }
  
  // loop through all the results and bind to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }
    
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }
    
    break;
  }
  
  if (p == NULL)  {
    fprintf(stderr, "server: failed to bind\n");
    //return 2;
    exit(2);
  }
  
  freeaddrinfo(servinfo); // all done with this structure
  
  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }
  
  sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }
  
  printf("server: waiting for connections...\n");

  int numConnected = 0;
  while (numConnected < numExpectedClients) {
    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }
    
    numConnected++;
    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
    printf("server: got connection %d from %s\n", numConnected, s);
    
    /*
    waitForAndReceiveSwapBuffersRequest(new_fd);
    
    std::vector<VREvent> events;
    Vec3 v1(1,2,3);
    VREvent e1("event 1", v1);
    for (int i=0;i<50;i++) {
      events.push_back(e1);
    }
    sendInputEvents(new_fd, events);
    */

    //if (send(new_fd, "Hello, world!", 13, 0) == -1) {
    //  perror("send");
    //}
    _clientSocketFDs.push_back(new_fd);
  }
}

VRNetServer::~VRNetServer()
{
  for (std::vector<int>::iterator i=_clientSocketFDs.begin(); i < _clientSocketFDs.end(); i++) {
    close(*i);
  }
}


void 
VRNetServer::synchronizeInputEventsAcrossAllNodes(std::vector<VREvent> &inputEvents) 
{
  // 1. wait for, receive, and parse an inputEvents message from every client, add them to the inputEvents vector
  // TODO: rather than a for loop could use a select() system call here (I think) to figure out which socket is ready for a read in the situation where 1 is ready but other(s) are not
  for (std::vector<int>::iterator itr=_clientSocketFDs.begin(); itr < _clientSocketFDs.end(); itr++) {
    waitForAndReceiveInputEvents(*itr, inputEvents);
  }

  // TODO: rather than just appending the events from each client to a list, could possibly sort these inputEvents based upon a timestep.  necessary???
  
  // 2. send new combined inputEvents array out to all clients
  for (std::vector<int>::iterator itr=_clientSocketFDs.begin(); itr < _clientSocketFDs.end(); itr++) {
    sendInputEvents(*itr, inputEvents);
  }
}

void 
VRNetServer::synchronizeSwapBuffersAcrossAllNodes() 
{
  // 1. wait for, receive, and parse a swap_buffers_request message from every client
  // TODO: rather than a for loop could use a select() system call here (I think) to figure out which socket is ready for a read in the situation where 1 is ready but other(s) are not
  for (std::vector<int>::iterator itr=_clientSocketFDs.begin(); itr < _clientSocketFDs.end(); itr++) {
    waitForAndReceiveSwapBuffersRequest(*itr);
  }
  
  // 2. send a swap_buffers_now message to every client
  for (std::vector<int>::iterator itr=_clientSocketFDs.begin(); itr < _clientSocketFDs.end(); itr++) {
    sendSwapBuffersNow(*itr);
  }
}




int main() {

  /*
  ByteStream bs;
  bs.writeString("hello there");

  std::string s = bs.readString();

  cout << s << endl;


  VREvent e2("event 2", 2);
  e2.serialize(bs);

  VREvent e3;
  e3.deserialize(bs);

  int i = e3.getData().toInt();
  std::cout << "i = " << i << std::endl;


  Vec3 v1(1,2,3);
  v1.print();
  VREvent e1("event 1", v1);

  e1.serialize(bs);


  // To get the data out, we can use the templated deserializeData method:
  VREvent e4;
  e4.deserialize(bs);
  Vec3 v2(e4.getData());
  v2.print();
  */

  VRNetServer server("3490", 1);
  int i = 0;
  std::vector<VREvent> events;
  while (1) {
    std::cout << "in draw loop " << i << std::endl;
    server.synchronizeInputEventsAcrossAllNodes(events);
    sleep(2);
    server.synchronizeSwapBuffersAcrossAllNodes();
    i++;
  }

}
