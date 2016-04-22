
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

	std::string type = config->getValue("Type", nodeNameSpace);
	if (type != "VRGroupNode") {
		// This factory cannot create the type specified
		return NULL;
	}

	VRDisplayNode *node = new VRGroupNode(valName);

    if (config->exists("Children", nodeNameSpace)) {

  	  std::vector<std::string> childrenNames = config->getValue("Children", nodeNameSpace);
	  for (std::vector<std::string>::iterator it = childrenNames.begin(); it < childrenNames.end(); ++it) {
		VRDisplayNode *child = vrMain->getFactory()->createDisplayNode(vrMain, config, *it, "/MinVR/");
		if (child != NULL) {
			node->addChild(child);
		}
	  }
    }

	return node;
}


} // end namespace


