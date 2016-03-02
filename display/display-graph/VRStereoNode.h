#ifndef VRSTEREONODE_H_
#define VRSTEREONODE_H_

#include "VRWindowChildNode.h"

namespace MinVR {
	
	class VRStereoNode : public VRWindowChildNode
	{
		virtual ~VRStereoNode();

		virtual void render();
		virtual void join();
		virtual void synchronize();
	};
}

#endif