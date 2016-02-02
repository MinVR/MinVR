#include <map>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include "config/VRDataIndex.h"
#include "config/VRDataQueue.h"

// This is just a test program meant to be a vague illustration of the
// use of the VRDataIndex type system.  Also check out the exercz program.

int main() {
  VRDataIndex *index = new VRDataIndex;

  // Create a container object called cora, with two member objects.
  index->addData("/cora/nora", 4);
  index->addData("/cora/flora", "gosh and golly");

  // Create another container called homer, with two members.
  index->addData("/homer/bart", 3.4);
  index->addData("/homer/lisa", 5.2);

  // Show the index structure.
  std::cout << "Index Structure" << std::endl;
  std::cout << index->printStructure();

  VRDataQueue *queue = new VRDataQueue;

  // Push cora onto the queue.
  queue->push(index->serialize("cora", "/"));

  // Change the values of cora's members.
  index->addData("/cora/nora", 7);
  index->addData("/cora/flora", "are you sure?");

  VRIntArray ia;
  ia.push_back(1);
  ia.push_back(2);
  ia.push_back(3);
  index->addData("/cora/ia", ia);

  VRDoubleArray da;
  da.push_back(1.2);
  da.push_back(3.4);
  da.push_back(5.6);
  index->addData("/cora/da", da);

  VRStringArray sa;
  sa.push_back("hello");
  sa.push_back("good-bye");
  index->addData("/cora/sa", sa);
  
  std::cout << "Index Structure after changing /cora" << std::endl;
  std::cout << index->printStructure();

  // Push the new cora onto the queue.
  queue->push(index->serialize("cora", "/"));

  // Push homer onto the queue.
  queue->push(index->serialize("homer", "/"));

  // Show the queue.
  std::cout << "Queue" << std::endl;
  queue->printQueue();

  std::cout << std::endl;
  std::cout << "serialized queue: " << queue->serialize() << std::endl;
  std::string queueData = queue->serialize();
  
  // Pretend we are in a remote program, having received event data
  // over the net.  It comes in as some kind of string data called
  // queueData.
  VRDataQueue *newQueue = new VRDataQueue(queueData);

  // Here's the index we'll populate with the new data.
  VRDataIndex *remoteIndex = new VRDataIndex;
  
  // While there is something in the queue, unpack it into the index,
  // and examine it.
  while (newQueue->notEmpty()) {

    // Unpack the items from the queue.
    std::string p =
      remoteIndex->addSerializedValue( newQueue->getSerializedObject() );

    std::cout << std::endl << "examining the data..." << std::endl;
    std::cout << "The object named " << p << " is a " <<
      remoteIndex->getTypeString(p) << "." << std::endl;
    if (remoteIndex->getType(p) == VRCORETYPE_CONTAINER) {
      VRContainer lp = remoteIndex->getValue(p);

      std::cout << "... it contains these" << std::endl;

      for (VRContainer::iterator it = lp.begin(); it != lp.end(); it++) {
        std::cout << "  " << *it << " (" << remoteIndex->getTypeString(*it) << ")" << std::endl;
      }
    }
    
    // Print out the entire index.
    std::cout << "Remote Index Structure" << std::endl;
    std::cout << remoteIndex->printStructure();

    // Perform arithmetic with some data from the index.
    int r = remoteIndex->getValue("/cora/nora");
    std::cout << " ... sum of /cora/nora and seven = " << 7 + r << std::endl;

    // Get the next item from the queue.
    newQueue->pop();

  }
}
