#include "VREventQueue.h"

VREventQueue::VREventQueue()
{

}

VREventQueue::~VREventQueue()
{
	while (!_queue.empty())
	{
		VREvent * ev = _queue.front();
		_queue.pop();
		delete ev;
	}
}

void VREventQueue::push(VREvent *ev)
{
	_queue.push(ev);
}

VREvent * VREventQueue::pop() {
	VREvent * ev = _queue.front();
	_queue.pop();
	return ev;
}

bool VREventQueue::notEmpty()
{
	return !_queue.empty();
}
