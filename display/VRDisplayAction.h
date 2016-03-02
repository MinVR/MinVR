/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRDISPLAYACTION_H_
#define VRDISPLAYACTION_H_

namespace MinVR {

class VRDisplayAction {
public:
	virtual ~VRDisplayAction() {}

	virtual void exec() const = 0;
};

}

#endif
