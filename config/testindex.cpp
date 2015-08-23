#include <map>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include "Cxml.h"
#include "element.h"
#include "MinVRDataIndex.h"

int main() {
  MinVRDataIndex *index = new MinVRDataIndex;

  int a = 4;
  int b = 6;
  double f = 3.1415926;
  double g = 2.71828;
  std::string s1 = std::string("wowie!");
  std::string s2 = std::string("shazam!");

  index->addValueInt(std::string("henry"), a);
  index->addValueInt(std::string("ralph"), b);

  index->addValueDouble(std::string("george"), f);
  index->addValueDouble(std::string("mary"), g);

  index->addValueString(std::string("billy"), s1);
  index->addValueString(std::string("johnny"), s2);

  std::string c1 = std::string("container1");
  std::string c2 = std::string("container2");

  index->addValueContainer(std::string("ted"), c1);
  index->addValueContainer(std::string("eva"), c2);


  index->addValue(std::string("<flora type=\"int\">3274</flora><morton type=\"float\">34.5</morton>"));

  std::string input = "";

  while(true) {
    std::cout << "Give a name: ";
    getline(std::cin, input);
    std::cout << "You entered:->" << input << "<-" << std::endl;

    MinVRDatumPtr p = index->getValue(input);

    switch (p->getType()) {
    case MVRINT:
      std::cout << "which gives->" << (p.intVal()->getValue()) << std::endl;
      break;

    case MVRFLOAT:
      std::cout << "which gives->" << (p.doubleVal()->getValue()) << std::endl;
      break;

    case MVRSTRING:
      std::cout << "which gives (S)->" << (p.stringVal()->getValue()) << std::endl;
      break;

    case MVRCONTAINER:
      std::cout << "which gives (C)->" << (p.containerVal()->getValue()) << std::endl;
      break;
    }


    std::cout << "description: " << index->getDescription(input) << std::endl;
    std::cout << "serialization: " << index->serialize(input) << std::endl;
  }

}
