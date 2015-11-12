
#include <iostream>

#include <data/VRDataIndex.h>
#include <event/VREvent.h>
#include <math/VRMath.h>


int main(int argc, char **argv) {

  
  // fill a data index with an example for each of the VRCoreTypes
  
  VRDataIndex e1Data;
  // VRInt
  e1Data.addData("MyInteger", 1);

  // VRDouble
  e1Data.addData("MyFloat", 2.2);

  // VRString
  e1Data.addData("MyString", "test string!");

  // VRIntArray
  VRIntArray ia;
  ia.push_back(1);
  ia.push_back(2);
  ia.push_back(3);
  e1Data.addData("MyIntArray", ia);

  // VRDoubleArray
  VRDoubleArray da;
  da.push_back(1.23);
  da.push_back(2.34);
  da.push_back(3.45);
  e1Data.addData("MyDoubleArray", da);

  // VRStringArray
  VRStringArray sa;
  sa.push_back("one fish");
  sa.push_back("two fish");
  sa.push_back("red fish");
  sa.push_back("blue fish");
  e1Data.addData("MyStringArray", sa);

  // create a new event with this data index
  VREvent e1("ExampleEvent", e1Data);
  std::cout << "Event 1:" << e1.toXML() << std::endl;
  

  // create a second event with data from the first, testing the Event::to/fromXML methods
  VREvent e2 = VREvent::fromXML(e1.toXML());
  std::cout << "Event 2:" << e2.toXML() << std::endl;
  

  if (e1.toXML() == e2.toXML()) {
    std::cout << "Test of Event::toXML() and Event::fromXML() successful!" << std::endl;
  }
  else {
    std::cout << "Error: e1.toXML() is not equal to e2.toXML() -- could be a problem with Event::toXML(), Event::fromXML(), or VRDataIndex" << std::endl;
  }

  // Test reading the data out into a new variable, using the long notation
  VRDoubleArray myDA = e2.getDataIndex()->getDatumPtr("MyDoubleArray", "/")->getValueDoubleArray();
  std::cout << e2.getDataIndex()->getTypeAsString("MyDoubleArray", "/") << std::endl;
  for (VRDoubleArray::iterator it=myDA.begin(); it<myDA.end(); ++it) {
    std::cout << *it << std::endl;
  }

  
  // Test reading the data out into a new variable, here VRAnyCoreType gets returned by getValue and is then cast into a VRDoubleArray
  VRDoubleArray myDA2 = e2.getDataIndex()->getValue("MyDoubleArray");
  for (VRDoubleArray::iterator it=myDA2.begin(); it<myDA2.end(); ++it) {
    std::cout << *it << std::endl;
  }
  
  
  
  std::cout << std::endl;

  
  
  // now test types from VRMath
  
  VRDataIndex e3Data;
  VRPoint3 p(1,2,3);
  // At its heart, VRPoint3 is really just a 3 element array of doubles, so it's easy to
  // convert the data to a VRDoubleArray, which is one of the VRCoreTypes accepted by VRDataIndex.
  e3Data.addData("Position", p.toVRDoubleArray());
  
  // Alternatively, since VRPoint3 inherets from VRDoubleArrayConvertable, we can call addData
  // directly on p as a shorthand.
  e3Data.addData("Position2", p);
  
  
  VRVector3 v(1.2, 2.3, 3.4);
  e3Data.addData("Heading", v);
  VRMatrix4 m = VRMatrix4::translation(VRVector3(5.5, 6.6, 7.7)) * VRMatrix4::scale(VRVector3(2.0, 2.0, 2.0));
  e3Data.addData("TransformMatrix", m);
  
  // check to see that toXML and fromXML work as expected...
  
  VREvent e3("EventWithMathTypes", e3Data);
  std::cout << "Event 3: " << e3.toXML() << std::endl;
  
  VREvent e4 = VREvent::fromXML(e3.toXML());
  std::cout << "Event 4: " << e4.toXML() << std::endl;
  
  
  // now test reading some data out of the dataIndex and getting it into the right VRMath type

  // note: getValue returns VRAnyCoreType and then the VRPoint3(VRAnyCoreType) constructor is
  // called, where VRAnyCoreType is interpreted as the core type VRDoubleArray and then the
  // point is created.
  VRPoint3 p2 = e4.getDataIndex()->getValue("Position");
  std::cout << "Position = " << p2 << std::endl;
  
  VRVector3 h = e4.getDataIndex()->getValue("Heading");
  std::cout << "Heading = " << h << std::endl;
  
  VRMatrix4 m2 = e4.getDataIndex()->getValue("TransformMatrix");
  std::cout << "TransformMatrix = " << m2 << std::endl;
  
  
  
  
  
  
  
  /*** OLD APPROACH - PRE VRDataIndex
  
  // EVENTS FOR ARBITRARY TYPES:

  // Using the templated constructor for VREvent, we can create an event with any arbitrary data object
  // as long as that object implements the method:  void serialize(ByteData &data);
  VRVector3 v1(1,2,3);
  std::cout << v1 << std::endl;
  VREvent e1("event 1", v1);

  // To get the data out, we can use the templated getData method:
  VRVector3 v2(e1.getData());
  std::cout << v2 << std::endl;

  // Or, if the type implements a constructor that takes a ByteData argument, then we can get the data this way.
  // Here, there is an implicit call to the constructor: Vec3(const ByteData &data);
  VRVector3 v3 = e1.getData();
  std::cout << v3 << std::endl;


  // EVENTS FOR BUILTIN TYPES:

  // Note: we could skip the asInt() or asFloat() syntax below if we overload the cast () operator in ByteData in order
  // to automatically cast it to an int or float, but I think it might be better to avoid this.  I can see bugs coming
  // up from confusion over implicit casts to floats vs. doubles for example.

  VREvent e2("event 2", 2);
  int i = e2.getData().toInt();
  std::cout << "i = " << i << std::endl;


  VREvent e3("event 3", 123456789);
  int j = e3.getData().toInt();
  std::cout << "j = " << j << std::endl;


  VREvent e4("event 4", (float)1.23456789);
  float f = e4.getData().toFloat();
  std::cout << "f = " << f << std::endl;


  // Use a ByteStream to serialize a more complex datastructure through several write/serialize calls.
  VRByteStream bs;
  bs.writeInt(5);
  bs.writeInt(3);
  v3.serialize(bs);
  bs.writeInt(1);

  std::cout << bs.readInt() << " " << bs.readInt() << std::endl;
  VRVector3 v4;
  v4.deserialize(bs);
  std::cout << v4 << std::endl;
  std::cout << bs.readInt() << std::endl;

  // Anythign that can be packed into ByteData can be used as a data type for events
  VREvent e5("event 5", bs.toByteData());
  
  VRByteStream bs2(e5.getData());
  std::cout << bs2.readInt() << " " << bs2.readInt() << std::endl;
  VRVector3 v5;
  v5.deserialize(bs2);
  std::cout << v5 << std::endl;
  std::cout << bs2.readInt() << std::endl;
   
   
  ***/
}

