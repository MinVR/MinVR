#include <map>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include "VRDataIndex.h"
#include "VRDataQueue.h"

#define HELPMESSAGE  std::cout << "ls get the list of data names" << std::endl; \
      std::cout << "px - show namespaces / containers " << std::endl; \
      std::cout << "p <name> print a value from the list" << std::endl; \
      std::cout << "file <filename> read an XML file" << std::endl; \
      std::cout << "add <newname> <type> <value> add a primitive value" << std::endl; \
      std::cout << "add <name> container   - create a container" << std::endl; \
      std::cout << "cd <namespace> set a default namespace" << std::endl; \
      std::cout << "q quit" << std::endl; \
      std::cout << "? print this message." << std::endl;

// This is just a test program meant to be a vague illustration of the
// use of the VRDataIndex type system.  The program is essentially
// a really dumb interpreter with a few commands implemented, such as
// look at a program, read a data file, add a data value and change
// the namespace.  Use the operation of the program to learn about
// namespaces and use the code below as a way to learn how to use the
// data index. It's just hacked together, so don't read too much into
// it.
int main() {
  VRDataIndex *index = new VRDataIndex;
  VRDataQueue *queue = new VRDataQueue;

  // Set up some sample data names and values.
  int a = 4;
  int b = 6;
  double f = 3.1415926;
  double g = 2.71828;
  std::string s1 = std::string("wowie!");
  std::string s2 = std::string("shazam!");

  index->addValue(std::string("/henry"), a);
  index->addValue(std::string("/ralph"), b);

  index->addValue(std::string("/george"), f);
  index->addValue(std::string("/mary"), g);

  index->addValue(std::string("/billy"), s1);
  index->addValue(std::string("/johnny"), s2);

  index->addSerializedValue(std::string("<bob type=\"container\"><flora type=\"int\">3274</flora><morton type=\"double\">34.5</morton><cora type=\"container\"><flora type=\"int\">1234</flora><nora type=\"double\">23.45</nora></cora></bob>"));

  index->addSerializedValue(std::string("<chester type=\"arraydouble\">32.7@44.56@22.3@78.2@99.134@</chester>"));

  std::vector<std::string> elems;
  std::string elem;
  std::string line;
  std::string nameSpace("/");

  queue->push(index->getDatum("billy", nameSpace)->serialize());
  queue->push(index->getDatum("george", nameSpace)->serialize());
  queue->printQueue();

  HELPMESSAGE

  while ((std::cout << "> ") && std::getline(std::cin, line)) {

    elems.clear();
    std::stringstream ss(line);
    while (std::getline(ss, elem, ' ')) {
      elems.push_back(elem);
    }

    ////// command: ? (help)
    if (elems[0].compare("?") == 0) {
      HELPMESSAGE

    ////// command: file (open file)
    } else if (elems[0].compare("file") == 0) {

      index->processXMLFile(elems[1]);

    ////// command: cd (set namespace)
    } else if (elems[0].compare("cd") == 0) {

      if (elems.size() > 1) {

        nameSpace = index->validateNameSpace(elems[1]);

      } else {
        std::cout << "using namespace: " << nameSpace << std::endl;
      }

    ////// command: px
    } else if (elems[0].compare("px") == 0) {

      index->printWholeKitAndKaboodle();

    ////// command: z (undocumented in help; use for testing)
    } else if (elems[0].compare("z") == 0) {

      std::cout << elems.size() << std::endl;

      int N = elems.size();

      while (N >= 1)
        {

          std::vector<std::string> nms(&elems[1], &elems[N]);
          std::string out;
          for (std::vector<std::string>::iterator it = nms.begin();
               it != nms.end(); ++it)
            {
              out += "/" + *it;
            }
          std::cout << ">" << out << "<" << std::endl;
          N -= 1;
        }


    ////// command: p (print value)
    } else if (elems[0].compare("p") == 0) {

      try {
        VRDatumPtr p = index->getDatum(elems[1], nameSpace);

        switch (p->getType()) {
        case MVRINT:
          std::cout << "an integer containing: " << ((int)p->getValue()) << std::endl;

          std::cout << "same as: " << (int)index->getValue(elems[1], nameSpace) << std::endl;
          break;

        case MVRDOUBLE:
          std::cout << "a double containing: " << ((double)p->getValue()) << std::endl;
          break;

        case MVRSTRING:
          std::cout << "a string containing: " << ((std::string)p->getValue()) << std::endl;
          break;

        case MVRARRAYDOUBLE:
          {
            MVRArrayDouble pdata = p->getValue();
            for (MVRArrayDouble::iterator it = pdata.begin(); it != pdata.end(); ++it) {
              std::cout << "element: " << *it << std::endl;
            }
            break;
          }

        case MVRCONTAINER:

          {
            std::cout << "a container containing: " << std::endl;

            MVRContainer nameList = p->getValue();
            for (MVRContainer::iterator nl = nameList.begin();
                 nl != nameList.end(); nl++) {
              std::cout << "                        " << *nl << std::endl;
            }
            break;
          }
        }

        std::cout << "description: " << index->getDescription(elems[1], nameSpace) << std::endl;
        std::cout << "serialization: " << index->serialize(elems[1], nameSpace) << std::endl;
      } catch (const std::exception& e) {

        std::cout << "oops: " << e.what() << std::endl;

      }

    ////// command: l (list all values)
    } else if (elems[0].compare("ls") == 0) {
      MVRContainer nameList = index->getDataNames(nameSpace);
      for (MVRContainer::iterator it = nameList.begin();
           it != nameList.end(); it++) {
        std::cout << *it << std::endl;
      }
    ////// command: add (add something)
    } else if (elems[0].compare("add") == 0) {

      if ((elems.size() == 4) || (elems.size() == 3)) {

        // Make a list of data values to add to the parent container.
        // In this case, the list will have only one value, but it
        // could have more.
        MVRContainer c;
        c.push_back(nameSpace + elems[1]);

        std::cout << "nameSpace: " << nameSpace << " c: " << c.front() << std::endl;

        if (elems[2].compare("container") == 0) {
          index->addValue(nameSpace + elems[1]);
        } else {

          if (elems[2].compare("int") == 0) {

            int iVal;
            sscanf(elems[3].c_str(), "%d", &iVal);
            // Add the value to the index.
            index->addValue(nameSpace + elems[1], iVal);
            // Add it to the parent container.
            if (nameSpace.size() > 1) index->addValue(nameSpace.substr(0,nameSpace.size() - 1), c);

          } else if (elems[2].compare("double") == 0) {

            double dVal;
            sscanf(elems[3].c_str(), "%lf", &dVal);
            index->addValue(nameSpace + elems[1], dVal);
            if (nameSpace.size() > 1) index->addValue(nameSpace.substr(0,nameSpace.size() - 1), c);

          } else if (elems[2].compare("string") == 0) {

            index->addValue(nameSpace + elems[1], elems[3]);
            if (nameSpace.size() > 1) index->addValue(nameSpace.substr(0,nameSpace.size() - 1), c);

          } else {

            std::cout << "can't handle that data type." << std::endl;

          }
        }
        //        std::string serialized = "<" + elems[1] + " type=\"" + elems[2] + "\">" + elems[3] + "</" + elems[1] + ">";
        //        index->addSerializedValue(serialized, nameSpace);
      } else {
        std::cout << "try 'add harry int 27' (that is, do 'add <name> <type> <value>' \n or 'add <name> container')" << std::endl;
      }

    ////// command: q (exit)
    } else if (elems[0].compare("q") == 0) {
      return EXIT_SUCCESS;

    } else {

      std::cout << "Was '" << elems[0] << "' meant to be a command? Try '?'." << std::endl;
    }
  }
}

