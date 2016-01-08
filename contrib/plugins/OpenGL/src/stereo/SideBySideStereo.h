/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef SIDEBYSIDESTEREO_H_
#define SIDEBYSIDESTEREO_H_

#include "plugin/PluginFramework.h"
#include "display/VRDisplayDevice.h"

namespace MinVR {

class SideBySideStereo : public VRDisplayDevice {
public:
	PLUGIN_API SideBySideStereo();
	PLUGIN_API virtual ~SideBySideStereo();

	PLUGIN_API int getXOffset();
	PLUGIN_API int getYOffset();
	PLUGIN_API int getWidth();
	PLUGIN_API int getHeight();

	PLUGIN_API void finishRendering();

protected:
	PLUGIN_API void startRendering(const MinVR::VRRenderer& renderer, int x);

private:
	int x, y, width, height;
};

} /* namespace MinVR */

#endif /* SIDEBYSIDESTEREO_H_ */
