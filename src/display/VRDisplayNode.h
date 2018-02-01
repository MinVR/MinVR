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

/** VRDisplayNode is an abstract base class that can be inherited to
    create a different types of displays.
 */

class VRMainInterface;

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

