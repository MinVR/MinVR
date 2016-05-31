/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRDISPLAYNODE_H_
#define VRDISPLAYNODE_H_

#include <vector>
#include <string>

#include <config/VRDataIndex.h>
#include <main/VRRenderHandler.h>

namespace MinVR {

/** VRDisplayNode is an abstract base class that can be inherited to create a different types of displays.
 */
 
class VRMainInterface;
 
class VRDisplayNode {
public:
	VRDisplayNode(const std::string &name);

	/// VRDisplayNode assumes that it owns its children, so it will delete its children when destroyed.  If
	/// this functionality is not desired, create an adapter display node with a weak pointer.
	virtual ~VRDisplayNode();

	/// Returns the name given to this display node in the config file.
	virtual std::string getName() { return _name; }

	/// Returns a string describing the type of the node (e.g., VRGroupNode, VRWindowNode)
	virtual std::string getType() = 0;

	/// Calls render function on each of the display node's children.  If there are no children, then we are
	/// at a leaf node and onVRRenderScene() is called for each render handler.  Some subclasses, such as
	/// VRGraphicsWindow nodes should override this method and call renderHandler->onVRRenderContext() 
	/// before calling the children render functions so that the application programmer will have
	/// a callback at the context level as well as the scene level.
	virtual void render(VRDataIndex *renderState, VRRenderHandler *renderHandler);

	/// In cases where rendering is asynchronous (e.g., the GPU executes commands in parallel with the CPU), 
	/// subclasses should implement a way to block during this function until the rendering is complete
	/// (e.g., call glFinish()).  This is needed to support synchronized rendering on multiple networked nodes.
	virtual void waitForRenderToComplete(VRDataIndex *renderState);

	/// Actually displays the rendered results.  This call should be optimized to be as fast as possible,
	/// with the majority of the rendering work happening in the render() function.  For graphics 
	/// displays, the only thing this call should do is swapBuffers.
	virtual void displayFinishedRendering(VRDataIndex *renderState);

	/// Returns the display node's children
	virtual const std::vector<VRDisplayNode*>& getChildren() const;

	/// Adds a display node child
	virtual void addChild(VRDisplayNode* child);

	/// Clears the children in a node
	void clearChildren(bool destroyChildren = false);

	/// Creates the children of the node
	virtual void createChildren(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);

protected:
	std::vector<VRDisplayNode*> _children;
    std::string _name;
};

}

#endif

