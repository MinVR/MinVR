#include "VRDataQueue.h"
#include <main/VRError.h>


namespace MinVR {

// Use this when the client has no new data to offer.
const VRDataQueue::serialData VRDataQueue::noData = "";


VRDataQueue::VRDataQueue(const VRDataQueue::serialData serializedQueue) {

  addSerializedQueue(serializedQueue);

}

// This function does *not* process arbitrary XML, but it *does* process XML
// that was produced by the serialize() method below.  This is why it looks a
// bit hacky, with mysterious constants like the number 18.  This
// serialization is only intended to transmit from one instance of this class
// to another, even if it more or less honors the look and feel of XML.
void VRDataQueue::addSerializedQueue(const VRDataQueue::serialData serializedQueue) {

  // Looking for the number in <VRDataQueue num="X">
  if (serializedQueue.size() < 18) return;

  std::size_t start, end;

  start = 18;
  end = serializedQueue.find("\"", start);

  //std::cout << serializedQueue.substr(start, end - start) << std::endl;

  int numIncluded;
  std::istringstream( serializedQueue.substr(start,end)) >> numIncluded;
  start = serializedQueue.find(">", end);

  // Found the number of items.  Now parse the items, making sure to
  // record the timeStamp of each one.

  int numReceived = 0;
  long long timeStamp;
  start = serializedQueue.find("<VRDataQueueItem timeStamp=\"", start);
  while (start != std::string::npos) {

    end = serializedQueue.find("\">", start);
    // These constants have to do with the length of the tags used to
    // encode a queue.
    std::istringstream(serializedQueue.substr(start + 28, end - start - 28)) >>
      timeStamp;

    start = serializedQueue.find("</VRDataQueueItem>", end);
    push(timeStamp, serializedQueue.substr(end + 2, start - end - 2) );
    numReceived++;
    start = serializedQueue.find("<VRDataQueueItem timeStamp=\"", end);
  }

  if (numReceived != numIncluded) {
    VRERRORNOADV("Serialized queue appears corrupted.");
  }
}

void VRDataQueue::addQueue(const VRDataQueue newQueue) {

  if (newQueue.notEmpty()) {

    for (VRDataQueue::const_iterator it = newQueue.begin();
         it != newQueue.end(); it++) {
      // We only want the time value of the timestamp, not the disambiguation
      // value.
      push(it->first.first, it->second);
    }
  }
}

VRDataIndex VRDataQueue::getFirst() const {
  if (_dataMap.empty()) {

    return VRDataIndex();
  } else {

    return _dataMap.begin()->second.getData();
  }
}

void VRDataQueue::pop() {
  _dataMap.erase(_dataMap.begin());
}

void VRDataQueue::clear() {
  _dataMap.clear();
}

// Suggested on Stackoverflow:
//
// inline uint64_t rdtsc() {
//     uint32_t lo, hi;
//     __asm__ __volatile__ (
//       "xorl %%eax, %%eax\n"
//       "cpuid\n"
//       "rdtsc\n"
//       : "=a" (lo), "=d" (hi)
//       :
//       : "%ebx", "%ecx");
//     return (uint64_t)hi << 32 | lo;
// }
//
// http://stackoverflow.com/questions/88/is-gettimeofday-guaranteed-to-be-of-microsecond-resolution
//
// Possibly this is a better way to do the timestamps in the push() method
// below.  There seems to be a resolution issue on some Windows machines that
// makes lots of events have the same time stamp.


long long VRDataQueue::makeTimeStamp() {

#ifdef WIN32
	LARGE_INTEGER t1;

	FILETIME ft;

	// Determine if windows 8 or greater
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	//https://en.wikipedia.org/wiki/List_of_Microsoft_Windows_versions
	if (osvi.dwMajorVersion > 6 || (osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion >= 2)) {
		GetSystemTimePreciseAsFileTime(&ft); // Requires windows 8 and above. Should give microsecond resolution and be consistent across different machines if they are using a time server.
	}
	else {
		GetSystemTimeAsFileTime(&ft);
	}

	
	t1.LowPart = ft.dwLowDateTime;
	t1.HighPart = ft.dwHighDateTime;

	long long timeStamp = t1.QuadPart;
#else

  struct timeval tp;
  gettimeofday(&tp, NULL);

  // Get current timestamp in milliseconds.
  long long timeStamp = (long long) tp.tv_sec * 1000000L + tp.tv_usec;

#endif

  return timeStamp;
}

void VRDataQueue::push(const VRDataQueue::serialData serializedData) {
  push(makeTimeStamp(), serializedData);
}

void VRDataQueue::push(const VRDataIndex event) {
  VRDataIndex* eventPtr = new VRDataIndex(event);
  push(makeTimeStamp(), VRDataQueueItem(eventPtr));
}

void VRDataQueue::push(const long long timeStamp,
                       const VRDataQueue::serialData serializedData) {
  push(timeStamp, VRDataQueueItem(serializedData));
}

void VRDataQueue::push(const long long timeStamp,
                       const VRDataQueueItem queueItem) {

  VRTimeStamp testStamp = VRTimeStamp(timeStamp, 0);
  while (_dataMap.find(testStamp) != _dataMap.end()) {
    testStamp = VRTimeStamp(timeStamp, testStamp.second + 1);
  }

  _dataMap.insert(VRDataListItem(testStamp, queueItem));
}


VRDataQueue::serialData VRDataQueue::serialize() {

  std::ostringstream lenStr;
  lenStr << _dataMap.size();

  VRDataQueue::serialData out;

  out = "<VRDataQueue num=\"" + lenStr.str() + "\">";
  for (VRDataList::iterator it = _dataMap.begin(); it != _dataMap.end(); ++it) {
    std::ostringstream timeStamp;
    timeStamp << it->first.first << "-" << std::setfill('0') << std::setw(3) << it->first.second;
    out += "<VRDataQueueItem timeStamp=\"" + timeStamp.str() + "\">" +
      it->second.serialize() + "</VRDataQueueItem>";
  }
  out += "</VRDataQueue>";

  return out;
}

// DEBUG only
std::string VRDataQueue::printQueue() const {

  std::string out;

  char buf[6];  // No queues more than a million entries long.
  int i = 0;
  for (VRDataList::const_iterator it = _dataMap.begin();
       it != _dataMap.end(); ++it) {
    sprintf(buf, "%d", ++i);
    std::string identifier = "element";
    if (it->second.isSerialized()) {
      identifier += "*";
    } else {
      identifier += " ";
    }
    out += identifier + std::string(buf) + ": " + it->second.serialize() + "\n";
  }

  return out;
}

} // end namespace MinVR
