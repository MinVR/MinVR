#ifndef DISPLAYDEVICE_H
#define DISPLAYDEVICE_H

#include "config/VRDataIndex.h"
#include "display/VRRenderer.h"
#include "display/VRDisplayAction.h"
#include "display/VRDisplayFrameAction.h"
#include "display/VRRenderState.h"
#include "display/VRFrameController.h"
#include <string>
#include <vector>

/** DisplayDevice:    An as-simple-as-possible public interface for Display Devices
 */
class VRDisplayDevice {
public:
	virtual ~VRDisplayDevice() {}

	virtual std::string getName() const = 0;
	virtual void setName(std::string& name) = 0;
	virtual int getDisplayXOffset() = 0;
	virtual int getDisplayYOffset() = 0;
	virtual int getXOffset() = 0;
	virtual int getYOffset() = 0;
	virtual int getWidth() = 0;
	virtual int getHeight() = 0;
	virtual bool isOpen() = 0;
	virtual bool allowThreading() = 0;
	virtual bool allowGraphics() = 0;
	virtual bool isQuadbuffered() = 0;

	virtual void initialize();
	bool renderFrame(MinVR::VRDisplayFrameAction& frameAction);
	void use(const MinVR::VRDisplayAction& action);
	void startRendering(const MinVR::VRRenderer& renderer);
	virtual void finishRendering() = 0;
	void render(const MinVR::VRRenderer& renderer);

	virtual VRDisplayDevice* getParent() const = 0;
	virtual void setParent(VRDisplayDevice* parent) = 0;
	virtual const std::vector<VRDisplayDevice*>& getSubDisplays() const = 0;
	virtual void addSubDisplay(VRDisplayDevice* display) = 0;

	template<class T>
	void render(T *obj, void (T::*method)(VRRenderState&) const);
	void render(void (*method)(VRRenderState&));

	template<class T>
	void use(T *obj, void (T::*method)() const);
	void use(void (*method)());

	template<class T>
	bool renderFrame(T *obj, bool (T::*method)());
	bool renderFrame(bool (*method)());

	template<class T>
	void startRendering(T *obj, void (T::*method)(VRRenderState&) const);
	void startRendering(void (*method)(VRRenderState&));
	static void startRendering(VRDisplayDevice* &display, const MinVR::VRRenderer& renderer, VRRenderState& renderState);

	virtual MinVR::VRFrameController* getFrameController() = 0;

protected:
	virtual void startRendering(const MinVR::VRRenderer& renderer, VRRenderState& renderState) = 0;
	virtual void useDisplay(const MinVR::VRDisplayAction& action) = 0;
};

template<class T>
void VRDisplayDevice::render(T *obj, void (T::*method)(VRRenderState&) const)
{
	render(MinVR::SpecificVRRenderer<T>(obj, method));
}

template<class T>
void VRDisplayDevice::use(T *obj, void (T::*method)() const)
{
	use(MinVR::SpecificVRDisplayAction<T>(obj, method));
}

template<class T>
void VRDisplayDevice::startRendering(T *obj, void (T::*method)(VRRenderState&) const)
{
	startRendering(MinVR::SpecificVRRenderer<T>(obj, method));
}

template<class T>
bool VRDisplayDevice::renderFrame(T *obj, bool (T::*method)())
{
	return renderFrame(MinVR::SpecificVRDisplayFrameAction<T>(obj, method));
}

#endif
