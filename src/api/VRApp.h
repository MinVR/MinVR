/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRAPP_H_
#define VRAPP_H_

#include "VREvent.h"
//#include "VRAudioState.h"
#include "VRGraphicsState.h"
//#include "VRHapticsState.h"

namespace MinVR {


class VRAppInternal; // forward declaration for implementation details


/** VRGraphicsApp is a simple way to create a graphics VR application.  Developers
  need only to subclass VRApp and override the onVREvent(...), onVRRenderGraphics(...),
  and/or onVRRenderGraphicsContext(...).  To run the application, developers can call run.
  To shut down the application, users can call the shutdown() method.  For example here is how
  to create a simple MinVR Graphics application:

  ------------------------------------------------------------------------
  #include <api/MinVR.h>

  class MyVRApp : public VRApp {
  public:
	MyVRApp(int argc, char** argv) : VRApp(argc, argv) {}

	virtual void onVRRenderGraphics(VRGraphicsState& renderState) {
		// draw graphics
	}
  };

  // see description below on expected command line args
  int main(int argc, char **argv) {
	MyVRApp app(argc, argv);
	app.run();
	return 0;
  }
  ------------------------------------------------------------------------

  MinVR's convention for command line arguments is as follows:

        -h, --help         Display a help message.

        Add any of the following arguments to the command line as many times as
        needed in a space separated list.

        -c <configname>, --load-config <configname>
                           Search for and load the pre-installed MinVR config file
                           named <configname>.minvr -- the search looks in:
                           1. the current working directory [cwd]
                           2. [cwd]/config
                           3. ../../config (for developers running build tree
                              executables from build/bin or tests/testname
                           4. MINVR_ROOT/config if the MINVR_ROOT envvar is defined
                           5. the install_prefix specified when libMinVR was built.

        -s <key>=<value>, --set-value <key>=<value>
                           Add an entry to the MinVR configuration directly from
                           the command line rather than by specifying it in a
                           config file. This can be used to override one specific
                           option in a pre-installed configuration or config file
                           specified earlier on the command line.  For example,\n"
                           'myprogram -c desktop -s WindowHeight=500 -s WindowWidth=500'
                           would start myprogram, load the installed desktop MinVR
                           config and then override the WindowHeight and
                           WindowWidth values in the pre-installed desktop
                           configuration with the new values specified.

        [nothing]          If no command line arguments are provided, then MinVR
                           will try to load the pre-installed default
                           configuration, whis is the same as running the command
                           'myprogram --load-config default'.

        [anything else]    MinVR will silently ignore anything else provided as
                           a command line option.

        --MINVR_DATA=xxxx  A special command line argument reserved for internal
                           use by MinVR.
 */
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

	/**
	 * onVREvent is called when MinVR issues an event callback.  Since event data is extremely diverse,
	 * developers can get the specific event data by named fields using the VREvent interface.
	 */
	virtual void onVREvent(const VREvent &event) {}


	//virtual void onVRRenderAudio(const VRAudioState& state) {}


	/** This function is called once for each time a display node requires the
      scene to be drawn.  For example, a stereo display node will require the
      scene to be drawn twice (once per eye).  For graphics rendering, this is
      where the application makes the OpenGL or other graphics calls to draw the
      scene on the GPU.  Every graphics program will need to override this
      function.  Think of it as the place where you draw your scene.  In some
      cases MinVR needs to interact with the graphics card or operating system
      in order to prepare for this rendering (e.g., MinVR will open the correct
      graphics windows for you), but as much as possible, MinVR attempts to
      simply pass the relevant state on to the application programmer to handle
      as needed in your own shaders.  This is done through the VRGraphicsState
      object.  MinVR updates this data structure as it traverses the display
      graph so that it contains any information you may need to draw graphics
      (e.g., the correct projection matrix to apply in your shaders in order to
      support head tracked stereo rendering).
	 */
	virtual void onVRRenderGraphics(const VRGraphicsState& state) {}

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
	virtual void onVRRenderGraphicsContext(const VRGraphicsState& state) {}

	//virtual void onVRRenderHaptics(const VRHapticsState& state) {}

  /** Returns whether or not the application is running. */
  bool isRunning() const;

	/** Starts the application. */
	void run();

	/** Shuts the application down */
	void shutdown();

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
