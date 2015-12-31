/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRRENDERER_H_
#define VRRENDERER_H_

namespace MinVR {

class VRRenderer {
public:
	virtual ~VRRenderer() {}

	virtual void render() const = 0;
};

class VRRendererFunctor : public VRRenderer {
public:
	typedef void (*MethodType)();

	VRRendererFunctor(MethodType method) : method(method) {}
	virtual ~VRRendererFunctor() {}

	void render() const;

private:
	MethodType method;
};

template<class T>
class SpecificVRRenderer : public VRRenderer {
public:
	typedef void (T::*MethodType)() const;

	SpecificVRRenderer(T *obj, MethodType method);
	virtual ~SpecificVRRenderer() {}

	void render() const;

private:
	T *obj;
	MethodType method;
};

//---------------------------------

inline void VRRendererFunctor::render() const
{
	(*method)();
}

template<class T>
SpecificVRRenderer<T>::SpecificVRRenderer(T *obj, MethodType method) : obj(obj), method(method)
{
}

template<class T>
void SpecificVRRenderer<T>::render() const
{
	(obj->*method)();
}

} /* namespace MinVR */

#endif /* VRRENDERER_H_ */
