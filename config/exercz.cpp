#include <map>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include "Cxml.h"
#include "element.h"
#include "MinVRDataIndex.h"

int main() {
  MinVRDataIndex *index = new MinVRDataIndex;

  // Set up some sample data names and values.
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

  index->addValue(std::string("<bob type=\"container\"><flora type=\"int\">3274</flora><morton type=\"float\">34.5</morton></bob>"));

  std::vector<std::string> elems;
  std::string elem;
  std::string line;
  std::cout << "Try ? for commands." << std::endl;
  while ((std::cout << "> ") && std::getline(std::cin, line)) {

    elems.clear();
    std::stringstream ss(line);
    while (std::getline(ss, elem, ' ')) {
      elems.push_back(elem);
    }

    ////// command: ? (help)
    if (elems[0].compare("?") == 0) {
      std::cout << "l get the list of data names" << std::endl;
      std::cout << "p <name> print a value from the list" << std::endl;
      std::cout << "f <filename> read an XML file" << std::endl;
      std::cout << "a <newname> <type> <value> add a primitive value" << std::endl;
      std::cout << "q quit" << std::endl;

    ////// command: f (open file)
    } else if (elems[0].compare("f") == 0) {

      index->processXMLFile(elems[1]);

    ////// command: p (print value)
    } else if (elems[0].compare("p") == 0) {

      try {
        MinVRDatumPtr p = index->getValue(elems[1]);

        switch (p->getType()) {
        case MVRINT:
          std::cout << "an integer containing: " << (p.intVal()->getValue()) << std::endl;
          break;

        case MVRFLOAT:
          std::cout << "a float containing: " << (p.doubleVal()->getValue()) << std::endl;
          break;

        case MVRSTRING:
          std::cout << "a string containing: " << (p.stringVal()->getValue()) << std::endl;
          break;

        case MVRCONTAINER:

          {
            std::cout << "a container containing: " << std::endl;

            std::list<std::string> nameList = p.containerVal()->getValue();
            for (std::list<std::string>::iterator nl = nameList.begin();
                 nl != nameList.end(); nl++) {
              std::cout << "                        " << *nl << std::endl;
            }
            break;
          }
        }

        std::cout << "description: " << index->getDescription(elems[1]) << std::endl;
        std::cout << "serialization: " << index->serialize(elems[1]) << std::endl;
      } catch (const std::exception& e) {

        std::cout << "oops: " << e.what() << std::endl;

      }

    ////// command: l (list all values)
    } else if (elems[0].compare("l") == 0) {
      std::list<std::string> nameList = index->getDataNames();
      for (std::list<std::string>::iterator it = nameList.begin();
           it != nameList.end(); it++) {
        std::cout << *it << std::endl;
      }
    ////// command: a (something)
    } else if (elems[0].compare("a") == 0) {

      if (elems.size() < 4) {
        std::cout << "try 'a harry int 27' (that is, do 'a <name> <type> <value>')" << std::endl;
      } else {
        std::string serialized = "<" + elems[1] + " type=\"" + elems[2] + "\">" + elems[3] + "</" + elems[1] + ">";
        index->addValue(serialized);
      }

    ////// command: q (exit)
    } else if (elems[0].compare("q") == 0) {
      return EXIT_SUCCESS;

    } else {

      std::cout << "Was '" << elems[0] << "' meant to be a command? Try '?'." << std::endl;
    }
  }
}

