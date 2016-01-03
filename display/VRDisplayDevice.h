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

	VRDisplayDevice* getParent() const {
		return parent;
	}

	void setParent(VRDisplayDevice* parent) {
		this->parent = parent;
	}

	const std::vector<VRDisplayDevice*>& getSubDisplays() const {
		return subDisplays;
	}

	void addSubDisplay(VRDisplayDevice* display)
	{
		subDisplays.push_back(display);
	}

protected:
	virtual void startRendering(const MinVR::VRRenderer& renderer, int x) = 0;
	void startRendering(VRDisplayDevice* &display, const MinVR::VRRenderer& renderer, int x)
	{
		display->startRendering(renderer, x);
	}

private:
	VRDisplayDevice* parent;
	std::vector<VRDisplayDevice*> subDisplays;
};

class VRDisplayDeviceFactory {
public:
	virtual ~VRDisplayDeviceFactory() {}

	virtual std::vector<VRDisplayDevice*> create(VRDataIndex& config, const std::string nameSpace, VRDisplayDeviceFactory* factory) = 0;
};

class SimpleVRDisplayFactory : public VRDisplayDeviceFactory {
public:
	virtual ~SimpleVRDisplayFactory() {
		for (int f = 0; f < displays.size(); f++)
		{
			delete displays[f];
		}
	}

	std::vector<VRDisplayDevice*> create(VRDataIndex& config, const std::string nameSpace, VRDisplayDeviceFactory* factory)
	{
		std::vector<VRDisplayDevice*> newDisplays;

		VRContainer item = config.getValue(nameSpace);

		for (VRContainer::iterator f = item.begin(); f != item.end(); f++)
		{
			if (config.getType(*f) == VRCORETYPE_CONTAINER) {

				VRContainer item = config.getValue(*f);
				if (config.exists("displayType", *f))
				{
					std::string displayType = config.getValue("displayType", *f);
					VRDisplayDevice* display = createDisplay(displayType, *f, config, factory);
					if (display != NULL)
					{
						newDisplays.push_back(display);
						displays.push_back(display);

						std::vector<VRDisplayDevice*> subDisplays = factory->create(config, *f, factory);
						for (int f = 0; f < subDisplays.size(); f++)
						{
							subDisplays[f]->setParent(display);
							display->addSubDisplay(subDisplays[f]);
						}
					}
				}
			}
		}

		return newDisplays;
	}

	virtual VRDisplayDevice* createDisplay(const std::string type, const std::string name, VRDataIndex& config, VRDisplayDeviceFactory* factory) = 0;

protected:
	std::vector<VRDisplayDevice*> displays;
};

#endif
