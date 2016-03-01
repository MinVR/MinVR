#ifndef VRVIEWPORTNODE_H_
#define VRVIEWPORTNODE_H_

#include "VRWindowChildNode.h"

namespace MinVR {
	
	class VRViewportNode : public VRWindowChildNode
	{

	public:

		virtual ~VRViewportNode();

		virtual void render() = 0;
		virtual void join() = 0;
		virtual void synchronize() = 0;

	};
	
}

#endif