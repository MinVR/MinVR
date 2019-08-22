#include <net/VRNetServer.h>

#include <main/VRLog.h>
#include <main/VRError.h>



using namespace std;

namespace MinVR {

#define BACKLOG 100	 // how many pending connections queue will hold

    /**
#ifndef WIN32
void sigchld_handler(int s) {
	while (waitpid(-1, NULL, WNOHANG) > 0);
}
#endif

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
**/


VRNetServer::VRNetServer(const std::string &listenPort, int numExpectedClients)
{

  VRLOG_STATUS("VRNetServer starting networking.");


#ifdef WIN32  // Winsock implementation

    WSADATA wsaData;
    int rv;
    rv = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (rv != 0) {
        stringstream s;
        s << "WSAStartup failed with error: " << rv;
        VRERROR(s.str(), "Check for a problem with Windows networking.");    exit(1);
    }

    const char yes = 1;
    SOCKET serv_fd = INVALID_SOCKET;

    if ((serv_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        VRERROR("VRServer: cannot create a socket", "socket(AF_INET, SOCK_STREAM, 0) failed");
        exit(1);
    }
    
    if (setsockopt(serv_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        VRERROR("VRServer: setsockopt() failed.", "Check for a problem with networking.");
        exit(1);
    }
    
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(stoi(listenPort.c_str()));
    
    if (::bind(serv_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
        VRERROR("VRServer: bind() failed.", "Check for a problem with networking.");
        close(serv_fd);
        exit(1);
    }
    
    if (listen(serv_fd, BACKLOG) == -1) {
        VRERROR("VRServer: listen() failed.", "Check for a problem with networking.");
        exit(1);
    }
    
    VRLOG_STATUS("VRNetServer listening for client connection(s) on port " + listenPort + "...");
    
    socklen_t client_len;
    struct sockaddr_in client_addr;
    SOCKET client_fd;
    
    int numConnected = 0;
    while (numConnected < numExpectedClients) {
        client_len = sizeof(client_addr);
        client_fd = accept(serv_fd, (struct sockaddr *) &client_addr, &client_len);
        if (client_fd == -1) {
            VRERROR("VRServer: accept() failed.", "Check for a problem with networking.");
            exit(1);
        }
        
        // Disable Nagle's algorithm on the client's socket
        setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
        
        numConnected++;
        
        char clientname[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, clientname, sizeof(clientname));
        
        stringstream s;
        s << "Received connection " << numConnected << " of " << numExpectedClients << " from " << clientname;
        VRLOG_STATUS(s.str());
        
        _clientSocketFDs.push_back(client_fd);
    }

    /**
    // listen on sock_fd, new connection on new_fd
    SOCKET serv_fd = INVALID_SOCKET;
    SOCKET new_fd = INVALID_SOCKET;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    const char yes = 1;
    char saddr[INET6_ADDRSTRLEN];
    
    
    
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE; // use my IP

  if ((rv = getaddrinfo(NULL, listenPort.c_str(), &hints, &servinfo)) != 0) {
    stringstream s;
    s << "getaddrinfo() failed with error: " << rv;
    VRERROR(s.str(), "Check for a problem with Windows networking.");
    WSACleanup();
    exit(1);
  }

  // loop through all the results and bind to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((serv_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == INVALID_SOCKET) {
      stringstream s;
      s << "socket() failed with error " << WSAGetLastError() << "; will retry.";
      VRLOG_STATUS(s.str());
      continue;
    }

    if (setsockopt(serv_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == SOCKET_ERROR) {
      stringstream s;
      s << "setsockopt() failed with error " << WSAGetLastError() << ".";
      VRERROR(s.str(), "Check for a problem with Windows networking.");
      closesocket(serv_fd);
      WSACleanup();
      exit(1);
    }

    if (bind(serv_fd, p->ai_addr, (int)p->ai_addrlen) == SOCKET_ERROR) {
      closesocket(serv_fd);
      serv_fd = INVALID_SOCKET;
      stringstream s;
      s << "bind() failed with error " << WSAGetLastError() << "; will retry.";
      VRLOG_STATUS(s.str());
      continue;
    }

    break;
  }

  if (p == NULL) {
    VRERROR("VRNetServer failed to bind -- exiting.", "Check for a problem with Windows networking.");
    exit(2);
  }

  freeaddrinfo(servinfo); // all done with this structure

  if (listen(serv_fd, BACKLOG) == SOCKET_ERROR) {
    stringstream s;
    s << "listen() failed with error " << WSAGetLastError() << ".";
    VRERROR(s.str(), "Check for a problem with Windows networking.");
    closesocket(serv_fd);
    WSACleanup();
    exit(1);
  }

  // Should we do the "reap all dead processes" as in the linux implementation below?

  VRLOG_STATUS("VRNetServer waiting for client connection(s)...");

  int numConnected = 0;
  while (numConnected < numExpectedClients) {
    sin_size = sizeof their_addr;
    new_fd = accept(serv_fd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == INVALID_SOCKET) {
      stringstream s;
      s << "accept() got an INVALID_SOCKET; will retry.";
      VRLOG_STATUS(s.str());
      continue;
    }

    // Disable Nagle's algorithm on the client's socket
    char value = 1;
    setsockopt(new_fd, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

    numConnected++;
    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), saddr, sizeof saddr);
    stringstream s;
    s << "Received connection " << numConnected << " of " << numExpectedClients << " from " << saddr;
    VRLOG_STATUS(s.str());

    _clientSocketFDs.push_back(new_fd);
  }
     */
  VRLOG_STATUS("Established all expected connections.");


#else  // BSD sockets implementation

    int yes=1;
    int serv_fd;
    
    if ((serv_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        VRERROR("VRServer: cannot create a socket", "socket(AF_INET, SOCK_STREAM, 0) failed");
        exit(1);
    }

    if (setsockopt(serv_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        VRERROR("VRServer: setsockopt() failed.", "Check for a problem with networking.");
        exit(1);
    }
    
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(stoi(listenPort.c_str()));
    
    if (::bind(serv_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
        VRERROR("VRServer: bind() failed.", "Check for a problem with networking.");
        close(serv_fd);
        exit(1);
    }

    if (listen(serv_fd, BACKLOG) == -1) {
        VRERROR("VRServer: listen() failed.", "Check for a problem with networking.");
        exit(1);
    }

    VRLOG_STATUS("VRNetServer listening for client connection(s) on port " + listenPort + "...");
    
    socklen_t client_len;
    struct sockaddr_in client_addr;
    int client_fd;
    
    int numConnected = 0;
    while (numConnected < numExpectedClients) {
        client_len = sizeof(client_addr);
        client_fd = accept(serv_fd, (struct sockaddr *) &client_addr, &client_len);
        if (client_fd == -1) {
            VRERROR("VRServer: accept() failed.", "Check for a problem with networking.");
            exit(1);
        }
        
        // Disable Nagle's algorithm on the client's socket
        setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
        
        numConnected++;
        
        char clientname[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, clientname, sizeof(clientname));
        
        stringstream s;
        s << "Received connection " << numConnected << " of " << numExpectedClients << " from " << clientname;
        VRLOG_STATUS(s.str());
        
        _clientSocketFDs.push_back(client_fd);
    }

    VRLOG_STATUS("Established all expected connections.");

    
    /**
    
    int serv_fd, new_fd;  // listen on serv_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char saddr[INET6_ADDRSTRLEN];
    int rv;

    
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP


  if ((rv = getaddrinfo(NULL, listenPort.c_str(), &hints, &servinfo)) != 0) {
    stringstream s;
    s << "getaddrinfo() failed with error: " << gai_strerror(rv);
    VRERROR(s.str(), "Check for a problem with networking.");
    exit(1);
  }

  // loop through all the results and bind to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((serv_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      stringstream s;
      s << "socket() failed; will retry.";
      VRLOG_STATUS(s.str());
      continue;
    }

    if (setsockopt(serv_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      stringstream s;
      s << "setsockopt() failed.";
      VRERROR(s.str(), "Check for a problem with networking.");
      exit(1);
    }

      if (::bind(serv_fd, p->ai_addr, p->ai_addrlen) == -1) {
      close(serv_fd);
      stringstream s;
      s << "bind() failed; will retry.";
      VRLOG_STATUS(s.str());
      continue;
    }

    break;
  }

  if (p == NULL)  {
    VRERROR("VRNetServer failed to bind -- exiting.", "Check for a problem with networking.");
    exit(2);
  }

  freeaddrinfo(servinfo); // all done with this structure

  if (listen(serv_fd, BACKLOG) == -1) {
    stringstream s;
    s << "listen() failed.";
    VRERROR(s.str(), "Check for a problem with networking.");
    exit(1);
  }

  sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    stringstream s;
    s << "sigaction() failed.";
    VRERROR(s.str(), "Check for a problem with networking.");
    exit(1);
  }

  VRLOG_STATUS("VRNetServer waiting for client connection(s)...");

  int numConnected = 0;
  while (numConnected < numExpectedClients) {
    sin_size = sizeof their_addr;
    new_fd = accept(serv_fd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
      stringstream s;
      s << "accept() failed; will retry.";
      VRLOG_STATUS(s.str());
      continue;
    }

    // Disable Nagle's algorithm on the client's socket
    char value = 1;
    setsockopt(new_fd, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

    numConnected++;
    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), saddr, sizeof saddr);

    stringstream s;
    s << "Received connection " << numConnected << " of " << numExpectedClients << " from " << saddr;
    VRLOG_STATUS(s.str());

    _clientSocketFDs.push_back(new_fd);
  }

     **/
     
#endif
}

VRNetServer::~VRNetServer()
{
  VRLOG_STATUS("VRNetServer closing all sockets.");
  for (std::vector<SOCKET>::iterator i=_clientSocketFDs.begin(); i < _clientSocketFDs.end(); i++) {
	#ifdef WIN32
      closesocket(*i);
    #else
      close(*i);
    #endif
  }

#ifdef WIN32
  WSACleanup();
#endif
}


// Wait for and receive an eventData message from every client, add
// them together and send them out again.
VRDataQueue VRNetServer::syncEventDataAcrossAllNodes(VRDataQueue eventQueue) {

  // TODO: rather than a for loop, could use a select() system call
  // here (I think) to figure out which socket is ready for a read in
  // the situation where one client is ready but other(s) are not
  for (std::vector<SOCKET>::iterator itr=_clientSocketFDs.begin();
       itr < _clientSocketFDs.end(); itr++) {
    VRDataQueue::serialData eventData = waitForAndReceiveEventData(*itr);

    eventQueue.addQueue(eventData);
  }

  VRDataQueue::serialData serializedEventQueue = eventQueue.serialize();
  // 2. send new combined inputEvents array out to all clients
  for (std::vector<SOCKET>::iterator itr=_clientSocketFDs.begin();
       itr < _clientSocketFDs.end(); itr++) {
    sendEventData(*itr, serializedEventQueue);
  }

  return eventQueue;
}

void VRNetServer::syncSwapBuffersAcrossAllNodes() {
  // 1. wait for, receive, and parse a swap_buffers_request message
  // from every client

  // TODO: rather than a for loop could use a select() system call
  // here (I think) to figure out which socket is ready for a read in
  // the situation where 1 is ready but other(s) are not
  for (std::vector<SOCKET>::iterator itr=_clientSocketFDs.begin();
       itr < _clientSocketFDs.end(); itr++) {
    waitForAndReceiveSwapBuffersRequest(*itr);
  }

  // 2. send a swap_buffers_now message to every client
  for (std::vector<SOCKET>::iterator itr=_clientSocketFDs.begin();
       itr < _clientSocketFDs.end(); itr++) {
    sendSwapBuffersNow(*itr);
  }
}

} // end namespace MinVR
