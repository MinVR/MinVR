/**
  Adapted from VRG3D/vrbase/StringUtils.[H,cpp], originally written 2004-01-28 by Dan Keefe, Brown University
  Reinterpreted for MinVR2 2015-11-11 by Dan Keefe, University of Minnesota
*/


#ifndef XMLUTILS_H
#define XMLUTILS_H

#include <string>
#include <sstream>
#include <map>


/***** Utilities for Parsing and Writing Data in a Simplified XML Syntax *****/
class XMLUtils {
public:

  ///  Here's my interpretation of XML for the mini parser and generator below:
  ///  <fieldname1 property1="value1" property2="value2">fielddata1, fielddata2
  ///  </fieldname1>
  ///  <fieldname2> <subfield1>blah</subfield1> </fieldname2>
  ///  etc.. 
  ///
  ///  getXMLField(..) returns datastructures based on this description.
  ///  Returns false if the fieldName is not found in input.  If there
  ///  is more than one occurance of a field named fieldName in input,
  ///  returns data from the first one.  leftOverInput is everything in 
  ///  input that comes after the end of the field.
  static bool getXMLField(const std::string &input, const std::string &fieldName,
                   std::map<std::string, std::string> &propertiesAndValues,
                   std::string &fieldData,
                   std::string &leftoverInput);


  /// fielddata can include a call to writeXMLField to make a subfield, this
  /// gets formatted as described in the getXMLField() function description.
  static std::string writeXMLField(const std::string &fieldName,
              const std::map<std::string, std::string> &propertiesAndValues,
              const std::string &fieldData);


  /// This function is used to convert from a string type to something
  /// else.  The new type is inferred using a template and it must define
  /// the input stream operator >> for this function to work properly.
  template <class T>
  static bool retypeString(const std::string &str, T &val) {
    std::istringstream is(str.c_str());
    is >> val;
    if (!is) return false;
    else return true;
  }


  static std::string intToString(int i) {
    std::ostringstream os;
    os << i;
    return os.str();
  }

  
};



#endif
