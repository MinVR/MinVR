/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Ben Knorlein
 */

#ifndef VROPENVRINPUTDEVICE_H_
#define VROPENVRINPUTDEVICE_H_


#include <map>
#include <vector>
#include <string>

#include <openvr.h>

#include <config/VRDataQueue.h>
#include <input/VRInputDevice.h>
#include <plugin/VRPlugin.h>
#include <math/VRMath.h>

namespace MinVR {

	class VROpenVRRenderModels;

/** A VRInputDevice that polls input events 
 */
class VROpenVRInputDevice : public VRInputDevice {
public:
	PLUGIN_API VROpenVRInputDevice(vr::IVRSystem *pHMD, std::string name);
	PLUGIN_API virtual ~VROpenVRInputDevice();

	PLUGIN_API void appendNewInputEventsSinceLastCall(VRDataQueue* queue);
 
	PLUGIN_API void updatePoses();
	PLUGIN_API VRMatrix4 getPose(int device_idx);
 
private:
	std::vector<std::string> _events;
	VRDataIndex _dataIndex;
    std::vector<int > _windows;
	vr::IVRSystem *m_pHMD;
	std::vector<std::string> devices;
	vr::TrackedDevicePose_t m_rTrackedDevicePose[ vr::k_unMaxTrackedDeviceCount ];
	std::string m_name;
	void processVREvent( const vr::VREvent_t & event ,vr::TrackedDevicePose_t *pose);
	void reportStates();
	VRMatrix4 poseToMatrix4(vr::TrackedDevicePose_t *pose);
	std::string getDeviceName(int idx);
	void updateDeviceNames();
	std::string getButtonName(vr::EVRButtonId id);
	std::string getAxisType(int device, int axis);
};


} /* namespace MinVR */

#endif /* VROPENVRINPUTDEVICE_H_ */
