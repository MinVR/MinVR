#ifndef VRWINDOWCHILDNODE_H_
#define VRWINDOWCHILDNODE_H_

#include "VRDisplayGraphNode.h"

#include <vector>

namespace MinVR {

	class VRWindowChildNode : public VRDisplayGraphNode
	{
	public:

		virtual ~VRWindowChildNode();

		virtual void render() = 0;
		virtual void join() = 0;
		virtual void synchronize() = 0;

	};

}

#endif