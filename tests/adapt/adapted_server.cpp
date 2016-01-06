
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

void makeHeadAngleEvent() {
  horizAngle -= incAngle;
  vertAngle -= incAngle;

  if (horizAngle > 6.283185) horizAngle -= 6.283185;
  if (horizAngle < 0.0) horizAngle += 6.283185;

  if (vertAngle > 6.283185) vertAngle -= 6.283185;
  if (vertAngle < 0.0) vertAngle += 6.283185;

  dataIndex->addData("/HeadAngle/vertAngle", vertAngle);
  dataIndex->addData("/HeadAngle/horizAngle", horizAngle);
  dataIndex->addData("/HeadAngle/timestamp", (int)clock());
}


int main(int argc, char**argv) {

  dataIndex = new VRDataIndex;
  
  if (argc > 1)
    dataIndex->processXMLFile(argv[1], "/");
  else
    dataIndex->processXMLFile("tests/adapt/adapted.xml",
                              VRDataIndex::rootNameSpace);


  dataIndex->printStructure();
  
  // VRNetServer server((VRString)dataIndex->getValue("/config/net/serverPort"),
  //                    (VRInt)dataIndex->getValue("/config/net/numClients"));
  VRNetServer server("3490",1);

  VRDataQueue *queue = new VRDataQueue;

  int i = 0;
  while (1) {
    std::cout << "in draw loop " << i << std::endl;

    makeHeadAngleEvent();
    queue->push(dataIndex->serialize("/HeadAngle"));

    std::cout << "EVENTS:" << queue->serialize() << std::endl;

    std::cout << "Index Structure" << std::endl;
    dataIndex->printStructure();

    // Show the queue.
    std::cout << "Queue" << std::endl;
    queue->printQueue();
      
    server.syncEventDataAcrossAllNodes(queue->serialize());
    #ifdef WIN32
	  Sleep(2000);
    #else
	  sleep(2);
    #endif
    server.syncSwapBuffersAcrossAllNodes();
    i++;

    queue->pop();
  }
}

