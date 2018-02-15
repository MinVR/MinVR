#ifndef VRINPUTDEVICE_H
#define VRINPUTDEVICE_H

#include <vector>
#include <config/VRDataIndex.h>
#include <config/VRDataQueue.h>

namespace MinVR {

/// \brief Abstract base class for Input Devices.
///
/// Input devices are polled once per frame by VRMain and should
///	return any new events generated since the last call.
///
/// MinVR supports a flexible event interface, where events can have
/// any name and contain any data.  However, to maintain compatibility
/// with other MinVR devices, and so that event handlers can be
/// developed in polynomial time, here is a list of common event types
/// and the data that belong to them.  If you are defining a new input
/// device, and want to support existing software, try to use an event
/// that conforms to the requirements set out here.
///
/// Events themselves are instantiated with the VREvent type, which is
/// just a typedef synonym for VRDataIndex.
///
/// All events will have a "name" and a "type".  The name is held in
/// the VREvent name field, and can be returned with event.getName().
/// The type is a string stored in the event as EventType, just the way
/// other data is stored there.
///   ~~~
///   VREvent event("Head_Move");
///   std::string name = event.getName();
///   event.addData("EventType", "TrackerMove");
///   event.addData("Transform", headMatrix);
///   ~~~
///
/// ## EVENT STANDARDS
///
/// Life will be better for all concerned if you can use one of the
/// event types and names specified here.  Obviously new devices are
/// going to require new names, but these are the names that existing
/// software will expect to see so if you can fit your needs into this
/// framework, it will be compatible with other work.  The list below
/// is of event types, with the event names listed within.
///
/// ### AnalogUpdate
///
/// Use the AnalogUpdate type for analog values, transmitted as VRFloats.
///
/// #### FrameStart
///
/// The FrameStart event is a heartbeat event, issued at regular
/// intervals by the MinVR software, for animation purposes.  It uses
/// the type AnalogUpdate.
///
/// Contains   |    Type    |   What is it?
/// -----------|------------|-------------------
/// AnalogValue| VRFloat    | Number of seconds
/// ElapsedSeconds| VRFloat | Synonym for AnalogValue
///
/// Example:
///   ~~~
///   FrameStart
///   | AnalogValue = 10.598754 (float)
///   | ElapsedSeconds = 10.598754 (float)
///   | EventType = AnalogUpdate (string)
///   ~~~
///
/// ### ButtonDown / ButtonUp
///
/// Use these for keyboard events.  The keyboard events have names
/// like "Kbdr_Down" and "Kbdr_Up" for pressing and releasing the
/// "r" key.
///
/// Contains   |    Type    |   What is it?
/// -----------|------------|-------------------
/// ButtonState| VRInt      | 0 for up, 1 for down.
///
/// ### CursorMove
///
/// Reports the location of a 2D cursor on a screen.
///
/// #### Mouse_Move
///
/// Indicates the mouse moved on a desktop.
///
/// Contains   |    Type    |   What is it?
/// -----------|------------|-------------------
/// Position   | VRFloatArray(2) | mouse position in pixel coordinates
/// NormalizedPosition | VRFloatArray(2) | mouse position with coordinates ranging from 0 to 1
///
///
/// ### TrackerMove
///
/// Provides the 4x4 matrix transform to indicate the pose of a
/// tracker.  This is used in Head_Move events and Wand_Move events.
/// Systems with multiple wands will typically use events called
/// Wand0_Move and Wand1_Move and so on.
///
/// Contains   |    Type    |   What is it?
/// -----------|------------|-------------------
/// Transform  | VRFloatArray(16) | The 4x4 pose matrix, in column-major form.
///
class VRInputDevice {
public:
	virtual ~VRInputDevice() {}
  virtual void appendNewInputEventsSinceLastCall(VRDataQueue* queue) = 0;

	static std::string getAttributeName(){ return "inputdeviceType"; };
};

} // ending namespace MinVR

#endif
