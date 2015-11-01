#include <map>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include "VRDataIndex.h"
#include "VRDataQueue.h"


// This is just a test program meant to be a vague illustration of the
// use of the VRDataIndex type system.  Also check out the exercz program.

int main() {
  VRDataIndex *index = new VRDataIndex;

  // Create a container object called cora, with two member objects.
  index->addValue("/cora/nora", 4);
  index->addValue("/cora/flora", "gosh and golly");

  // Create another container called homer, with two members.
  index->addValue("/homer/bart", 3.4);
  index->addValue("/homer/lisa", 5.2);

  // Show the index structure.
  std::cout << "Index Structure" << std::endl;
  index->printStructure();

  VRDataQueue *queue = new VRDataQueue;

  // Push cora onto the queue.
  queue->push(index->serialize("cora", "/"));

  // Change the values of cora's members.
  index->addValue("/cora/nora", 7);
  index->addValue("/cora/flora", "are you sure?");

  std::cout << "Index Structure after changing /cora" << std::endl;
  index->printStructure();

  // Push the new cora onto the queue.
  queue->push(index->serialize("cora", "/"));

  // Push homer onto the queue.
  queue->push(index->serialize("homer", "/"));

  // Show the queue.
  std::cout << "Queue" << std::endl;
  queue->printQueue();

  // Pretend we are in a remote program, having received event data over the net.
  VRDataIndex *remoteIndex = new VRDataIndex;

  // While there is something in the queue, unpack it into the index, and examine.
  while (queue->notEmpty()) {

    remoteIndex->addSerializedValue( queue->getSerializedDatum() );

    std::cout << "Remote Index Structure" << std::endl;
    remoteIndex->printStructure();

    int r = remoteIndex->getValue("/cora/nora");
    std::cout << " ... sum of /cora/nora and seven = " << 7 + r << std::endl;

    queue->pop();

  }
}
