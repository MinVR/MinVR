
#include <net/VRNetServer.h>
#include <math/VRMath.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <ctime>

VRDataIndex *dataIndex;

void makeHeadMoveEvent(double x, double y, double z) {
  dataIndex->addData("/HeadMove/x", x);
  dataIndex->addData("/HeadMove/y", y);
  dataIndex->addData("/HeadMove/z", z);
  dataIndex->addData("/HeadMove/timestamp", (int)clock());
}

void makeWandMoveEvent(double x, double y, double z) {
  VRDoubleArray da;
  da.push_back(1.2);
  da.push_back(3.4);
  da.push_back(5.6);
  dataIndex->addData("/WandMove/position", da);
  dataIndex->addData("/WandMove/description", "Where the wand is now.");
}  

VRDataQueue::serialData makeQueue() {

  VRDataQueue *queue = new VRDataQueue;

  makeHeadMoveEvent(1.2, 2.3, 3.4);
  queue->push(dataIndex->serialize("/HeadMove"));

  makeWandMoveEvent(2.3, 3.4, 4.5);
  queue->push(dataIndex->serialize("/WandMove"));

  makeHeadMoveEvent(4.5, 5.6, 6.7);
  queue->push(dataIndex->serialize("/HeadMove"));

  makeHeadMoveEvent(6.7, 7.8, 8.9);
  queue->push(dataIndex->serialize("/HeadMove"));

  std::cout << "Index Structure" << std::endl;
  std::cout << dataIndex->printStructure();

  // Show the queue.
  std::cout << "Queue" << std::endl;
  std::cout << queue->printQueue();
  
  return queue->serialize();
}

int main() {
  dataIndex = new VRDataIndex();

  if (VRNetInterface::isLittleEndian()) {
    std::cout << "little endian" << std::endl;
  }
  else {
    std::cout << "big endian" << std::endl;
  }
  
  VRNetServer server("3490", 1);
  int i = 0;
  while (1) {
    std::cout << "in draw loop " << i << std::endl;

    VRDataQueue::serialData events = makeQueue();
    std::cout << "EVENTS:" << events << std::endl;

    
    server.syncEventDataAcrossAllNodes(events);
    #ifdef WIN32
	  Sleep(2000);
    #else
	  sleep(2);
    #endif
    server.syncSwapBuffersAcrossAllNodes();
    i++;
  }
}

