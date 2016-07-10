/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#include <plugin/VRPlugin.h>
#include "config/VRDataIndex.h"


using namespace MinVR;

// VRDataIndex specific methods
extern "C" {
	// Get the type of the value requested so we know which specific function to call
	PLUGIN_API int VRDataIndex_getType(void* renderState, const char* valName, const char* nameSpace) {
		return ((VRDataIndex*)renderState)->getType(valName, nameSpace);
	}

	// Get the data index value as an integer
	PLUGIN_API int VRDataIndex_getIntValue(void* renderState, const char* valName, const char* nameSpace) {
			return  ((VRDataIndex*)renderState)->getValue(valName, nameSpace);
	}
}
