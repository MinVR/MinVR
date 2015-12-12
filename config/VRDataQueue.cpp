#include "VRDataQueue.h"

// This function does *not* process arbitrary XML, but it *does* process
// XML that was produced by the serialize() method below.
VRDataQueue::VRDataQueue(std::string serializedQueue) {
  std::size_t start, end, start2;

  start = 18;
  end = serializedQueue.find("\"", start);
  
  //std::cout << serializedQueue.substr(start, end - start) << std::endl;
  
  int num;
  std::istringstream( serializedQueue.substr(start,end)) >> num;
  start = serializedQueue.find(">", end);

  start = serializedQueue.find("<VRDataQueueItem>", start);
  while (start != std::string::npos) {

    end = serializedQueue.find("</VRDataQueueItem>", start);
    push( serializedQueue.substr(start + 17, end - start - 17) );
    //std::cout << serializedQueue.substr(start + 17, end - start - 17) << std::endl;
    start = serializedQueue.find("<VRDataQueueItem>", end);
  }
}

std::string VRDataQueue::serialize() {

  std::ostringstream lenStr;
  lenStr << mqueue.size();

  std::string out;

  out = "<VRDataQueue num=\"" + lenStr.str() + "\">";
  for (VRDataList::iterator it = mqueue.begin(); it != mqueue.end(); ++it) {
    out += "<VRDataQueueItem>" + *it + "</VRDataQueueItem>";
  }
  out += "</VRDataQueue>";

  return out;
}

// DEBUG only
void VRDataQueue::printQueue() {

  int i = 0;
  for (VRDataList::iterator it = mqueue.begin(); it != mqueue.end(); ++it) {
    std::cout << "element " << ++i << ": " << *it << std::endl;
  }
}
