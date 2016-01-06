
#ifndef VREVENTFACTORY_H
#define VREVENTFACTORY_H
#include "VREvent.h"


/** VREventFactory
 */
typedef bool (*VREventCallback)(VREvent **);

class VREventFactory {
public:
	static VREventFactory* getInstance();

	virtual ~VREventFactory();

	void RegisterFactoryCallback(VREventCallback callback);

	VREvent * createVREvent(const std::string &serialized_string);

private:
	static VREventFactory* instance;
	std::vector <VREventCallback> callbacks;

protected:
	VREventFactory();

};


#endif
