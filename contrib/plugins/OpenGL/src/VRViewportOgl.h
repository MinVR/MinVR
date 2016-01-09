/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRVIEWPORTOGL_H_
#define VRVIEWPORTOGL_H_

#include "plugin/PluginFramework.h"
#include "display/concrete/BaseDisplayDevice.h"

namespace MinVR {

class VRViewportOgl: public BaseDisplayDevice {
public:
	PLUGIN_API VRViewportOgl(int x, int y, int width, int height);
	PLUGIN_API virtual ~VRViewportOgl();

	PLUGIN_API int getXOffset();
	PLUGIN_API int getYOffset();
	PLUGIN_API int getWidth();
	PLUGIN_API int getHeight();

	PLUGIN_API void finishRendering();

protected:
	PLUGIN_API void startRendering(const MinVR::VRRenderer& renderer, VRRenderState& state);

private:
	int x, y, width, height;
};

} /* namespace MinVR */

#endif /* VRVIEWPORTOGL_H_ */
