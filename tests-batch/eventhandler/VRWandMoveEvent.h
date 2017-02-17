
#ifndef VRWANDMOVEEVENT_H
#define VRWANDMOVEEVENT_H

#include <cstring>
#include <string>
#include <config/VRDataIndex.h>
#include <event/VREvent.h>

/** VREvent
 */
class VRWandMoveEvent : public VREvent {
public:
	VRWandMoveEvent(VREvent * ev);

	VRDoubleArray getPosition();

	VRString getDescription();
};


#endif
