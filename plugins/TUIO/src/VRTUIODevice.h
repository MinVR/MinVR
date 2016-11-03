/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0.
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */


#ifndef MINVR_TUIOEVENTS_H
#define MINVR_TUIOEVENTS_H

#include <vector>
#include <set>
#include <input/VRInputDevice.h>
#include <config/VRDataIndex.h>
#include <main/VRMainInterface.h>

#include  <TuioClient.h>

#define TUIO_PORT  3333

namespace MinVR {

class VRTUIODevice : public VRInputDevice
{
public:
	VRTUIODevice(int port = TUIO_PORT, double  xScaleFactor = 1.0, double  yScaleFactor=1.0 );
	virtual ~VRTUIODevice();
	
	void appendNewInputEventsSinceLastCall(VRDataQueue *inputEvents);

	static VRInputDevice* create(VRMainInterface *vrMain, VRDataIndex *config, const std::string &nameSpace);

private:
	TUIO::TuioClient *_tuioClient;
	std::set<int>    _cursorsDown;
	VRDataIndex _dataIndex;
	double      _xScale;
	double      _yScale;

};

}                        // end namespace

#endif                  
