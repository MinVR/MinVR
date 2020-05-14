/**
 * /author Dan Keefe (dfk)
 *
 * /file  GfxMgrCallbacks.H
 * /brief Callback Functors setup for GfxMgr
 *
 */ 

#ifndef GFXMGRCALLBACKS_H
#define GFXMGRCALLBACKS_H

#include "../../VRG3D/include/CommonInc.H"

namespace MinVR {


/// Functors are used to handle callback methods
class PoseMethodFunctor
{
public:
  PoseMethodFunctor() {}
  virtual ~PoseMethodFunctor() {}
  virtual void exec(G3D::Array<G3D::PosedModel::Ref> &posedModels, const G3D::CoordinateFrame &virtualToRoomSpace) = 0;
};

template <class T>
class SpecificPoseMethodFunctor : public PoseMethodFunctor
{
public:
  typedef void (T::*MethodType)(G3D::Array<G3D::PosedModel::Ref> &posedModels, const G3D::CoordinateFrame &virtualToRoomSpace);
  
  SpecificPoseMethodFunctor(T *obj, MethodType meth) {
    _obj = obj;
    _method = meth;
  }
  
  virtual ~SpecificPoseMethodFunctor() {}

  void exec(G3D::Array<G3D::PosedModel::Ref> &posedModels, const G3D::CoordinateFrame &virtualToRoomSpace) { 
    (_obj->*_method)(posedModels, virtualToRoomSpace); 
  }

protected:
  T          *_obj;
  MethodType  _method;
};




class DrawMethodFunctor
{
public:
  DrawMethodFunctor() {}
  virtual ~DrawMethodFunctor() {}
  virtual void exec(G3D::RenderDevice *rd, const G3D::CoordinateFrame &virtualToRoomSpace) = 0;
};

template <class T>
class SpecificDrawMethodFunctor : public DrawMethodFunctor
{
public:
  typedef void (T::*MethodType)(G3D::RenderDevice *rd, const G3D::CoordinateFrame &virtualToRoomSpace);

  SpecificDrawMethodFunctor(T *obj, MethodType meth) {
    _obj = obj;
    _method = meth;
  }

  virtual ~SpecificDrawMethodFunctor() {}

  void exec(G3D::RenderDevice *rd, const G3D::CoordinateFrame &virtualToRoomSpace) { 
    (_obj->*_method)(rd, virtualToRoomSpace); 
  }

protected:
  T          *_obj;
  MethodType  _method;
};


} // end namespace

#endif
