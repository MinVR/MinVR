/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRAPP_H_
#define VRAPP_H_

#include <string>
#include <api/VREventHandler.h>
#include <api/VRRenderHandler.h>

namespace MinVR {

class VRApp : public VREventHandler {
public:
	/**
	 * VRApp expects the command line parameters and a config file path.
	 */
	VRApp(int argc, char** argv, const std::string& configFile);

	/**
	 * Default constructor shuts down the application and the interface with MinVR
	 */
	virtual ~VRApp();

	/**
	 * onVREvent is called when MinVR issues an event callback.  Since event data is extremely diverse,
	 * developers can get the specific event data from the VRDataIndex.
	 */
	virtual void onVREvent(const std::string &eventName, VRDataIndex *eventData) {}

	/** Starts the application. */
	void run();

	/** Shuts the application down */
	void shutdown();

	/** Returns whether or not the application is running. */
	bool isRunning() const;

	/** Returns the current frame number */
	int getFrame() const;

	/** Allows event handlers to be added to the application. */
	void addEventHandler(VREventHandler& handler);

	/** Allows render handlers to be added to the application. */
	void addRenderHandler(VRRenderHandler& handler);

private:
	// opaque reference to application
	class VRAppInternal;
	VRAppInternal &_;
};

} /* namespace MinVR */

#endif /* VRAPP_H_ */
