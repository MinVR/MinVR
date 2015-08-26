#ifndef VRABSTRACTGRAPHICSTOOLKIT_H
#define VRABSTRACKGRAPHICSTOOLKIT_H

/** Interface for graphics toolkits to be implemented as plugins.  Avoiding a dependency on a specific graphics toolkit
    is one of the key design decisions in MinVR, and this is the abstraction that enables that.  Within MinVR, any 
    required graphics functions are handled as calls to VRAbstractGraphicsToolkit.  At runtime, these are filled in by a 
    concrete implementation of the GraphicsToolkit.  We plan for several alternative contrete implementations to be
    created via MinVR's plugin system, e.g., OpenGLGraphicsToolkit, GlutGraphicsToolkit, G3D8GraphicsToolkit, 
    G3D9GraphicsToolkit, G3D10GraphicsToolkit, VTKGraphicsToolkit, OSGGraphicsToolkit, ...
*/
class VRAbstractGraphicsToolkit {
public:
  /// Creates a window that holds a graphics rendering context with the specific features requested in VRWindowSettings.
  /// Returns a unique ID for the window/context or -1 if the window could not be created for some reason.
  int createWindow(VRWindowSettings) = 0;


  /// For use with multi-windowed systems
  void setCurrentGraphicsContext(int graphicsWindowID) = 0;


  /// Clears the screen for the current graphics context.  (Do we need to include any options here?)
  void clear() = 0;


  enum DrawBufferType {
    BACK,
    BACK_LEFT,
    BACK_RIGHT
  };
  /// Sets the frame buffer that all drawing commands will be sent to for the current graphics context.  In non-VR
  /// graphics this defaults to the BACK buffer, and typically does not need to be changed.  But, for quadbuffered
  /// stereo, there are two back buffers, one for each eye.
  void setDrawBuffer(DrawBufferType buffer) = 0;


  enum ColumnMaskType {
    DRAW_ON_ODD_COLUMNS_ONLY,
    DRAW_ON_EVEN_COLUMNS_ONLY    
  };
  /// For column-interlaced stereo displays, MinVR needs the graphics toolkit to enable drawing on just odd or even columns.
  /// Inside the graphics toolkit, this would usually be implemented using a stencil buffer.  Column-interlaced stereo is 
  /// not as common as other stereo formats, such as side-by-side or quadbuffered, so some concrete implemenations of
  /// this class may choose to not support column-interlaced stereo.  In this case, they should still implement these
  /// functions, but just make them report an error saying that the functionality is not supported for that particular
  /// graphics toolkit.
  void enableColumnMask(ColumnMaskType maskType) = 0;
  void disableColumnMask() = 0;


  /// Calls swapbuffers for the current graphics context, which tells the graphics card to swap the front and back frame buffers.
  void swapBuffers() = 0;

};

#endif

