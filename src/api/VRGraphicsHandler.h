/*
 * Copyright Regents of the University of Minnesota, 2016.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRGRAPHICSHANDLER_H_
#define VRGRAPHICSHANDLER_H_

#include "api/VRRenderHandler.h"
#include "VRGraphicsState.h"

namespace MinVR {

/** This interface class provides the hook / callback mechanism for
    application programmers to render graphics. Application programmers should subclass
    from VRRenderHandler and then register their class with MinVR using
    VRMain::registerRenderHandler(..);
 */
class VRGraphicsHandler : public VRRenderHandlerImpl {
public:
	VRGraphicsHandler();
	virtual ~VRGraphicsHandler();

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
	virtual void onVRRenderGraphics(VRGraphicsState& renderState) = 0;

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
	virtual void onVRRenderGraphicsContext(VRGraphicsState& renderState) = 0;
};

} /* namespace MinVR */

#endif /* VRGRAPHICSHANDLER_H_ */
