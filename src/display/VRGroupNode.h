/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 *		Dan Keefe, UMN (following Dan Orban's design)
 */

#ifndef VRGROUPNODE_H
#define VRGROUPNODE_H

#include "VRDisplayNode.h"
#include <main/VRFactory.h>

namespace MinVR {

/** A very simple implementation, essentially the same as the base class, but useful to make the
    concept of a group node explicit and provide a factory for creating those nodes.
 */
class VRGroupNode : public VRDisplayNode {
public:
	VRGroupNode(const std::string &name);
	virtual ~VRGroupNode();

	virtual std::string getType() const { return "VRGroupNode"; }
	static VRDisplayNode* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);
};

} // end namespace

#endif

