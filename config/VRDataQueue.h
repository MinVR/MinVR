// -*-c++-*-
#ifndef MINVR_DATAQUEUE_H
#define MINVR_DATAQUEUE_H

#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <sys/time.h>

// This object maintains a queue (FIFO) of serialized VRDatum objects.
// Serialization turns them into strings, so this is basically just a
// queue of strings.  See the VRDataIndex object for more about these
// objects and their types as part of a system of named data values.
// Over here, we don't care so much about names, as about times of
// creation, and quick assembly and disassembly into serialized data.
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

  typedef std::map<long long,std::string> VRDataList;
  VRDataList dataMap;
  
public:
  VRDataQueue() {};
  VRDataQueue(const std::string serializedQueue);

  void parseSerializedQueue(const std::string serializedQueue);
  
  bool notEmpty() { return (bool)dataMap.size(); }

  // Returns the object at the head of the queue, but does not remove
  // it from the queue.
  std::string getSerializedObject();
  // Removed the object at the front of the queue.
  void pop();

  // Takes a serialized bit of data and pushes it onto the end of the
  // queue.
  void push(const std::string serializedData);
  void push(const long long timeStamp, const std::string serializedData);

  // Serialize the whole queue into a piece of XML.  There is no
  // deserialize method, but there is a constructor that takes a serialized
  // queue as input.
  std::string serialize();

  // A debug-friendly output function.
  void printQueue(); 
  
};

#endif
