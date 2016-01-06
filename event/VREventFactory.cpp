#include "VREventFactory.h"

#include <stddef.h>
VREventFactory* VREventFactory::instance = NULL;


VREventFactory::VREventFactory()
{

}

VREventFactory::~VREventFactory()
{
	instance = NULL;
}

void VREventFactory::RegisterFactoryCallback(VREventCallback callback)
{
	callbacks.push_back(callback);
}

VREvent* VREventFactory::createVREvent(const std::string &serialized_string)
{
	VREvent *ev = new VREvent(serialized_string);
	for (std::vector<VREventCallback>::const_iterator it = callbacks.begin(); it != callbacks.end(); ++it)
	{
		if ((*it)(&ev))
		{
			break;
		}
	}
	return ev;
}

VREventFactory* VREventFactory::getInstance()
{
	if (!instance)
	{
		instance = new VREventFactory();
	}
	return instance;
}
