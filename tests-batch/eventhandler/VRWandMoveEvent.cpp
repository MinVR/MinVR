#include "VRWandMoveEvent.h"

VRWandMoveEvent::VRWandMoveEvent(VREvent * ev) : VREvent()
{
	_name = ev->getName();
	_dataIndex = (*ev->getDataIndex());
	delete ev;
}

VRDoubleArray VRWandMoveEvent::getPosition()
{
	return getDataIndex()->getValue("/WandMove/position");
}

VRString VRWandMoveEvent::getDescription()
{
	return getDataIndex()->getValue("/WandMove/description");
}


