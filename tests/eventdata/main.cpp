
#include "VREvent.h"
#include "VRMath.h"
#include "VREngine.h"
#include "VRScene.h"
#include <iostream>


class MyVRScene : public VRScene {
public:
  MyVRScene() : frame(0) {}
  void drawGraphics() {
    std::cout << frame << ":  Drawing graphics..." << std::endl;
    frame++;
  }
private:
  int frame;
};


int main(int argc, char **argv) {

  // EVENTS FOR ARBITRARY TYPES:

  // Using the templated constructor for VREvent, we can create an event with any arbitrary data object
  // as long as that object implements the method:  void serialize(ByteData &data);
  Vec3 v1(1,2,3);
  v1.print();
  VREvent e1("event 1", v1);

  // To get the data out, we can use the templated deserializeData method:
  Vec3 v2(e1.getData());
  v2.print();

  // Or, if the type implements a constructor that takes a ByteData argument, then we can get the data this way.
  // Here, there is an implicit call to the constructor: Vec3(const ByteData &data);
  Vec3 v3 = e1.getData();
  v3.print();


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
  ByteStream bs;
  bs.writeInt(5);
  bs.writeInt(3);
  v3.serialize(bs);
  bs.writeInt(1);

  std::cout << bs.readInt() << " " << bs.readInt() << std::endl;
  Vec3 v4;
  v4.deserialize(bs);
  v4.print();
  std::cout << bs.readInt() << std::endl;

  // Anythign that can be packed into ByteData can be used as a data type for events
  VREvent e5("event 5", bs.toByteData());
  
  ByteStream bs2(e5.getData());
  std::cout << bs2.readInt() << " " << bs2.readInt() << std::endl;
  Vec3 v5;
  v5.deserialize(bs2);
  v5.print();
  std::cout << bs2.readInt() << std::endl;
  

  VREngine *vrEngine = VREngine::fromCommandLineOptions(argc, argv);
  VRScene *currentScene = new MyVRScene();
  while (!currentScene->readyToClose()) {
    vrEngine->mainloop(currentScene);    
  }
  vrEngine->shutdown();
  delete vrEngine;  
}

