/** 
This file is part of the MinVR Open Source Project, which is developed and 
maintained collaboratively by the University of Minnesota and Brown University.

Copyright (c) 2016 Regents of the University of Minnesota and Brown University.
This software is distributed under the BSD-3 Clause license, which can be found
at: MinVR/LICENSE.txt.

Original Author(s) of this File: 
	Dan Keefe, 2016, University of Minnesota
	
Author(s) of Significant Updates/Modifications to the File:
	...
*/

#ifndef VREVENT_H_
#define VREVENT_H_


#include <string>
#include <vector>


namespace MinVR {

class VREventInternal; // forward declaration needed for MinVR's implementation



/** MinVR events are defined by a name stored as a std::string.  Sometimes
    this is all you need to know about an event.  For example, an
    event named "WandLeftBtn_Down" tells you all you need to know.

    In your code, you might do something like the following:

	const std::string brushDownEventName = "WandLeftBtn_Down";

	void onVREvent(const VREvent &event) {
		if (event->getName() == brushDownEventName) {
			// start painting
		}
	}

	An easy way to make your code portable from one VR environment to another
	is to simply set brushDownEventName to something different based on the
	current environment.  If your application supports a config file or 
	command line options or some other way of configuring the program, then
	use it to set brushDownEventName = "WandLeftBtn_Down" when you are running in
	a Cave with a Wand or set brushDownEventName = "KeyB_Down" when running on
	the desktop.  In MinVR, all events intentionally have the same type (i.e., no 
	separate class for keyboard events vs. wand button events vs. multi-touch 
	events) in order to make it easy to switch from one hardware configuration
	to another without making changes to the event handling code.

	Many events also carry data with them.  The primary data value associated
	with each event can be accessed using the getData*() methods.

	Advanced users can refer to VREventAdvanced for more complex needs, 
    including access to any extended, secondary data fields that may be 
    stored with the event.
*/
class VREvent {
public:

	/** Every event has a name.  The naming convention is to include 
	    a descriptive name first, then an underscore, then a keyword to 
	    denote the type of event, for example, "WandLeftBtn_Down".
	    To denote the type of event, the following conventions are used:
	    For buttons:  "Down" or "Up".  For trackers, joysticks, mice, and 
	    other pointing devices: "Move".  For cameras: "Refresh". 
	*/
	std::string getName() const;


	/** Returns the names of all data fields stored within the current event. 
	*/
	std::vector<std::string> getDataFields() const;


	/** Returns the data stored in the named data field interpreted as an int.  
	    If the key is not found then a 0 is returned.  Example use:

		int id = event->getDataAsInt("TouchID");
	*/
	int getDataAsInt(const std::string &fieldName) const;

	/** Returns the data stored in the named data field interpreted as a float.  
	    If the key is not found then a 0.0 is returned.  Example use:

		float speed = event->getDataAsFloat("Speed");
	*/
	float getDataAsFloat(const std::string &fieldName) const;

	/** Returns the data stored in the named data field interpreted as a char
	    array.  If the key is not found then "\0" is returned.  Example use:

		char *name = event->getDataAsCharArray("Name");

		Memory is managed by the VREvent object.  The pointer is valid as long 
		as the VREvent object is valid.
	*/
	const char * getDataAsCharArray(const std::string &fieldName) const;

    /** Returns the size of the array for the named char array data field */
    int getCharArraySize(const std::string &fieldName) const;
  
	/** Returns the data stored in the named data field interpreted as an int array.
	    If the key is not found then NULL is returned.  Example use:

		int xy[] = event->getDataAsIntArray("Position");
		if (xy != NULL) {
			cout << "(" << xy[0] << "," << xy[1] << ")" << endl;
		}

		Memory is managed by the VREvent object.  The pointer is valid as long 
		as the VREvent object is valid.
	*/
	const int * getDataAsIntArray(const std::string &fieldName) const;

    /** Returns the size of the array for the named int array data field */
    int getIntArraySize(const std::string &fieldName) const;

	/** Returns the data stored in the named data field interpreted as a float array.
	    If the key is not found then NULL is returned.  Example use:

		float quat[] = event->getDataAsFloatArray("Quaternion");
		if (quat != NULL) {
			cout << "<" << quat[0] << "," << quat[1] << "," << quat[2] << "," << quat[3] << ">" << endl;
		}

		Memory is managed by the VREvent object.  The pointer is valid as long 
		as the VREvent object is valid.
	*/
	const float * getDataAsFloatArray(const std::string &fieldName) const;

    /** Returns the size of the array for the named float array data field */
    int getFloatArraySize(const std::string &fieldName) const;
  
	/** Enumerated type for the builtin data types used in MinVR to store event data
	*/
	enum DataType {IntData, FloatData, CharArrayData, IntArrayData, FloatArrayData};

	/** Returns the type for the named data field using an enumerated type.
	*/
	DataType getDataType(const std::string &fieldName) const;

  
    /** Prints all data fields and data to stdout.
     */
    void print() const;
  

	/** Returns a pointer to the raw data type used internally by MinVR to
		create this VREvent.  Advanced users may use this pointer to access
		raw event data and input device information not exposed by this API.
	*/
	VREventInternal* getInternal() const;


private:

	/* Users should never create a VREvent directly.  VREventInternal provides a mechanism
	   for advanced users to create their own custom events if needed.
	*/
	VREvent(VREventInternal *internalEvent); // Used by friend class VREventInternal
	VREvent(); // private to prevent creating VREvents directly
	VREvent(const VREvent&); // private to prevent copies, which should be handed by VREventInternal

	friend class VREventInternal;

	VREventInternal *_internal;
};


} // end namespace

#endif
