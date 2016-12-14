#ifndef VRRENDERHANDLER_H
#define VRRENDERHANDLER_H

#include <config/VRDataIndex.h>


namespace MinVR {

class VRDisplayNode;
  
/** This abstract interface class provides the hook / callback mechanism for
    application programmers to render graphics, audio, haptics or other content.
    Application programmers should subclass from VRRenderHandler and then register
    their class with MinVR using VRMain::registerRenderHandler(..);
*/
class VRRenderHandler {
public:
	virtual ~VRRenderHandler() {}
  /// This function must be implemented in subclasses.  onVRRender(..) is
  /// called from within VRMain::renderDisplayGraph() once for each time a
  /// display node requires the scene to be drawn.  For example, a stereo
  /// display node will require the scene to be drawn twice (once per eye).
  /// For graphics rendering, this is where the application makes the OpenGL
  /// or other graphics calls to draw the scene on the GPU.  In some cases
  /// MinVR needs to interact with the graphics card or operating system in
  /// order to prepare for this rendering (e.g., MinVR will open the correct
  /// graphics windows for you), but as much as possible, MinVR attempts to
  /// simply pass the relevant state on to the application programmer to
  /// handle as needed in your own shaders.  This is done through the
  /// VRDataIndex that is passed in to the function.  MinVR updates this
  /// data structure as it traverses the display graph so that it contains
  /// any information you may need to draw graphics (e.g., the correct 
  /// projection matrix to apply in your shaders in order to support head
  /// tracked stereo rendering).
  virtual void onVRRender(VRDataIndex *renderState, VRDisplayNode *callingNode) = 0;
};

/**
 * VRRenderHandlerImpl allows subclasses to specify an implementation of the
 * render handler.  This mostly helps in allowing developers to specify their
 * own implementation of onVRRender(...) in cpp files rather than in the header
 * files.
 */
class VRRenderHandlerImpl : public VRRenderHandler {
public:
	/**
	 * Pass in a pointer to a render handler proxy object.  The VRRenderHandlerImpl will
	 * delete the implementation object for simplicity.
	 */
	VRRenderHandlerImpl(VRRenderHandler* handler) : handler(handler) {}
	virtual ~VRRenderHandlerImpl() { delete handler; }

	/** Calls internal implementation of the render handler.
	 */
	void onVRRender(VRDataIndex *renderState, VRDisplayNode *callingNode) {
		handler->onVRRender(renderState, callingNode);
	}

private:
	VRRenderHandler* handler;

};

} // end namespace

#endif
