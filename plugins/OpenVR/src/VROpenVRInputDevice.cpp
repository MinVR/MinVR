/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Ben Knorlein
 */

#include <ctime>
#include <cctype>

#include "VROpenVRRenderModelHandler.h"
#include "VROpenVRInputDevice.h"
#include "VROpenVRNode.h"
#include "api/VRButtonEvent.h"
#include "api/VRTrackerEvent.h"
#include "api/VRAnalogEvent.h"

namespace MinVR {
                       
	VROpenVRInputDevice::VROpenVRInputDevice(vr::IVRSystem *pHMD, string name, VROpenVRNode * node, float deviceUnitsToRoomUnits, VRMatrix4 deviceToRoom) :m_pHMD(pHMD), m_name(name), m_node(node), deviceUnitsToRoomUnits(deviceUnitsToRoomUnits), deviceToRoom(deviceToRoom)
	{
		updateDeviceNames();

		for (vr::TrackedDeviceIndex_t unDevice = 0; unDevice < vr::k_unMaxTrackedDeviceCount; unDevice++)
		{
			setTipOffset(unDevice);
		}
	}

	VROpenVRInputDevice::~VROpenVRInputDevice() {
	}

void VROpenVRInputDevice::appendNewInputEventsSinceLastCall(VRDataQueue* queue) {
	//process VR Events
	vr::VREvent_t event;
	vr::TrackedDevicePose_t pose;
	while( m_pHMD->PollNextEventWithPose(vr::VRCompositor()->GetTrackingSpace(),  &event, sizeof(event), &pose ) )
	{
		processVREvent( event, &pose );
	}

	//Report current States
	reportAnalog();

    for (int f = 0; f < _events.size(); f++)
	{
    	queue->push(_events[f]);
    }
    _events.clear();
}

void VROpenVRInputDevice::updatePoses(){
	vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

	for (vr::TrackedDeviceIndex_t unDevice = 0; unDevice < vr::k_unMaxTrackedDeviceCount; unDevice++)
	{
		if (m_pHMD->IsTrackedDeviceConnected(unDevice)){
			std::string event_name = getDeviceName(unDevice);
			VRMatrix4 pose = poseToMatrix4(&m_rTrackedDevicePose[unDevice]) * m_tip_offset[unDevice];

			pose(0, 3) *= deviceUnitsToRoomUnits;
			pose(1, 3) *= deviceUnitsToRoomUnits;
			pose(2, 3) *= deviceUnitsToRoomUnits;

			VRDataIndex event = VRTrackerEvent::createValidDataIndex(event_name + "_Move", (deviceToRoom.inverse() * pose).toVRFloatArray());
			_events.push_back(event);
		}
	}
}

VRMatrix4 VROpenVRInputDevice::getPose(int device_idx){
	return poseToMatrix4( &m_rTrackedDevicePose[device_idx]);
}

bool getButtonState(vr::EVRButtonId id, uint64_t state){
	return vr::ButtonMaskFromId(id) & state;
}

std::string VROpenVRInputDevice::getAxisType(int device, int axis){
	int32_t type = m_pHMD->GetInt32TrackedDeviceProperty( device, ((vr::ETrackedDeviceProperty) (vr::Prop_Axis0Type_Int32 +  axis)));
	switch(type){
		case vr::k_eControllerAxis_None:
			return "None" + to_string(axis);
		case vr::k_eControllerAxis_TrackPad:
			return "TrackPad" + to_string(axis);
		case vr::k_eControllerAxis_Joystick:
			return "Joystick" + to_string(axis);
		case vr::k_eControllerAxis_Trigger:
			return "Trigger" + to_string(axis);
	}
	return "Invalid";
}

void VROpenVRInputDevice::reportAnalog()
{
	for (vr::TrackedDeviceIndex_t unDevice = 0; unDevice < vr::k_unMaxTrackedDeviceCount; unDevice++)
	{
		if (m_pHMD->IsTrackedDeviceConnected(unDevice))
		{
			std::string event_name = getDeviceName(unDevice);
			vr::VRControllerState_t state;
			m_pHMD->GetControllerState(unDevice, &state, sizeof(state));
			for (int axis = 0; axis < vr::k_unControllerStateAxisCount; axis++){
				int32_t type = m_pHMD->GetInt32TrackedDeviceProperty(unDevice, ((vr::ETrackedDeviceProperty) (vr::Prop_Axis0Type_Int32 + axis)));
				if (type == vr::k_eControllerAxis_Trigger)
				{
					VRDataIndex di = VRAnalogEvent::createValidDataIndex(event_name + "_" + getAxisType(unDevice, axis), state.rAxis[axis].x);
					_events.push_back(di);
				}
				else if (type == vr::k_eControllerAxis_TrackPad || type == vr::k_eControllerAxis_Joystick)
				{
					VRDataIndex dix = VRAnalogEvent::createValidDataIndex(event_name + "_" + getAxisType(unDevice, axis) + "_X", state.rAxis[axis].x);
					_events.push_back(dix);

					VRDataIndex diy = VRAnalogEvent::createValidDataIndex(event_name + "_" + getAxisType(unDevice, axis) + "_Y", state.rAxis[axis].y);
					_events.push_back(diy);
				}	
			}
		}
	}
}

std::string VROpenVRInputDevice::getDeviceName(int idx){
	if (devices[idx] == "")
	{
		//this should never happen. Update deviceNames
		updateDeviceNames();
	}
	
	return devices[idx];
}

void VROpenVRInputDevice::updateDeviceNames()
{
	devices.clear();
	int hmd_count = 0;
	int controller_count = 0;
	int trackingReference_count = 0;
	int unknown_count = 0;
	for (vr::TrackedDeviceIndex_t unDevice = 0; unDevice < vr::k_unMaxTrackedDeviceCount; unDevice++)
	{
		if (m_pHMD->IsTrackedDeviceConnected(unDevice))
		{
			std::string entry;
			vr::TrackedDeviceClass deviceClass = m_pHMD->GetTrackedDeviceClass(unDevice);
			if (deviceClass == vr::TrackedDeviceClass_HMD)
			{
				hmd_count++;
				entry = m_name + "_HMD_" + std::to_string(hmd_count);
			}
			else if (deviceClass == vr::TrackedDeviceClass_Controller){
				controller_count++;
				vr::ETrackedControllerRole role = m_pHMD->GetControllerRoleForTrackedDeviceIndex(unDevice);
				switch (role){
				case vr::TrackedControllerRole_Invalid:
				default:// Invalid value for controller type
					entry = m_name + "_Controller_" + std::to_string(controller_count);
					break;
				case vr::TrackedControllerRole_LeftHand:	
					entry = m_name + "_Controller_Left";
					break;
				case vr::TrackedControllerRole_RightHand:
					entry = m_name + "_Controller_Right";
					break;	
				}
			}
			else if (deviceClass == vr::TrackedDeviceClass_TrackingReference){
				trackingReference_count++;
				entry = m_name + "_TrackingReference_" + std::to_string(trackingReference_count);
			}
			else{
				unknown_count++;
				entry = m_name + "_UnknownDevice_" + std::to_string(unknown_count);
			}
			devices.push_back(entry);
		}
		else
		{
			devices.push_back("");
		}
	}
}

std::string  VROpenVRInputDevice::getButtonName(vr::EVRButtonId id){
	switch( id )
	{
		case vr::k_EButton_System:
			return "SystemButton";
		case vr::k_EButton_ApplicationMenu:
			return "ApplicationMenuButton";
		case vr::k_EButton_Grip:
			return "GripButton";
		case vr::k_EButton_DPad_Left:
			return "DPad_LeftButton";
		case vr::k_EButton_DPad_Up:
			return "DPad_UpButton";
		case vr::k_EButton_DPad_Right:
			return "DPad_RightButton";
		case vr::k_EButton_DPad_Down:
			return "DPad_DownButton";
		case vr::k_EButton_A:
			return "AButton";
		case vr::k_EButton_Axis0:
			return "Axis0Button";
		case vr::k_EButton_Axis1:
			return "Axis1Button";
		case vr::k_EButton_Axis2:
			return "Axis2Button";
		case vr::k_EButton_Axis3:
			return "Axis3Button";
		case vr::k_EButton_Axis4:
			return "Axis4Button";
	}
	return "Invalid";
}

void VROpenVRInputDevice::processVREvent( const vr::VREvent_t & event ,vr::TrackedDevicePose_t *pose)
{	
	switch( event.eventType )
	{
		case vr::VREvent_TrackedDeviceActivated:
		{
			updateDeviceNames(); 
			if (m_node->getRenderModelHandler())
				m_node->getRenderModelHandler()->queueModelForLoading(event.trackedDeviceIndex);
			setTipOffset(event.trackedDeviceIndex);
			std::cerr << "Device " << getDeviceName(event.trackedDeviceIndex) << " idx : " << event.trackedDeviceIndex << " attached.\n" << std::endl;		
		}
		break;
	case vr::VREvent_TrackedDeviceDeactivated:
		{
			std::cerr << "Device " << getDeviceName( event.trackedDeviceIndex) << " detached.\n" << std::endl;
			updateDeviceNames();
		}
		break;
	case vr::VREvent_TrackedDeviceUpdated:
		{
			std::cerr << "Device " << getDeviceName(event.trackedDeviceIndex) << " updated.\n" << std::endl;
			updateDeviceNames();
		}
		break;
	case vr::VREvent_ButtonPress :
		{							
			std::string event_name = getDeviceName(event.trackedDeviceIndex) + "_" + getButtonName((vr::EVRButtonId) event.data.controller.button) + "_Down";
			VRDataIndex di = VRButtonEvent::createValidDataIndex(event_name, 1);
			_events.push_back(di);
		}
		break;
	case vr::VREvent_ButtonUnpress:
		{
			std::string event_name = getDeviceName(event.trackedDeviceIndex) + "_" + getButtonName((vr::EVRButtonId) event.data.controller.button) + "_Up";
			VRDataIndex di = VRButtonEvent::createValidDataIndex(event_name, 0);
			_events.push_back(di);
		}
		break;
	case vr::VREvent_ButtonTouch:
		{
			std::string event_name = getDeviceName(event.trackedDeviceIndex) + "_" + getButtonName((vr::EVRButtonId) event.data.controller.button) + "_Touch";
			VRDataIndex di = VRButtonEvent::createValidDataIndex(event_name, 3);
			_events.push_back(di);
		}
		break;
	case vr::VREvent_ButtonUntouch:
		{
			std::string event_name = getDeviceName(event.trackedDeviceIndex) + "_" + getButtonName((vr::EVRButtonId) event.data.controller.button) + "_Untouch";
			VRDataIndex di = VRButtonEvent::createValidDataIndex(event_name, 4);
			_events.push_back(di);
		}
		break;
	}
}

VRMatrix4 VROpenVRInputDevice::poseToMatrix4(vr::TrackedDevicePose_t *pose)
{
	if(!pose->bPoseIsValid)
		return VRMatrix4();

	
	VRMatrix4 mat = VRMatrix4::fromRowMajorElements(
		pose->mDeviceToAbsoluteTracking.m[0][0], pose->mDeviceToAbsoluteTracking.m[1][0], pose->mDeviceToAbsoluteTracking.m[2][0], 0.0, 
		pose->mDeviceToAbsoluteTracking.m[0][1], pose->mDeviceToAbsoluteTracking.m[1][1], pose->mDeviceToAbsoluteTracking.m[2][1], 0.0,
		pose->mDeviceToAbsoluteTracking.m[0][2], pose->mDeviceToAbsoluteTracking.m[1][2], pose->mDeviceToAbsoluteTracking.m[2][2], 0.0,
		pose->mDeviceToAbsoluteTracking.m[0][3], pose->mDeviceToAbsoluteTracking.m[1][3], pose->mDeviceToAbsoluteTracking.m[2][3], 1.0f
		);

	return mat.transpose();
}

	void VROpenVRInputDevice::setTipOffset(int device)
	{
		if (m_pHMD->IsTrackedDeviceConnected(device) && (m_pHMD->GetTrackedDeviceClass(device) == vr::TrackedDeviceClass_Controller))
		{
			char *pchBuffer = new char[10000];
			unsigned int  name_length = 10000;
			m_pHMD->GetStringTrackedDeviceProperty(device, vr::Prop_RenderModelName_String, pchBuffer, name_length, NULL);

			vr::VRControllerState_t c_state;
			if (m_pHMD->GetControllerState(device, &c_state, sizeof(c_state))){
				vr::RenderModel_ControllerMode_State_t statemode;
				statemode.bScrollWheelVisible = false;
				vr::RenderModel_ComponentState_t component_state;

				if (vr::VRRenderModels()->GetComponentState(pchBuffer, "tip", &c_state, &statemode, &component_state))
				{
					m_tip_offset[device] = poseToMatrix4(component_state.mTrackingToComponentLocal.m);
				}

			}
		}
	}

	VRMatrix4 VROpenVRInputDevice::poseToMatrix4(float m[3][4])
{
	VRMatrix4 mat = VRMatrix4::fromRowMajorElements(
		m[0][0], m[1][0], m[2][0], 0.0, 
		m[0][1], m[1][1], m[2][1], 0.0,
		m[0][2], m[1][2], m[2][2], 0.0,
		m[0][3], m[1][3], m[2][3], 1.0f
		);

	return mat.transpose();
}


} /* namespace MinVR */


