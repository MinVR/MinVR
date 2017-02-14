
#include <net/VRNetClient.h>
#include <config/VRDataIndex.h>
#include <math/VRMath.h>
#include <event/VREvent.h>
#include <event/VREventQueue.h>
#include "VRWandMoveEvent.h"
#include <typeinfo>
#include <event/VREventFactory.h>
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


bool exchangeEvent(VREvent ** ev)
{
	if ((*ev)->getName() == "/WandMove")
	{
		*ev = new VRWandMoveEvent(*ev);
		return true;
	}
	return false;
}


int main() {
	VRNetClient client("localhost", "3490");

	VREventQueue queue;
	VREventFactory::getInstance()->RegisterFactoryCallback(exchangeEvent);

	int i = 0;
	while (1) {

		std::cout << "in draw loop " << i << std::endl;

		VRDataQueue::serialData eventData = client.syncEventDataAcrossAllNodes("");
		// Step 1: Unpacking the serialized queue data.
		VRDataQueue *events = new VRDataQueue(eventData);

		// While there is something in the VRDataQueue, unpack it into the VREventQueue using the Factory,
		while (events->notEmpty()) {
			// Get the next item from the queue.

			VREvent *ev = VREventFactory::getInstance()->createVREvent(events->getSerializedObject());
			
			queue.push(ev);

			// Get the next item from the queue.
			events->pop();
		}
		delete events;

		//Check The VREvents and clean them up until the queue is empty
		while (queue.notEmpty()) {
			//get first event
			VREvent *ev = queue.pop();
			
			//process the event for Wandevents
			VRWandMoveEvent * wand_ev = dynamic_cast<VRWandMoveEvent *>(ev);
			if (wand_ev)
			{
				std::cout << "WandMove" << std::endl;
				std::cout << wand_ev->getDescription() << std::endl;
				VRDoubleArray myDA2 = wand_ev->getPosition();
				for (VRDoubleArray::iterator it = myDA2.begin(); it<myDA2.end(); ++it) {
					std::cout << *it <<  " ";
				}
				std::cout << std::endl;
			}
			//otherwise process the event for HeadMove events
			else if (ev->getName() == "/HeadMove")
			{
				std::cout << "HeadMove" << std::endl;
			}

			//delete the event
			delete ev;
		}

		
#ifdef WIN32
		Sleep(2000);
#else
		//sleep(2);
#endif
		client.syncSwapBuffersAcrossAllNodes();
		i++;
	}
}
