/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRDISPLAYACTION_H_
#define VRDISPLAYACTION_H_

#include "display/VRRenderState.h"

namespace MinVR {



class VRRenderer {
public:
	virtual ~VRRenderer() {}

	virtual void render(VRRenderState& state) const = 0;
};

class VRRendererFunctor : public VRRenderer {
public:
	typedef void (*MethodType)(VRRenderState&);

	VRRendererFunctor(MethodType method) : method(method) {}
	virtual ~VRRendererFunctor() {}

	void render(VRRenderState& state) const;

private:
	MethodType method;
};

template<class T>
class SpecificVRRenderer : public VRRenderer {
public:
	typedef void (T::*MethodType)(VRRenderState&) const;

	SpecificVRRenderer(T *obj, MethodType method);
	virtual ~SpecificVRRenderer() {}

	void render(VRRenderState& state) const;

private:
	T *obj;
	MethodType method;
};

//---------------------------------

inline void VRRendererFunctor::render(VRRenderState& state) const
{
	(*method)(state);
}

template<class T>
SpecificVRRenderer<T>::SpecificVRRenderer(T *obj, MethodType method) : obj(obj), method(method)
{
}

template<class T>
void SpecificVRRenderer<T>::render(VRRenderState& state) const
{
	(obj->*method)(state);
}

} /* namespace MinVR */

#endif /* VRDISPLAYACTION_H_ */
