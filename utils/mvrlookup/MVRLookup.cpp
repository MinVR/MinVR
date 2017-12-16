#include <map>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <config/VRDataIndex.h>
#include <config/VRDataQueue.h>

// This program takes a config file, and a data name, and prints out
// the value corresponding to that name on standard output.  If the
// value is an array, print out the nth entry in the array if n is
// specified, the 0th otherwise.  It does not print the \n at the end
// of a line, so is suitable to use in a script.  To return the value
// of /MVR/RootNode/XPos in the Desktop.xml file, use it like this:
//
//  $ echo `MVRLookup Desktop.xml /MVR/RootNode/XPos`
//
// For an array value, try one of these:
//
//  $ echo `MVRLookup Desktop.xml /MVR/RootNode/HeadMatrix 0`
//  $ echo `MVRLookup Desktop.xml /MVR/RootNode/HeadMatrix 1`
//  $ echo `MVRLookup Desktop.xml /MVR/RootNode/HeadMatrix 2`
// 
inline void printHelpMessage() {
  std::cerr << "" << std::endl;
  std::cerr << "usage: MVRLookup file.xml vname [ n ]" << std::endl;  
  std::cerr << "  file.xml - a file containing a VRDataIndex" << std::endl; 
  std::cerr << "  vname    - the name of the datum to retrieve" << std::endl;  
  std::cerr << "  n        - if vname is an array, return the nth entry (default: 0)" << std::endl;
  std::cerr << "" << std::endl;
  std::cerr << "Parses the XML file and returns the value of the variable specified" << std::endl;
  std::cerr << "with the vname argument.  If the value is an array, you can include" << std::endl;
  std::cerr << "an index, n.  If no index value is provided, 0 is assumed." << std::endl;

}

int doTheRealWork(int argc, char **argv) {

  MinVR::VRDataIndex *index = new MinVR::VRDataIndex;

  // These will be extracted from argv.
  int nth = 0;

  // Extracting...
  switch(argc) {

  case 1:
    printHelpMessage();
    return EXIT_SUCCESS;
    
  case 2:
    if (argv[1][0] == '-') {
      printHelpMessage();
      return EXIT_SUCCESS;
    }
    throw std::runtime_error("Need a data name.  (Try --usage.)");
    break;

  case 3:
    // Don't really need to do anything here except avoid throwing error.
    break;
    
  case 4:
    // If there is a fourth arg, hope it's an integer.
    sscanf(argv[3], "%d", &nth);
    break;
    
  default:
    throw std::runtime_error("Too many arguments");
    break;
  }

  index->processXMLFile(argv[1], std::string("/"));

  MinVR::VRCORETYPE_ID tp = index->getType(argv[2]);

  // std::cout << "datumName:" << datumName << " N:" << nth << " tp:" << tp << std::endl;

  // Now print out the values, according to the data type.
  switch(tp) {

  case MinVR::VRCORETYPE_INT: 
    std::cout << (MinVR::VRInt)index->getValue(argv[2]);
    break;

  case MinVR::VRCORETYPE_FLOAT: 
    std::cout << (MinVR::VRFloat)index->getValue(argv[2]);
    break;

  case MinVR::VRCORETYPE_STRING:
    std::cout << (MinVR::VRString)index->getValue(argv[2]);
    break;

  case MinVR::VRCORETYPE_INTARRAY: {
    MinVR::VRIntArray ia = index->getValue(argv[2]);
    if (nth >= (int)ia.size())
      throw std::runtime_error("N too large for array");
    std::cout << ia[nth];
    break;
  }
    
  case MinVR::VRCORETYPE_FLOATARRAY: {
    MinVR::VRFloatArray ia = index->getValue(argv[2]);
    if (nth >= (int)ia.size())
      throw std::runtime_error("N too large for array");
    std::cout << ia[nth];
    break;
  }

  case MinVR::VRCORETYPE_STRINGARRAY: {
    MinVR::VRStringArray ia = index->getValue(argv[2]);
    if (nth >= (int)ia.size())
      throw std::runtime_error("N too large for array");
    std::cout << ia[nth];
    break;
  }

  case MinVR::VRCORETYPE_CONTAINER:
    std::cout << index->serialize(argv[2]);
    break;
    
  default:
    throw std::runtime_error("Not supported for that type.");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
    

int main(int argc, char **argv) {

  try {

    return doTheRealWork(argc, argv);

  } catch (const std::exception& e) {

    std::cerr << "Oopsy-daisy: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
