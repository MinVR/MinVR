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

#ifndef VREVENTINTERNAL_H_
#define VREVENTINTERNAL_H_


#include <api/VREvent.h>
#include <config/VRDataIndex.h>
#include <map>


namespace MinVR {

/** Provides access to secondary and raw event data that go beyond that 
    available via the VREvent interface.  VREvent is intended to be 
    sufficient for 90% of use cases; the VREventInternal class is useful
    for the 10% of users who require low-level access for research projects
    and other advanced cases.
*/
class VREventInternal {
public:

	/** Constructs a VREventInternal and a corresponding VREvent based on the event name and data provided.
	*/
	VREventInternal(const std::string &name, VRDataIndex *dataIndex);

    ~VREventInternal();

  
    /** Returns the name of the event itself, not to be confused with the event's
        payload of named data fields.
     */
    std::string getName() { return _name; }
  
  
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

  
    /** Returns the type for the named data field using the enumerated type
        defined in VREvent.
     */
    VREvent::DataType getDataType(const std::string &fieldName) const;
  
  
    /** Returns a pointer to the streamlined VREvent data type associated with this VREventInternal.
    */
    VREvent* getAPIEvent();

    /** Returns a pointer to the input device that generated this event.
    */
    //VRInputDevice *getInputDevice();


    /** Returns a pointer to the underlying raw data index used to implement the VREventInternal. 
    */
	VRDataIndex* getDataIndex();

private:

    std::string  _name;
    std::string  _indexName;
	VREvent      _event;
	VRDataIndex* _dataIndex;
    std::vector<std::string> _fieldNames;

    std::map<std::string,int>     _intMap;
    std::map<std::string,float>  _floatMap;

    std::map<std::string,char*>   _charArrayMap;
    std::map<std::string,int>     _charArraySizes;

    std::map<std::string,int*>    _intArrayMap;
    std::map<std::string,int>     _intArraySizes;

    std::map<std::string,float*> _floatArrayMap;
    std::map<std::string,int>     _floatArraySizes;
};


} // end namespace

#endif
