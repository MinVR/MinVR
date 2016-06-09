
#include <display/VRGroupNode.h>

namespace MinVR {


VRGroupNode::VRGroupNode(const std::string &name) : VRDisplayNode(name)
{
}

VRGroupNode::~VRGroupNode()
{
}


VRDisplayNode*
VRGroupNode::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &valName, const std::string &nameSpace) {
	std::string nodeNameSpace = nameSpace;

	VRDisplayNode *node = new VRGroupNode(valName);

    if (config->exists("Children", nodeNameSpace)) {

  	  std::vector<std::string> childrenNames = config->getValue("Children", nodeNameSpace);
	  for (std::vector<std::string>::iterator it = childrenNames.begin(); it < childrenNames.end(); ++it) {
		VRDisplayNode *child = vrMain->getFactory()->create<VRDisplayNode>(vrMain, config, *it, "/MinVR/");
		if (child != NULL) {
			node->addChild(child);
		}
	  }
    }

	return node;
}


} // end namespace


