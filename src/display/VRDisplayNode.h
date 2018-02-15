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
#include <set>

#include <config/VRDataIndex.h>
#include <main/VRRenderHandler.h>

namespace MinVR {

class VRMainInterface;

/// \brief VRDisplayNode is an abstract base class that can be inherited to
///    create different types of display nodes.
///
/// Use VRDisplayNode to create new components for a render tree.
/// This is a structure that MinVR uses to control the context within
/// which the user's render function is called.  Using the structure
/// specified in the configuration file, MinVR calls the render
/// function of the root of the render tree, which adds some
/// information to the render state and then calls the render function
/// of its children.  They, in turn, add information to the state and
/// call the render function of their children, and so on.  The leaf
/// nodes in the tree call the user's render function, which has
/// access to all of the information added to the render state by the
/// nodes above it.
///
/// This example of a tree might be used for a simple desktop display:
///
///   ~~~
///   Root Window Node > Head Tracking Node > Projection Node
///   ~~~
///
/// The root node adds some display data to the render function, and
/// passes it to a Head Tracking Node, which adds the camera matrix to
/// the render state and passes it to a projection node, which adds a
/// projection matrix, before calling the user's render function.
///
/// Another example, showing a branch in the tree for a stereo view:
///
///   ~~~
///   Root Node > Head Tracking Node > Stereo Node > Left Projection Node
///                                                > Right Projection Node
///   ~~~
///
/// This is the same situation as above, but the stereo node moves the
/// eye position it gets from the Head Tracking Node slightly to the
/// left or the right before calling the render functions for each of
/// its two children nodes.
///
/// ## STANDARDS
///
/// The render tree structure is a flexible way to control the context
/// within which a user's render function is executed, but if you are
/// writing a new node, life will be better for everyone if you can
/// make your node conform to existing naming and data type standards.
/// This will allow other (possibly existing) nodes to understand your
/// node's data seamlessly.
///
/// These are the data names and types currently in use for various
/// graphic functions.  If you can find the data you need in this
/// list, you will also find the name under which it is likely to be
/// filed.  Again, life will be better for everyone if you use these
/// names wherever possible.
///
/// Name       |   Type           |   Data
/// -----------|------------------|-------------
/// WindowX    | VRInt            | X coordinate of upper-left window position in pixels.
/// WindowY    | VRInt            | Y coordinate of upper-left window position in pixels.
/// WindowWidth| VRInt            | Width of the window in pixels.
/// WindowHeight| VRInt            | Height of the window.
/// FrameBufferWidth | VRInt      | Width of the frame buffer in pixels.
/// FrameBufferHeight | VRInt      | Height of the frame buffer in pixels.
/// SharedContextId | VRInt        | ??
/// WindowID    |   VRInt          | ??
/// HeadMatrix | VRFloatArray(16) | 4x4 column-major pose matrix showing the pose of the user's head.
/// CameraMatrix | VRFloatArray(16)| 4x4 column-major pose matrix showing the pose of the camera.  This will be related to the head matrix, but will be different by half the eye separation.
/// ProjectionMatrix | VRFloatArray(16)| 4x4 column-major projection matrix.
///


class VRDisplayNode {
public:
	VRDisplayNode(const std::string &name);

	/// VRDisplayNode assumes that it owns its children, so it will
	/// delete its children when destroyed.  If this functionality is
	/// not desired, create an adapter display node with a weak pointer.
	virtual ~VRDisplayNode();

	/// Returns the name given to this display node in the config file.
	virtual std::string getName() const { return _name; }

	/// Returns a string describing the type of the node (e.g.,
	/// VRGroupNode, VRWindowNode)
	virtual std::string getType() const = 0;

	/// Calls the render function on each of the display node's
	/// children.  If there are no children, then we are at a leaf node
	/// and onVRRenderScene() is called for each render handler.  Some
	/// subclasses, such as VRGraphicsWindow nodes should override this
	/// method and call renderHandler->onVRRenderContext() before
	/// calling the children render functions so that the application
	/// programmer will have a callback at the context level as well as
	/// the scene level.
	virtual void render(VRDataIndex *renderState, VRRenderHandler *renderHandler);

	/// In cases where rendering is asynchronous (e.g., the GPU executes
	/// commands in parallel with the CPU), subclasses should implement
	/// a way to block during this function until the rendering is
	/// complete (e.g., call glFinish()).  This is needed to support
	/// synchronized rendering on multiple networked nodes.
	virtual void waitForRenderToComplete(VRDataIndex *renderState);

	/// Actually displays the rendered results.  This call should be
	/// optimized to be as fast as possible, with the majority of the
	/// rendering work happening in the render() function.  For graphics
	/// displays, the only thing this call should do is swapBuffers.
	virtual void displayFinishedRendering(VRDataIndex *renderState);

	/// Returns the display node's children
	virtual const std::vector<VRDisplayNode*>& getChildren() const;

	/// Adds a display node child
	virtual void addChild(VRDisplayNode* child);

	/// Clears the children in a node
	void clearChildren(bool destroyChildren = false);

	/// Creates the children of the node
	virtual void createChildren(VRMainInterface *vrMain,
                              VRDataIndex *config,
                              const std::string &nameSpace);

	static std::string getAttributeName(){ return "displaynodeType"; };

  /// Returns a list of the values added to the render state by this
  /// node, and its children nodes.  Call this on the root node to get
  /// a list of all the values added to the render state, and by whom.
  virtual std::map<std::string,std::string> getValuesAdded();

  /// This function is meant to be called by a parent node.  It accepts a
  /// list of value names and compares it to a set of names that it requires.
  /// Essentially it is answering the question of whether it will have the
  /// data it needs to do its job when its render() method is called.  This
  /// function's role is to provide as informative a set of error messages as
  /// possible so that the user will not be left to puzzle over some "value
  /// not found" message later, during the render phase.  The method is
  /// called on this object, as well as on its children, so we can audit the
  /// entire display node graph.
  ///
  /// The return is void because the function will throw an error if a
  /// mismatch is found.  Return = success.  The treeData is printed in the
  /// event of throwing an error, and is expected to be a rendition of the
  /// node tree, as would be created by printNode().
  void auditValues(const std::string &treeData) {
    std::set<std::string> dummy;
    auditValues(dummy, treeData);
  }
  void auditValues(std::set<std::string> valuesSet,
                   const std::string &treeData);

  /// Provides a printable rendering of the display node graph, with an
  /// annotation for when a value needed by a lower node is not supplied by
  /// any node above it.
  std::string printNode(const std::string &prefix = "") const {
    std::set<std::string> dummy;
    return printNode(dummy, prefix);
  }
  std::string printNode(std::set<std::string> valuesSet,
                        const std::string &prefix = "") const;

protected:
	std::vector<VRDisplayNode*> _children;
  std::string _name;

  // This contains a list of the values added by this node.  When
  // getValuesAdded() is invoked, this will be returned, appended to a list of
  // values added by this node's children.
  std::list<std::string> _valuesAdded;

  // This is a list of the values needed by this node.  The boolean indicates
  // whether the value is required or not.  A missing value that is required
  // (TRUE) will cause the auditValues() to fail.  If the value is not required,
  // a warning will be printed.
  std::list<std::pair<std::string, bool> > _valuesNeeded;

  /// This is just a syntax sweetener to hide the std::pair stuff.  Use this to
  /// add needed values to the audit.  Set the "required" argument to false for
  /// optional arguments, but remember that if you check for optional arguments,
  /// the user will see a warning during the audit if they're not there.
  void _addValueNeeded(const std::string name, const bool required = true) {
    _valuesNeeded.push_back(std::pair<std::string, bool>(name, required));
  }

  friend std::ostream & operator<<(std::ostream &os, const VRDisplayNode& p) {
    return os << p.printNode("");
  };
  friend std::ostream & operator<<(std::ostream &os, const VRDisplayNode* p) {
    return os << p->printNode("");
  };
};

}

#endif

