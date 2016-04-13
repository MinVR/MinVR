/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef QUADBUFFERSTEREO_H_
#define QUADBUFFERSTEREO_H_

#include "plugin/PluginFramework.h"
#include "display/concrete/BaseDisplayDevice.h"

namespace MinVR {

class QuadbufferStereo : public BaseDisplayDevice {
public:
	PLUGIN_API QuadbufferStereo();
	PLUGIN_API virtual ~QuadbufferStereo();

	PLUGIN_API bool isQuadbuffered() { return true; }

	PLUGIN_API void finishRendering();

protected:
	PLUGIN_API void startRendering(const MinVR::VRRenderer& renderer, VRRenderState& state);
};

} /* namespace MinVR */

#endif /* QUADBUFFERSTEREO_H_ */
