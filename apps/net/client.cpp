
#include <net/VRNetClient.h>
#include <config/VRDataIndex.h>
#include <math/VRMath.h>
#ifndef WIN32
#include <unistd.h>
#endif

// A little client to illustrate both receiving data from a server and
// unpacking and inspecting it.  Below, the data is received as a
// serialized queue of serial data.  The procedure is like this:
//
//  1. Unpack serialized queue into a VRDataQueue.  The queue comes in
//     time-stamped order, for what it's worth.
//
//  2. Loop through the items in that queue, unpacking them into
//     the data index.
//
//  3. The unpacking (addSerializedValue()) returns the name of the
//     unpacked data, which can be used to get the value and type of
//     the new data.  If it's a container, you can also get the list
//     of its members and then inspect them, too.
//


int main() {
  VRNetClient client("localhost", "3490");

  int i = 0;
  while (1) {
    
    std::cout << "in draw loop " << i << std::endl;
    
    VRDataQueue::serialData eventData = client.syncEventDataAcrossAllNodes("");

    std::cout << eventData << std::endl;
    // Step 1: Unpacking the serialized queue data.
    VRDataQueue *events = new VRDataQueue( eventData ); 

    // Here's the index we'll populate with the new data.
    VRDataIndex *dataIndex = new VRDataIndex;
  
    // While there is something in the queue, unpack it into the index,
    // and examine it.
    while (events->notEmpty()) {

      // Step 2: Unpack the items from the queue.
      std::string p =
        dataIndex->addSerializedValue( events->getSerializedObject() );

      // Step 3: What do we have here?
      std::cout << std::endl << "examining the data..." << std::endl;
      std::cout << "The object named " << p << " is a " <<
        dataIndex->getTypeString(p) << "." << std::endl;
      if (dataIndex->getType(p) == VRCORETYPE_CONTAINER) {
        VRContainer lp = dataIndex->getValue(p);

        std::cout << "... it contains these" << std::endl;

        for (VRContainer::iterator it = lp.begin(); it != lp.end(); it++) {
          std::cout << "  " << *it << " (" << dataIndex->getTypeString(*it) << ")" << std::endl;
        }
      }
    
      // Print out the entire index.
      std::cout << "Index Structure" << std::endl;
      std::cout << dataIndex->printStructure();

      // Get the next item from the queue.
      events->pop();

    }
	delete dataIndex;
	delete events;

    #ifdef WIN32
	  Sleep(2000);
    #else
      //sleep(2);
    #endif
    client.syncSwapBuffersAcrossAllNodes();
    i++;
  }
}
