/*
 * Copyright Regents of the University of Minnesota, 2017.  This software is released under the following license: http://opensource.org/licenses/
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */

#ifndef VRSYSTEM_H_
#define VRSYSTEM_H_

#ifdef WIN32
	#include <winsock2.h> // must be included before windows.h
	#include <windows.h>
	#include <ctime> 
#else
	#include <sys/time.h>
#endif

namespace MinVR {

/**
 * VRSystem is a singleton class that holds cross platform system related implementations.  For example the
 * getTime() method will give the number of seconds since initialization.  Calling initialize() will
 * set all the variables necessary for calling the methods.  If initialize() is not called, the system
 * will be set from the first time the instance is used.  For example, the initialize() method will start the
 * timer.
 */
class VRSystem {
public:
	virtual ~VRSystem();

	static void initialize() { instance(); }
	static double getTime(bool absolute = false);

protected:
	static VRSystem& instance();

private:
	VRSystem();

	void initTime();
	double time(bool absolute);

	double initialTime;
#ifdef WIN32
	LARGE_INTEGER timeStart;
	LARGE_INTEGER counterFrequency;
#else
	struct timeval start;
#endif
};

} /* namespace MinVR */

#endif /* VRSYSTEM_H_ */
