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
	PLUGIN_API int VRDataIndex_getType(void* index, const char* valName, const char* nameSpace) {
		return ((VRDataIndex*)index)->getType(valName, nameSpace);
	}

	PLUGIN_API void VRDataIndex_freeValue(int type, void* value) {
		if (type == VRCORETYPE_STRING) {
			delete[] static_cast<char*>(value);
		}
		else if (type == VRCORETYPE_INTARRAY) {
			delete[] static_cast<int*>(value);
		}
		else if (type == VRCORETYPE_FLOATARRAY) {
			delete[] static_cast<float*>(value);
		}
	}

	// Get the data index value as an integer
	PLUGIN_API int VRDataIndex_getIntValue(void* index, const char* valName, const char* nameSpace) {
			return  ((VRDataIndex*)index)->getValue(valName, nameSpace);
	}

	// Get the data index value as an integer
	PLUGIN_API float VRDataIndex_getFloatValue(void* index, const char* valName, const char* nameSpace) {
			return  ((VRDataIndex*)index)->getValue(valName, nameSpace);
	}

	// Get the data index value as an integer
	PLUGIN_API char* VRDataIndex_getStringValue(void* index, const char* valName, const char* nameSpace) {
		std::string s = ((VRDataIndex*)index)->getValue(valName, nameSpace);
		char *c = new char[s.size()+1];
		strncpy(c, s.c_str(), s.size());
		c[s.size()] = '\0';
		return c;
	}

	PLUGIN_API int* VRDataIndex_getIntArrayValue(void* index, const char* valName, const char* nameSpace, int* size) {
        std::vector<int> v = ((VRDataIndex*)index)->getValue(valName, nameSpace);
        int *a = new int[v.size()];
        std::copy(v.begin(), v.end(), a);
        *size = (int)v.size();
        return a;
	}

	PLUGIN_API float* VRDataIndex_getFloatArrayValue(void* index, const char* valName, const char* nameSpace, int* size) {
        std::vector<float> v = ((VRDataIndex*)index)->getValue(valName, nameSpace);
        float *a = new float[v.size()];
        std::copy(v.begin(), v.end(), a);
        *size = (int)v.size();
        return a;
	}
}
