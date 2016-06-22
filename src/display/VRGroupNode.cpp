
#include <display/VRGroupNode.h>

namespace MinVR {


VRGroupNode::VRGroupNode(const std::string &name) : VRDisplayNode(name)
{
}

VRGroupNode::~VRGroupNode()
{
}


VRDisplayNode*
VRGroupNode::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) {
	std::string nodeNameSpace = nameSpace;

	VRDisplayNode *node = new VRGroupNode(nameSpace);

	return node;
}


} // end namespace


