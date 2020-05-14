/**
 * /author Dan Keefe (dfk)
 *
 * /file  GfxMgr.H
 * /brief 
 *
 */ 

#ifndef GFXMGR_H
#define GFXMGR_H

#include "../../VRG3D/include/CommonInc.H"

#include "GfxMgrCallbacks.H"
#include "../../VRG3D/include/ProjectionVRCamera.h"

namespace MinVR {


typedef G3D::ReferenceCountedPointer<class GfxMgr> GfxMgrRef;
/** This is the main class for handling graphical objects in IS3D.  The ideas is that this will
    replace DrawMgr and become an approach of handling drawing that is a bit more friendly to
    G3D's use of PosedModels.  If you want to draw something with direct calls to RenderDevice,
    as was common in the original use of DrawObjs with DrawMgr, you can still do that here.  Have
    your class add a draw callback.  The preferred method of rendering is to have your class 
    create a G3D::PosedModel and let GfxMgr draw that PosedModel for you.  This way you can get
    lighting, correct sorting for transparent rendering, and more taken care of for you.  Create
    your PosedModel(s) in a pose callback that you add to the GfxMgr and then add them to the 
    posedModel array passed to the callback.  
    
    This class and this framework is still under a bit of construction..

    Description of Texture Handling:

    The class holds a database of textures accessable by name.  Any texture files
    specified in the ConfigVal LoadTextures will be automatically
    loaded.  Textures in LoadTextures are separated by a semicolon.
    The format for specifying a texture is:

    \verbatim    
    <filename>[,alpha-channel-filename] <TexMgr keyname> [texture format] [wrap mode] [interpolate mode];
    \endverbatim

   Arguments in <> are required, args in [] are optional.  For 
   example, in a config file you could have:

   \verbatim
    LoadTextures   file1.jpg                  background-image RGB8 TRANSPARENT_BORDER ; \
                   file2.jpg                  foreground-image ; \
		   file3.jpg,file3-alpha.jpg  myTextureWithAnAlpha ;
   \endverbatim
		 
   Possible values for texture format, wrap mode, and interpolate mode
   are the same as G3D's enumerated constants.
*/
class GfxMgr : public G3D::ReferenceCountedObject
{
public:

  PLUGIN_API GfxMgr(G3D::RenderDevice *renderDevice, ProjectionVRCameraRef camera);
  PLUGIN_API virtual ~GfxMgr();

  PLUGIN_API G3D::RenderDevice* getRenderDevice() { return _renderDevice; }

  PLUGIN_API G3D::LightingRef getLighting()              { return _lighting; }
  PLUGIN_API void        setLighting(G3D::LightingRef l) { _lighting = l; }

  PLUGIN_API G3D::SkyRef getSky()         { return _sky; }
  PLUGIN_API void   setSky(G3D::SkyRef s) { _sky = s; }
  PLUGIN_API void   setSkyLightingParams(G3D::SkyParameters lp) { _skyLightingParams = lp; }

  /// The first time it is called, tries to load a default font file and returns a ref to the font.
  PLUGIN_API G3D::GFontRef getDefaultFont();

  PLUGIN_API G3D::Array<G3D::Surface::Ref> getPosedModels() { return _posedModels; }


  /** Register a callback method (must be a method of a class) to be called once
      each new frame (but not once per eye) wherein you may want to repose your graphics
      models based on new user input or a new roomToVirtualSpace coordinateframe.

      void MyClass::pose(Array<PosedModel::Ref> &posedModels, const CoordinateFrame &virtualToRoomSpace);
      should be registered with a call from within MyClass like this
      int id = gfxMgr->addPoseCallback(this, &MyClass::pose);
  */
  template <class T>
  PLUGIN_API int addPoseCallback(T *thisPtr, void (T::*method)(G3D::Array<G3D::Surface::Ref> &posedModels, const G3D::CoordinateFrame &virtualToRoomSpace)) {
    PoseMethodFunctor *f = new SpecificPoseMethodFunctor<T>(thisPtr, method);
    int id = _nextPoseCallbackID;
    _poseCallbacks.set(id, f);
    _nextPoseCallbackID++;
    return id;
  }

  /// TODO: Make this safe to call from within a pose callback.
  PLUGIN_API void removePoseCallback(int id) {
    delete _poseCallbacks[id];
    _poseCallbacks.remove(id); 
  };


  /** This is the same idea as the pose callback except that the callback is called just
      once, for the next frame to be rendered and then never called again.
  */
  template <class T>
  int addOneTimePoseCallback(T *thisPtr, void (T::*method)(G3D::Array<G3D::Surface::Ref> &posedModels, const G3D::CoordinateFrame &virtualToRoomSpace)) {
    PoseMethodFunctor *f = new SpecificPoseMethodFunctor<T>(thisPtr, method);
    int id = _nextOneTimePoseCallbackID;
    _oneTimePoseCallbacks.set(id, f);
    _nextOneTimePoseCallbackID++;
    return id;
  }



  /** Register a callback method (must be a method of a class) to be called once
      per eye each new frame.  Inside this method you can make your usual calls
      to RenderDevice to draw geometry.

      void MyClass::draw(RenderDevice *rd, const CoordinateFrame &virtualToRoomSpace);
      should be registered with a call from within MyClass like this
      int id = gfxMgr->addDrawCallback(this, &MyClass::draw);
  */
  template <class T>
  int addDrawCallback(T *thisPtr, void (T::*method)(G3D::RenderDevice *rd, const G3D::CoordinateFrame &virtualToRoomSpace)) {
    DrawMethodFunctor *f = new SpecificDrawMethodFunctor<T>(thisPtr, method);
    int id = _nextDrawCallbackID;
    _drawCallbacks.set(id, f);
    _nextDrawCallbackID++;
    return id;
  }

  /// TODO: Make this safe to call within a draw callback.
  PLUGIN_API  void removeDrawCallback(int id) {
    delete _drawCallbacks[id];
    _drawCallbacks.remove(id);
  }


  /// Call this from your main loop once per frame
  PLUGIN_API void poseFrame();
  /// Call this from your main loop once per eye per frame after applying the
  /// correct camera transformation onto the OpenGL stack??
  PLUGIN_API void drawFrame(G3D::Vector3 lookVec=G3D::Vector3(0,0,-1));

  PLUGIN_API void drawStats(bool showFrameRate=true, bool showTriRate=true, bool showTrisPerFrame=true);


  /***  Begin Texture Handling Routines  ***/

  PLUGIN_API void               loadTexturesFromConfigVal(const std::string &configName, G3D::Log *log);
  PLUGIN_API G3D::TextureRef         getTexture(const std::string &keyName);
  PLUGIN_API G3D::Array<G3D::TextureRef>  getTextures(const G3D::Array<std::string> &keyNames);
  PLUGIN_API void               setTextureEntry(const std::string &keyName, G3D::TextureRef tex);
  PLUGIN_API void               removeTextureEntry(const std::string &keyName);
  PLUGIN_API std::string        lookupTextureKeyName(G3D::TextureRef tex);
  PLUGIN_API G3D::Array<std::string> getAllTextureKeys();




  /***  Begin Coordinate System Routines  ***/


  /** This is the transformation between RoomSpace and VirtualSpace.
      Initially, the two spaces are the same.  As the user navigates
      around VirtualSpace changes, but RoomSpace stays fixed to the
      physical Cave, monitor, screen, trackers, etc..  This
      transformation is composed of two parts: 1. a uniform scale
      factor, 2. a rigid-body CoordinateFrame transformation.
  */
  PLUGIN_API G3D::CoordinateFrame getRoomToVirtualSpaceFrame();
  PLUGIN_API double          getRoomToVirtualSpaceScale();

  /// Sets the rigid-body CoordinateFrame transformation between the
  /// Room and Virtual spaces.
  PLUGIN_API void setRoomToVirtualSpaceFrame(const G3D::CoordinateFrame &roomToVirtual);
  
  /// Sets the uniform scale factor between tne Room and Virtual
  /// spaces.  Increasing the RoomToVirtualSpaceScale has the effect
  /// of shrinking the virtual space w.r.t the Room - zooming out.  
  PLUGIN_API void setRoomToVirtualSpaceScale(const double scale);
  
  /// Use these to convert from RoomSpace to VirtualSpace, applies a
  /// CoordinateFrame transformation as well as a scale.
  PLUGIN_API G3D::Vector3 roomPointToVirtualSpace(const G3D::Vector3 &v);
  PLUGIN_API G3D::Vector3 roomVectorToVirtualSpace(const G3D::Vector3 &v);
  PLUGIN_API G3D::Vector3 roomNormalToVirtualSpace(const G3D::Vector3 &v);
  PLUGIN_API double  roomDistanceToVirtualSpace(const double &d);

  /// Use these to convert from VirtualSpace to RoomSpace, applies a
  /// CoordinateFrame transformation as well as a scale.
  PLUGIN_API G3D::Vector3 virtualPointToRoomSpace(const G3D::Vector3 &v);
  PLUGIN_API G3D::Vector3 virtualVectorToRoomSpace(const G3D::Vector3 &v);
  PLUGIN_API G3D::Vector3 virtualNormalToRoomSpace(const G3D::Vector3 &v);
  PLUGIN_API double  virtualDistanceToRoomSpace(const double &d);

  PLUGIN_API G3D::Ray roomToVirtualSpace(const G3D::Ray &r);
  PLUGIN_API G3D::Ray virtualToRoomSpace(const G3D::Ray &r);

  PLUGIN_API G3D::CoordinateFrame roomToVirtualSpace(const G3D::CoordinateFrame &c);
  PLUGIN_API G3D::CoordinateFrame virtualToRoomSpace(const G3D::CoordinateFrame &c);


  /// Converts IS3D's screen space coordinates into G3D's coordinates,
  /// which are in pixels rather than floating point.
  PLUGIN_API G3D::Vector2 screenPointToG3DCoords(const G3D::Vector2 &screenPt);

  /// Converts pixel screen space coordinates into IS3D's screen space coordinates
  PLUGIN_API G3D::Vector2 pixelScreenPointToIS3DCoords(const G3D::Vector2 &pixelPt);

  /// Projects a Room space point onto the filmplane, then returns it
  /// in Screen coordinates.
  PLUGIN_API G3D::Vector2 roomSpacePointToScreenPoint(const G3D::Vector3 &roomPt);
  
  /// This converts a screenspace point, like the coordinates of the
  /// mouse to a 3D point by reading back the current Z-buffer to find
  /// the first piece of geometry that is intersected by the point on
  /// the screen.  If there is no geometry at the location then the
  /// point returned will lie roughly on the far clipping plane and
  /// hitGeometry will be set to false.
  PLUGIN_API G3D::Vector3 screenPointToRoomSpacePoint(const G3D::Vector2 &v, bool &hitGeometry, int whichBuffer=GL_FRONT);
  
  /// This converts a screenspace point, like the coordinates of the mouse
  /// to a 3D point that lies in the RoomSpace XY plane (Z=0).
  PLUGIN_API G3D::Vector3 screenPointToRoomSpaceXYPlane(const G3D::Vector2 &v);

  /// This converts a screenspace point, like the coordinates of the mouse
  /// to a 3D point that lies in the RoomSpace plane Z=z.
  PLUGIN_API G3D::Vector3 screenPointToRoomSpaceZEqualsPlane(const G3D::Vector2 &v, double z);

  /// This converts a screenspace point, like the coordinates of the mouse
  /// to a 3D point that lies in IS3DEngine's current notion of where the
  /// filmplane is in RoomSpace.  By the way, the filmplane location and
  /// size will only change if you resize the rendering window.
  PLUGIN_API G3D::Vector3 screenPointToRoomSpaceFilmplane(const G3D::Vector2 &v);

  /// The filmplane, in projected VR, the same as the plane of the screen.
  /// Returned in RoomSpace coordinates.
  PLUGIN_API G3D::Plane getFilmplane();

  ProjectionVRCameraRef  getCamera() { return _camera; }

  
  /// returns the seconds per frame, usefull for input that should depend on
  /// time, not number of frames
  PLUGIN_API float secPerFrame();


  PLUGIN_API static int                     _nextPoseCallbackID;
  PLUGIN_API static int                     _nextOneTimePoseCallbackID;
  PLUGIN_API static int                     _nextDrawCallbackID;
private:
  G3D::Array<G3D::Surface::Ref>           _posedModels;
  G3D::Table<int, PoseMethodFunctor*> _poseCallbacks;
  G3D::Table<int, PoseMethodFunctor*> _oneTimePoseCallbacks;
  G3D::Table<int, DrawMethodFunctor*> _drawCallbacks;
  G3D::GFontRef                       _defaultFont;
  G3D::SkyParameters                  _skyLightingParams;
  G3D::LightingRef                    _lighting;
  ProjectionVRCameraRef          _camera;
  G3D::RenderDevice*                  _renderDevice;
  G3D::SkyRef                         _sky;
  G3D::Table<std::string, G3D::TextureRef> _texMap;

  /// Transformation from RoomSpace to VirtualSpace
   G3D::CoordinateFrame                _roomToVirtual;
  /// Uniform scale factor between RoomSpace and VirtualSpace
   double                         _roomToVirtualScale;

};

} // end namespace

#endif
