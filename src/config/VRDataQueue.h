// -*-c++-*-
#ifndef MINVR_DATAQUEUE_H
#define MINVR_DATAQUEUE_H

//
// Copyright Brown University, 2017.  This software is released under the
// following license: http://opensource.org/licenses/
// Source code originally developed at the Brown University Center for
// Computation and Visualization (ccv.brown.edu).
//
// Code author(s):
//   Tom Sgouros
//

#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <iomanip>

// These are mostly for the iterator.
#include <cassert>      // Provides assert
#include <cstddef>      // Provides ptrdiff_t
#include <iterator>     // iterator
#include <type_traits>  // remove_cv
#include <utility>      // swap


#ifdef WIN32
// winsock2 must be included before windows.h because we are also using it in net
	#include <winsock2.h>
	#include <windows.h>
	#include <ctime>
#else
	#include <sys/time.h>
#endif
#include <stdio.h>
#include <stdexcept>

#include <config/VRDataIndex.h>

namespace MinVR {

/// \brief Contains an item on the VRDataQueue.
///
/// This class contains an item representing an event on a VRDataQueue.  It
/// might be a serialized collection of variables representing a part or an
/// entire VRDataIndex, or it might be a pointer to an entire VRDataIndex,
/// not serialized.  The class exists merely to allow the ambiguity.  The
/// point is to support networked queue operations (where data must be
/// serialized) as well as queue operations within a single process running on
/// a specific machine where performance demands mean we should not serialize
/// the data all the time.
class VRDataQueueItem {
private:
  VRDataIndex* _dataIndex;
  std::string _serialData;

public:
  VRDataQueueItem() : _dataIndex(NULL), _serialData("") {};
  VRDataQueueItem(VRDataIndex* index) :
    _dataIndex(index), _serialData("") {};
  VRDataQueueItem(std::string str) : _dataIndex(NULL), _serialData(str) {};

  std::string serialize() const {
    if (_dataIndex) {
      return _dataIndex->serialize();
    } else {
      return _serialData;
    }
  }

  VRDataIndex getValue() const {
    if (_dataIndex) {
      return *_dataIndex;
    } else {
      return VRDataIndex(_serialData);
    }
  }
};



// template
// <
//     class Type,
//     class UnqualifiedType = std::remove_cv<Type>
// >
// class ForwardIterator
//   : public std::iterator<std::forward_iterator_tag,
//                          UnqualifiedType,
//                          std::ptrdiff_t,
//                          Type*,
//                          Type&> {
//   VRDataPairnode<UnqualifiedType>* itr;

//   explicit ForwardIterator(node<UnqualifiedType>* nd) : itr(nd) {}

// public:
//   /// Default constructor.
//   ///
//   /// Empty argument gives end().
//   ForwardIterator() : itr(nullptr) {}

//   void swap(ForwardIterator& other) noexcept {
//     using std::swap;
//     swap(itr, other.iter);
//   }

//   /// Pre-increment operator.
//   ForwardIterator& operator++ () {
//     assert(itr != nullptr && "Out-of-bounds iterator increment!");
//     itr = itr->next;
//     return *this;
//   }

//   /// Post-increment operator.
//   ForwardIterator operator++ (int) {
//     assert(itr != nullptr && "Out-of-bounds iterator increment!");
//     ForwardIterator tmp(*this);
//     itr = itr->next;
//     return tmp;
//   }



/// \brief A queue of MinVR events.
///
///
/// This object maintains a queue (FIFO) of serialized VRDatum objects,
/// ordered by a timestamp.  Events, which are just small VRDataIndex
/// objects, are pushed onto the queue, examined, and popped off.  You can
/// serialize a queue, preparing it to be sent across a network, and you can
/// also add the contents of one queue to another and have all the events
/// come out in the right time order.
///
/// The system is meant to support network operations -- where the entire
/// queue is sent across a network and therefore must be serialized -- as
/// well as threaded operation, where no network and no serialization are
/// necessary.  Thus the queue is a map between timestamps and objects that
/// can contain either a std::string of serialized data or a pointer to a
/// VRDataIndex object.
///
/// Use this queue for sending data to some other process, or receiving
/// it.  The transmission format is the same XML format as in the
/// VRDataIndex description.
///
class VRDataQueue {
public:
  typedef std::string serialData;

private:

  /// \brief A timestamp for the queue system.
  ///
  /// A timestamp in this system consists of a time value and a
  /// disambiguation value, in case multiple events are recorded at the
  /// same time step.
  typedef std::pair<long long,int> VRTimeStamp;
  typedef std::pair<VRTimeStamp,VRDataQueueItem> VRDataListItem;
  typedef std::map<VRTimeStamp,VRDataQueueItem> VRDataList;
  VRDataList _dataMap;

public:
  VRDataQueue() {};
  VRDataQueue(const serialData serializedQueue);

  static const serialData noData;

  // We want to be able to use iterators.
  typedef VRDataList::iterator iterator;
  typedef VRDataList::const_iterator const_iterator;

  iterator begin() { return _dataMap.begin(); }
  const_iterator begin() const { return _dataMap.begin(); }
  iterator end() { return _dataMap.end(); }
  const_iterator end() const { return _dataMap.end(); }

  /// Process a chunk of XML into queue items and add them to the
  /// existing queue.
  void addSerializedQueue(const serialData serializedQueue);

  /// Add another queue's data to this one.
  void addQueue(const VRDataQueue newQueue);

  /// \brief A boolean to determine whether there is anything in the queue.
  ///
  /// This one is more efficient than returning the negation so that
  /// the user can negate it. There is also an empty() for those who
  /// want something more traditional.
  bool notEmpty() const { return (bool)_dataMap.size(); }

  /// \brief A boolean to determine whether there is anything in the queue.
  ///
  /// A more traditional test.
  bool empty() const { return !(bool)_dataMap.size(); }

  /// \brief Returns the event at the head of the queue, but does not remove
  /// it.
  VRDataIndex getFirst() const;

  /// \brief Return the first item in the queue with its timestamp.
  VRDataListItem getFirstItem() const { return *_dataMap.begin(); };

  /// Removes the object at the front of the queue.
  void pop();

  /// Removes all the objects in the queue.
  void clear();

  /// Makes a timestamp from system facilities.
  long long makeTimeStamp();

  /// Adds an event to the queue.
  void push(const VRDataIndex event);
  /// Adds a serialized event to the queue.
  void push(const serialData eventString);

  /// \brief Adds an event to the queue with a given time stamp.
  ///
  /// Use this method if you want to generate your own time stamp, perhaps
  /// for debugging.  Objects in the queue will be sorted by the time stamp,
  /// and will be popped off the stack in time stamp order.
  void push(const long long timeStamp, const VRDataQueueItem item);

  /// \brief Adds a serialized event to the queue with a given time stamp.
  ///
  /// Use this method if you want to generate your own time stamp, perhaps
  // for debugging.  Objects in the queue will be sorted by the time stamp,
  // and will be popped off the stack in time stamp order.
  void push(const long long timeStamp, const serialData eventString);

  // Serialize the whole queue into a piece of XML.
  serialData serialize();

  // A debug-friendly output function.
  std::string printQueue();

  // How big is the queue?
  int size() { return _dataMap.size(); };

};

} // end namespace MinVR
#endif
