/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRGRAPHICSAPP_H_
#define VRGRAPHICSAPP_H_

#include "VRApp.h"
#include "VRGraphicsHandler.h"

namespace MinVR {

/** VRGraphicsApp is a simple way to create a graphics VR application.  Developers
  need only to subclass VRGraphicsApp and override the onVREvent(...), onVRRenderGraphics(...),
  and/or onVRRenderGraphicsContext(...).  To run the application, developers can call run.
  To shut down the application, users can call the shutdown() method.  For example here is how
  to create a simple MinVR Graphics application:

  ------------------------------------------------------------------------

  #include <api/MinVR.h>

  class MyVRApp : public VRGraphicsApp {
  public:
	MyVRApp(int argc, char** argv, const std::string& configFile) : VRGraphicsApp(argc, argv, configFile) {}

	virtual void onVRRenderGraphics(VRGraphicsState& renderState) {
		// draw graphics
	}
  };

  int main(int argc, char **argv) {
    // The second argument is the config file path for this example
	MyVRApp app(argc, argv, argv[1]);
	app.run();
	return 0;
  }

  ------------------------------------------------------------------------
 */
class VRGraphicsApp : public VRApp, public VRGraphicsHandler {
public:
	/**
	 * VRGraphicsApp expects the command line parameters and a config file path.
	 */
	VRGraphicsApp(int argc, char** argv, const std::string& configFile) : VRApp(argc, argv, configFile) {
		addRenderHandler(*this);
	}

	/**
	 * Default constructor shuts down the application and the interface with MinVR
	 */
	virtual ~VRGraphicsApp() {}

	/**
	 * onVREvent is called when MinVR issues an event callback.  Since event data is extremely diverse,
	 * developers can get the specific event data from the VRDataIndex.
	 */
	virtual void onVREvent(const std::string &eventName, VRDataIndex *eventData) {}

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
	virtual void onVRRenderGraphics(VRGraphicsState& renderState) {}

	/** Whereas onVRRenderGraphics(..) is called once per scene (e.g., twice for a
	      simple stereo display), onVRRenderGraphicsContext(..) is called once per
	      rendering context.  For a stereo graphics application, onVRRenderContext(..)
	      will be called once per frame for each active graphics context (graphics
	      window), and THEN onVRRenderScene(..) will be called once per eye to draw
	      the scene multiple times.  Thus, to write optimal graphics rendering code,
	      onVRRenderContext(..) is the place where programmers should do any
	      computation that is the same for both eyes, such as loading textures or
	      mesh data into graphics card memory.
	 */
	virtual void onVRRenderGraphicsContext(VRGraphicsState& renderState) {}
};

} /* namespace MinVR */

#endif /* VRGRAPHICSAPP_H_ */
