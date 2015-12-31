#ifndef DISPLAYDEVICE_H
#define DISPLAYDEVICE_H

#include "config/VRDataIndex.h"
#include "display/VRRenderer.h"
#include "display/VRDisplayAction.h"
#include <vector>

/** DisplayDevice:
    An as-simple-as-possible public interface for Display Devices
 */
class VRDisplayDevice {
public:
	virtual ~VRDisplayDevice() {}

	virtual bool isOpen() = 0;
	virtual void use(const MinVR::VRDisplayAction& action) = 0;
	void startRendering(const MinVR::VRRenderer& renderer) { startRendering(renderer, 0); }
	virtual void finishRendering() = 0;

	virtual void render(const MinVR::VRRenderer& renderer)
	{
		startRendering(renderer);
		finishRendering();
	}

	template<class T>
	void render(T *obj, void (T::*method)() const)
	{
		render(MinVR::SpecificVRRenderer<T>(obj, method));
	}

	void render(void (*method)())
	{
		render(MinVR::VRRendererFunctor(method));
	}

	template<class T>
	void use(T *obj, void (T::*method)() const)
	{
		use(MinVR::SpecificVRDisplayAction<T>(obj, method));
	}

	void use(void (*method)())
	{
		use(MinVR::VRDisplayActionFunctor(method));
	}

	template<class T>
	void startRendering(T *obj, void (T::*method)() const)
	{
		startRendering(MinVR::SpecificVRRenderer<T>(obj, method));
	}

	void startRendering(void (*method)())
	{
		startRendering(MinVR::VRRendererFunctor(method));
	}

protected:
	virtual void startRendering(const MinVR::VRRenderer& renderer, int x) = 0;
	void startRendering(VRDisplayDevice* &display, const MinVR::VRRenderer& renderer, int x)
	{
		display->startRendering(renderer, x);
	}
};

class VRDisplayDeviceFactory {
public:
	virtual ~VRDisplayDeviceFactory() {}

	virtual std::vector<VRDisplayDevice*> create(VRDataIndex& config, const std::string nameSpace, VRDisplayDeviceFactory* factory) = 0;
};


#endif
