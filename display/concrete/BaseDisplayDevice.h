/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef BASEDISPLAYDEVICE_H_
#define BASEDISPLAYDEVICE_H_

#include "display/VRDisplayDevice.h"

namespace MinVR {

class BaseDisplayDevice : public VRDisplayDevice {
public:
	virtual ~BaseDisplayDevice();

	virtual int getDisplayXOffset();
	virtual int getDisplayYOffset();
	virtual int getXOffset();
	virtual int getYOffset();
	virtual int getWidth();
	virtual int getHeight();
	virtual bool isOpen();
	virtual bool allowThreading();
	virtual void setAllowThreading(bool allowed);
	virtual bool isQuadbuffered();

	virtual void initialize();

	VRDisplayDevice* getParent() const;
	void setParent(VRDisplayDevice* parent);
	const std::vector<VRDisplayDevice*>& getSubDisplays() const;
	void addSubDisplay(VRDisplayDevice* display);

protected:
	BaseDisplayDevice();

	virtual void useDisplay(const MinVR::VRDisplayAction& action);
	void startRenderingAllDisplays(const MinVR::VRRenderer& renderer, VRRenderState& state);
	void useAllDisplays(const MinVR::VRDisplayAction& action);
	void finishRenderingAllDisplays();

private:
	VRDisplayDevice* parent;
	std::vector<VRDisplayDevice*> subDisplays;
	bool _allowThreading;
};

} /* namespace MinVR */

#endif /* BASEDISPLAYDEVICE_H_ */
