/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRAPP_H_
#define VRAPP_H_

#include "main/VRMain.h"
#include "VRAnalogEvent.h"
#include "VRAudioState.h"
#include "VRButtonEvent.h"
#include "VRConsoleState.h"
#include "VRCursorEvent.h"
#include "VRGraphicsState.h"
#include "VRHapticsState.h"
#include "VRTrackerEvent.h"

namespace MinVR {


class VRAppInternal; // forward declaration for implementation details


/** VRApp is a simple way to create a graphics VR application. */
class VRApp {
public:
	/**
	 * VRApp expects command line parameters using the MinVR command line convention.
	 */
	VRApp(int argc, char** argv);

	/**
	 * Default destructor shuts down the application and the interface with MinVR
	 */
	virtual ~VRApp();




	/** USER INTERFACE CALLBACKS **/

    virtual void onAnalogChange(const VRAnalogEvent &state) {}

	virtual void onButtonDown(const VRButtonEvent &state) {}

	virtual void onButtonUp(const VRButtonEvent &state) {}

	virtual void onCursorMove(const VRCursorEvent &state) {}

	virtual void onTrackerMove(const VRTrackerEvent &state) {}


	/** RENDERING CALLBACKS **/

    virtual void onRenderAudio(const VRAudioState& state) {}


    virtual void onRenderConsole(const VRConsoleState& state) {}


	/** This function is called once for each time a display node requires the scene
		      to be drawn.  For example, a stereo display node will require the scene to
		      be drawn twice (once per eye).  For graphics rendering, this is where the
		      application makes the OpenGL or other graphics calls to draw the scene on
		      the GPU.  Every graphics program will need to override this function.  Think
		      of it as the place where you draw your scene.  In some cases MinVR needs to
		      interact with the graphics card or operating system in order to prepare for
		      this rendering (e.g., MinVR will open the correct graphics windows for you),
		      but as much as possible, MinVR attempts to simply pass the relevant state on
		      to the application programmer to handle as needed in your own shaders.  This
		      is done through the VRGraphicsState object.  MinVR updates this data structure
		      as it traverses the display graph so that it contains any information you may
		      need to draw graphics (e.g., the correct projection matrix to apply in your
		      shaders in order to support head tracked stereo rendering).
	 */
	virtual void onRenderGraphicsScene(const VRGraphicsState& state) {}


	/** Whereas onVRRenderGraphics(..) is called once per scene (e.g., twice for a
      simple stereo display), onVRRenderGraphicsContext(..) is called once per
      rendering context.  For a stereo graphics application, onVRRenderContext()
      will be called once per frame for each active graphics context (graphics
      window), and THEN onVRRenderScene() will be called once per eye to draw
      the scene multiple times.  Thus, to write optimal graphics rendering code,
      onVRRenderContext() is the place where programmers should do any
      computation that is the same for both eyes, such as loading textures or
      mesh data into graphics card memory.
	 */
	virtual void onRenderGraphicsContext(const VRGraphicsState& state) {}

	virtual void onRenderHaptics(const VRHapticsState& state) {}


	/** Starts the application and does not return until the application exits. */
	void run();

	/** Frees memory and other resources.  Typically called after run() completes.
	    If shutdown() is called while the app is still running, it will cause the
	    app to exit from run() the next time through the mainloop. */
	void shutdown();

    /** Returns the name of the VRSetup that this app is running.  Set by the
        config files read by VRMain. */
	std::string getVRSetupName();

    /** After parsing the command line, the number of arguments unused by MinVR
        can be retrieved with this function.  */
    int getLeftoverArgc();

    /** After parsing the command line, the values of arguments unused by MinVR
        can be retrieved with this function.  */
    char** getLeftoverArgv();

private:

	VRAppInternal *_internal;  // opaque pointer to internal implementation

};

} /* namespace MinVR */

#endif /* VRAPP_H_ */
