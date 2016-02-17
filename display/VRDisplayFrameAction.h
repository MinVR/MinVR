/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRFRAMEACTION_H_
#define VRFRAMEACTION_H_

namespace MinVR {

class VRDisplayFrameAction {
public:
	virtual ~VRDisplayFrameAction() {}

	virtual bool exec() = 0;
};

class VRDisplayFrameActionFunctor : public VRDisplayFrameAction {
public:
	typedef bool (*MethodType)();

	VRDisplayFrameActionFunctor(MethodType method) : method(method) {}
	virtual ~VRDisplayFrameActionFunctor() {}

	bool exec();

private:
	MethodType method;
};

template<class T>
class SpecificVRDisplayFrameAction : public VRDisplayFrameAction {
public:
	typedef bool (T::*MethodType)();

	SpecificVRDisplayFrameAction(T *obj, MethodType method);
	virtual ~SpecificVRDisplayFrameAction() {}

	bool exec();

private:
	T *obj;
	MethodType method;
};

//---------------------------------

inline bool VRDisplayFrameActionFunctor::exec()
{
	return (*method)();
}

template<class T>
SpecificVRDisplayFrameAction<T>::SpecificVRDisplayFrameAction(T *obj, MethodType method) : obj(obj), method(method)
{
}

template<class T>
bool SpecificVRDisplayFrameAction<T>::exec()
{
	return (obj->*method)();
}

} /* namespace MinVR */

#endif /* VRFRAMEACTION_H_ */
