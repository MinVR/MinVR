/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRDISPLAYACTION_H_
#define VRDISPLAYACTION_H_

namespace MinVR {

class VRDisplayAction {
public:
	virtual ~VRDisplayAction() {}

	virtual void exec() const = 0;
};

class VRDisplayActionFunctor : public VRDisplayAction {
public:
	typedef void (*MethodType)();

	VRDisplayActionFunctor(MethodType method) : method(method) {}
	virtual ~VRDisplayActionFunctor() {}

	void exec() const;

private:
	MethodType method;
};

template<class T>
class SpecificVRDisplayAction : public VRDisplayAction {
public:
	typedef void (T::*MethodType)() const;

	SpecificVRDisplayAction(T *obj, MethodType method);
	virtual ~SpecificVRDisplayAction() {}

	void exec() const;

private:
	T *obj;
	MethodType method;
};

//---------------------------------

inline void VRDisplayActionFunctor::exec() const
{
	(*method)();
}

template<class T>
SpecificVRDisplayAction<T>::SpecificVRDisplayAction(T *obj, MethodType method) : obj(obj), method(method)
{
}

template<class T>
void SpecificVRDisplayAction<T>::exec() const
{
	(obj->*method)();
}

} /* namespace MinVR */

#endif /* VRDISPLAYACTION_H_ */
