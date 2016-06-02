
#include <display/VROffAxisProjectionNode.h>

namespace MinVR {


	VROffAxisProjectionNode::VROffAxisProjectionNode(const std::string &name, VRPoint3 topLeft, VRPoint3 botLeft, VRPoint3 topRight, VRPoint3 botRight, double nearClip, double farClip) :
	VRDisplayNode(name), _topLeft(topLeft), _botLeft(botLeft), _topRight(topRight), _botRight(botRight),  _nearClip(nearClip), _farClip(farClip)
{
}

VROffAxisProjectionNode::~VROffAxisProjectionNode()
{
}


void 
VROffAxisProjectionNode::render(VRDataIndex *renderState, VRRenderHandler *renderHandler)
{
	renderState->pushState();

	// This projection code follows the math described in this paper:
	// http://csc.lsu.edu/~kooima/pdfs/gen-perspective.pdf

	VRPoint3 pa = _botLeft;
	VRPoint3 pb = _botRight;
	VRPoint3 pc = _topLeft;
	VRPoint3 pe(0,0,0);
	if (renderState->exists("LookAtMatrix")){
		VRMatrix4 lookAtMatrix = renderState->getValue("LookAtMatrix");
		pe = VRPoint3(lookAtMatrix[3][0], lookAtMatrix[3][1], lookAtMatrix[3][2]);
	}


	// Compute an orthonormal basis for the screen
	VRVector3 vr = (pb - pa).normalize();
	VRVector3 vu = (pc - pa).normalize();
	VRVector3 vn = vr.cross(vu).normalize();

	// Compute the screen corner vectors
	VRVector3 va = pa - pe;
	VRVector3 vb = pb - pe;
	VRVector3 vc = pc - pe;

	// Find the distance from the eye to screen plane
	double d = -va.dot(vn);

	// Find the extent of the perpendicular projection
	double l = vr.dot(va) * _nearClip / d;
	double r = vr.dot(vb) * _nearClip / d;
	double b = vu.dot(va) * _nearClip / d;
	double t = vu.dot(vc) * _nearClip / d;

	VRMatrix4 projMat = VRMatrix4::projection(l, r, b, t, _nearClip, _farClip);

	renderState->addData("ProjectionMatrix", projMat);

	// Rotate the projection to be non-perpendicular
	VRMatrix4 Mrot(vr[0], vr[1], vr[2], 0.0,
		vu[0], vu[1], vu[2], 0.0,
		vn[0], vn[1], vn[2], 0.0,
		0.0, 0.0, 0.0, 1.0);

	// Move the apex of the frustum to the origin
	VRMatrix4 Mtrans = VRMatrix4::translation(VRPoint3(0, 0, 0) - pe);

	VRMatrix4 viewMat = Mrot * Mtrans;

	renderState->addData("ViewMatrix", viewMat);

	VRDisplayNode::render(renderState, renderHandler);

	renderState->popState();
}


VRDisplayNode*
VROffAxisProjectionNodeFactory::create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace) {

	VRPoint3 topLeft = config->getValue("TopLeft", nameSpace);
	VRPoint3 botLeft = config->getValue("BottomLeft", nameSpace);
	VRPoint3 topRight = config->getValue("TopRight", nameSpace);
	VRPoint3 botRight = config->getValue("BottomRight", nameSpace);
	double nearClip = config->getValue("NearClip", nameSpace);
	double farClip = config->getValue("FarClip", nameSpace);

	VROffAxisProjectionNode *node = new VROffAxisProjectionNode(nameSpace, topLeft, botLeft, topRight, botRight, nearClip, farClip);

	return node;
}


} // end namespace


