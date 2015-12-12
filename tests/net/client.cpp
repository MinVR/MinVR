
#include <net/VRNetClient.h>
#include <config/VRDataIndex.h>
#include <math/VRMath.h>
#ifndef WIN32
#include <unistd.h>
#endif

int main() {
  VRDataIndex *index = new VRDataIndex;

  
  VRNetClient client("localhost", "3490");

  int i = 0;
  while (1) {
    
    std::cout << "in draw loop " << i << std::endl;
    
    client.synchronizeInputEventsAcrossAllNodes(events);
    VRDataQueue *events = new VRDataQueue( eventData ); 

    while (events->notEmpty()) {

      std::string p = index->addSerializedValue(events->getSerializeObject());

      // examine the data

      std::cout << p->getType() << std::endl;
      if (p->getType() == VCORETYPE_CONTAINER) 
      
    
    for (std::vector<VREvent>::iterator it=events.begin(); it<events.end(); ++it) {
      std::cout << it->toXML() << std::endl;
    }
    
    #ifdef WIN32
	  Sleep(2000);
    #else
      //sleep(2);
    #endif
	client.synchronizeSwapBuffersAcrossAllNodes();
    i++;
  }
}
