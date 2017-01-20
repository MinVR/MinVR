
#include <display/VRLookAtNode.h>

namespace MinVR {


VRLookAtNode::VRLookAtNode(const std::string &name, VRMatrix4 initiallookAtMatrix) :
  VRDisplayNode(name),_lookAtMatrix(initiallookAtMatrix)
{
  _valuesAdded.push_back("/LookAtMatrix");
}

VRLookAtNode::~VRLookAtNode()
{
}


void 
VRLookAtNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler)
{
	renderState->pushState();

	renderState->addData("/LookAtMatrix", _lookAtMatrix);
	VRDisplayNode::render(renderState, renderHandler);

	renderState->popState();
}

VRDisplayNode* VRLookAtNode::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) {
	
	VRMatrix4 lookAtMatrix;

	if (config->exists("LookAtMatrix", nameSpace)){
		lookAtMatrix = config->getValue("LookAtMatrix", nameSpace);
	} 
	else if (config->exists("LookAtUp", nameSpace) && config->exists("LookAtEye", nameSpace) && config->exists("LookAtCenter", nameSpace))
	{
		VRVector3 up = config->getValue("LookAtUp", nameSpace);
		VRVector3 eye = config->getValue("LookAtEye", nameSpace);
		VRVector3 center = config->getValue("LookAtCenter", nameSpace);

		VRVector3 z = center - eye;
		z = z.normalize();
		VRVector3 x = up.cross(z);
		x = x.normalize();
		VRVector3 y = z.cross(x);
		
        VRMatrix4 M1 = VRMatrix4::fromRowMajorElements(x[0], y[0], z[0], 0,
                                                       x[1], y[1], z[1], 0,
                                                       x[2], y[2], z[2], 0,
                                                       0, 0, 0, 1);

        VRMatrix4 M2 = VRMatrix4::fromRowMajorElements(1, 0, 0, -eye[0],
                                                       0, 1, 0, -eye[1],
                                                       0, 0, 1, -eye[2],
                                                       0, 0, 0, 1);

		lookAtMatrix =  M1 * M2;
	}
	else
	{
		std::cerr << "Warning : no LookAtMatrix defined for " << nameSpace << std::endl;
		std::cerr << "Either Define  LookAtMatrix or LookAtUp,LookAEye and LookAtCenter" << std::endl;
		std::cerr << "Using IdentityMatrix as default 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 " << std::endl;
	}

	VRLookAtNode *node = new VRLookAtNode(nameSpace, lookAtMatrix);

	return node;
}


} // end namespace


