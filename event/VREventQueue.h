
#ifndef VREVENTQUEUE_H
#define VREVENTQUEUE_H

#include <config/VRDataIndex.h>
#include "VREvent.h"
#include <queue>

/** VREventQueue
 */
class VREventQueue {
public:

  VREventQueue();


  virtual ~VREventQueue();

  void push(VREvent *ev);
  VREvent * VREventQueue::pop();
  bool notEmpty();

protected:
	std::queue <VREvent *> _queue;
};


#endif
