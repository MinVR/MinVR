
#include <net/VRNetServer.h>
#include <math/VRMath.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <ctime>

VRDataIndex *dataIndex;
  
double vertAngle = 0.0;
double horizAngle = 0.0;
double incAngle = 0.1;
double radius = 15.0;

// This is a simple server for coordinating a bunch of MinVR
// processes.  It reads its configuration from the same config file
// used by the clients.  For practice, it sends the client processes
// two different event types, at differing intervals.

// To run this (and its friends) use the mvr.xml file.  I do it like this:
//
// $ cd MinVR
// $ bin/adapted_server tests/adapt/mvr.xml
//
// Then go over to two other windows and run each of these two commands
// in each of the windows:
//
// $ bin/adapted Display1 tests/adapt/mvr.xml
// $ bin/adapted Display2 tests/adapt/mvr.xml
//
// Enjoy.
//

// Making up two different events.  One rotates the view and the other
// controls a radius.

void makeHeadAngleEvent() {
  horizAngle -= incAngle;
  vertAngle -= incAngle;

  if (horizAngle > 6.283185) horizAngle -= 6.283185;
  if (horizAngle < 0.0) horizAngle += 6.283185;

  if (vertAngle > 6.283185) vertAngle -= 6.283185;
  if (vertAngle < 0.0) vertAngle += 6.283185;

  dataIndex->addData("/HeadAngleEvent/vertAngle", vertAngle);
  dataIndex->addData("/HeadAngleEvent/horizAngle", horizAngle);
  dataIndex->addData("/HeadAngleEvent/timestamp", (int)clock());
}

void makeRadiusEvent() {
  dataIndex->addData("/RadiusEvent/radius", radius);
  dataIndex->addData("/RadiusEvent/timestamp", (int)clock());
 
  radius -= 0.1;
}


int main(int argc, char**argv) {

  dataIndex = new VRDataIndex;

  // This should be more elaborate testing for existence, etc, but so it goes.
  if (argc > 1)
    dataIndex->processXMLFile(argv[1], VRDataIndex::rootNameSpace);

  // Just a check, print the contents of the file.
  dataIndex->printStructure();

  // The config file contains the port to listen on and the number of
  // clients to expect.
  VRNetServer server((VRString)dataIndex->getValue("/MVR/Server/Port"),
                     (VRInt)dataIndex->getValue("/MVR/Server/NumClients"));

  // Here's our empty queue.
  VRDataQueue *queue = new VRDataQueue;

  
  int i = 0;
  while (1) {
    std::cout << "in draw loop " << i << std::endl;

    // Create a head angle event and push it onto the queue.
    makeHeadAngleEvent();
    queue->push(dataIndex->serialize("/HeadAngleEvent"));

    // Every now and then create a radius event and push it onto the queue.
    if ( i % 10 == 0 ) {
      makeRadiusEvent();
      queue->push(dataIndex->serialize("/RadiusEvent"));
    }
    // If you want to see the structure of things, uncomment these.
    //std::cout << "EVENTS:" << queue->serialize() << std::endl;

    //std::cout << "Index Structure" << std::endl;
    //dataIndex->printStructure();

    // Show the queue.
    //std::cout << "Queue" << std::endl;
    //queue->printQueue();

    // Send the event queue and synchronize with all the clients.
    server.syncEventDataAcrossAllNodes(queue->serialize());
    #ifdef WIN32
	  Sleep(200);
    #else
	  usleep(200);
    #endif

    // Send out the swap message.
    server.syncSwapBuffersAcrossAllNodes();
    i++;

    // Empty the event queue.
    queue->clear();
  }
}

