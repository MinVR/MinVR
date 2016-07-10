///*
// * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0.
// * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
// *
// * Code author(s):
// * 		Dan Orban (dtorban)
// */
//
//#include <plugin/VRPlugin.h>
//
//// special: include this only once in one .cpp file per plugin
//#include <plugin/VRPluginVersion.h>
//
//
//namespace MinVR {
//
//class VRPythonPlugin : public VRPlugin {
//public:
//	PLUGIN_API VRPythonPlugin() {
//	}
//	PLUGIN_API virtual ~VRPythonPlugin() {
//	}
//	PLUGIN_API void registerWithMinVR(VRMainInterface *vrMain) {
//	}
//	PLUGIN_API void unregisterWithMinVR(VRMainInterface *vrMain) {
//	}
//};
//
//} // end namespace
//
//extern "C"
//{
//	PLUGIN_API MinVR::VRPlugin* createPlugin() {
//		return new MinVR::VRPythonPlugin();
//	}
//}
//
