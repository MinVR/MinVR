
#include <display/VRGroupNode.h>

namespace MinVR {


VRGroupNode::VRGroupNode(const std::string &name) : VRDisplayNode(name)
{
}

VRGroupNode::~VRGroupNode()
{
}


VRDisplayNode*
VRGroupNodeFactory::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) {

	VRDisplayNode *node = new VRGroupNode(nameSpace);

	return node;
}


} // end namespace


