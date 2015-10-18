// -*-c++-*-
#ifndef MINVR_DATAQUEUE_H
#define MINVR_DATAQUEUE_H

#include <deque>

// This object maintains a queue (FILO) of serialized MinVRDatum objects.
// See the MinVRDataIndex object for more about these objects and
// their types as part of a system of named data values.  Over here,
// we don't care so much about names, as about times of creation, and
// quick assembly and disassembly into serialized data.
//
// Use this queue for sending data to some other process, or receiving
// it.  The transmission format is the same XML format as in the
// MinVRDataIndex description.
//
// Tom Sgouros 10/16/2015
//
// To use:
//
// Create a queue object, and add values to the store in one of three ways.
//
//  1. You can add data with the specialized add functions, such as
//     addValueInt(), addValueFloat(), etc.  These take a name and a
//     value and park them in the queue.
//
//  2. You can feed some serialized data to one of the addValue()
//     functions.  This is just string data, that looks something like
//     this:
//
//     <flora type="int">42</flora>
//
//     <bob type="container">
//        <flora type="int">42</flora>
//        <pi type="float">3.1415926</pi>
//     </bob>
//
//     The type attributes are usually superfluous, since the code can
//     usually infer the type from the value string.  However, future
//     implemented data types might make the inference step less
//     reliable, so bear that in mind.
//
//  3. Feed a file containing XML into processXMLFile().
//
//  Once a queue has entries, they can be retrieved at your pleasure
//  with getDatum() or serialize().  The getDatum() method returns the
//  next MinVRDatum object in the queue, so can be used directly in
//  your program.  I hate remembering how to spell the static_cast<>()
//  options, so these are provided as a convenience via a helper class
//  methods to the pointer objects.  So (int)p->getDatum() gets you an
//  integer and (double)p->getDatum() gets you a double, so long as
//  the relevant objects actually contain an integer and double.
//
//  There are no other ways to get a datum value.  Your interest
//  should be in loading MinVRDatum objects into the queue, and
//  getting them off the other end.
//
//
class MinVRDataQueue {
private:

  typedef std::deque<std::string> MVRDataList;
  MVRDataList mqueue;

public:
  MinVRDataQueue() {};

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
