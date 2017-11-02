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
  /// This function must be implemented in subclasses.  onVRRenderScene(..) is
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
  virtual void onVRRenderScene(const VRDataIndex &stateData) = 0;

  /// This function has a default empty implementation because it is not
  /// always required.  However, it is important to use if you want to
  /// write optimal rendering code.  Whereas onVRRenderScene(..) is called
  /// once per scene (e.g., twice for a simple stereo display), onVRRenderContext(..)
  /// is called once per rendering context.  For a stereo graphics application, 
  /// onVRRenderContext will be called once per frame for each active graphics
  /// context (i.e., graphics window), and THEN onVRRenderScene(..) will be called
  /// once per eye to draw the scene multiple times.  Thus, to write optimal graphics
  /// rendering code, onVRRenderContext(..) is the place where programmers 
  /// should do any computation that is the same for both eyes, such as loading
  /// textures or mesh data into graphics card memory.
  virtual void onVRRenderContext(const VRDataIndex &stateData) {};
};

} // end namespace

#endif
