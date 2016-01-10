/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef COMMANDLINEDISPLAY_H_
#define COMMANDLINEDISPLAY_H_

#include "display/concrete/BaseDisplayDevice.h"
#include "display/concrete/BaseDisplayFactory.h"

namespace MinVR {

class CommandLineDisplay : public BaseDisplayDevice {
public:
	CommandLineDisplay();
	virtual ~CommandLineDisplay();

	bool allowGraphics() { return false; }
	bool allowThreading() { return true; }

	void finishRendering();

protected:
	void startRendering(const VRRenderer& renderer, VRRenderState& state);
	void useDisplay(const MinVR::VRDisplayAction& action);
};

class CommandLineDisplayFactory : public BaseDisplayFactory {
public:
	CommandLineDisplayFactory();
	virtual ~CommandLineDisplayFactory();

	VRDisplayDevice* createDisplay(const std::string type, const std::string name, VRDataIndex& config, VRDisplayDeviceFactory* factory);
};

} /* namespace MinVR */

#endif /* COMMANDLINEDISPLAY_H_ */
