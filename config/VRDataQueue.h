// -*-c++-*-
#ifndef MINVR_DATAQUEUE_H
#define MINVR_DATAQUEUE_H

#include <deque>

// This object maintains a queue (FILO) of serialized VRDatum objects.
// See the VRDataIndex object for more about these objects and
// their types as part of a system of named data values.  Over here,
// we don't care so much about names, as about times of creation, and
// quick assembly and disassembly into serialized data.
//
// Use this queue for sending data to some other process, or receiving
// it.  The transmission format is the same XML format as in the
// VRDataIndex description.
//
// Tom Sgouros 10/16/2015
//
//
class VRDataQueue {
private:

  typedef std::deque<std::string> MVRDataList;
  MVRDataList mqueue;

public:
  VRDataQueue() {};

  bool notEmpty() { return (bool)mqueue.size(); }

  // Returns a pointer to the object at the head of the queue.
  std::string getSerializedDatum() { return mqueue.front(); }

  // Takes a serialized bit of data and pushes it onto the end of the
  // queue.
  void push(const std::string serializedData) {
    mqueue.push_back(serializedData);
  }

  // Removes the value at the front of the queue.
  void pop() { mqueue.pop_front(); }

  // DEBUG only
  void printQueue() {

    int i = 0;
    for (MVRDataList::iterator it = mqueue.begin(); it != mqueue.end(); ++it) {
      std::cout << "element " << ++i << ": " << *it << std::endl;
    }
  }

};

#endif
