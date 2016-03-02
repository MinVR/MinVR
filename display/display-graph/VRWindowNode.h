#ifndef VRWINDOWNODE_H_
#define VRWINDOWNODE_H_

#include "VRDisplayGraphNode.h"

#include <vector>

namespace MinVR {
	
	class VRWindowNode : public VRDisplayNode
	{
	public:

		virtual ~VRWindowNode();

		virtual void render() = 0;
		virtual void join() = 0;
		virtual void synchronize() = 0;

		virtual void addChild( VRWindowChildNode* );

	private:
		std::vector< VRWindowChildNode* > m_children;
	};
	
}

#endif