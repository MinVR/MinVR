
#include <display/VRGroupNode.h>

namespace MinVR {


VRGroupNode::VRGroupNode(const std::string &name) : VRDisplayNode(name)
{
}

VRGroupNode::~VRGroupNode()
{
}


VRDisplayNode*
VRGroupNodeFactory::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) {
	std::string nodeNameSpace = config->validateNameSpace(nameSpace + valName);

	VRDisplayNode *node = new VRGroupNode(valName);

	return node;
}


} // end namespace


