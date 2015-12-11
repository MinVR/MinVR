
#include "XMLUtils.h"

#include <iostream>



int numSubstringOccurances(const std::string &str, const std::string &substr) {
  int n = 0;
  std::string s = str;
  while (s.find(substr) != s.npos) {
    int start = s.find(substr);
    s = s.substr(start+1);
    n++;
  }
  return n;
}


int findNth(const std::string &str, const std::string &substr, const int n) {
  std::string s = str;
  int nthIndex = 0;
  for (int i=0;i<n;i++) {
    int index = s.find(substr);
    if (index == s.npos)
      return s.npos;
    s = s.substr(index+1);
    if (i == 0)
      nthIndex = index;
    else
      nthIndex += index+1;
  }
  return nthIndex;
}


std::string trimWhitespace(const std::string &s) {
  const std::string whitespace = " \t\f\v\n\r";
  size_t first = s.find_first_not_of(whitespace);
  size_t last = s.find_last_not_of(whitespace);
  return s.substr(first, (last-first+1));
}


bool XMLUtils::getXMLField(const std::string &input, const std::string &fieldName,
                 std::map<std::string, std::string> &propertiesAndValues,
                 std::string &fieldData,
	               std::string &leftoverInput) {
 
  // Clear out the table of values 
  propertiesAndValues.clear();

  // Look for the beginning of the expected field
  int startfield = input.find("<" + fieldName);
  if (startfield == input.npos) {
    leftoverInput = input;
    return false;
  }
  
  // Get the substring that contains all the attributes of the field
  std::string propplus = input.substr(startfield + fieldName.length() + 1);
  int endofnameandproperties = propplus.find(">");
  std::string properties = propplus.substr(0,endofnameandproperties);


  while ((properties.size()) && (properties.find("=") != properties.npos)) {
    properties = trimWhitespace(properties);
    int equals = properties.find("=");
    std::string name = properties.substr(0,equals);
    bool quoted = false;
    if (properties[equals+1] == '"') {
      quoted = true;
      equals++;
    }
    std::string valplus = properties.substr(equals+1);
    int end = 0;
    if (quoted)
      end = valplus.find("\"");
    else
      end = valplus.find(" ");
    std::string value = valplus.substr(0,end);
    propertiesAndValues.insert(std::make_pair(name,value));
     properties = valplus.substr(end+1);
  }

  // If there is no data, then some XML writers will end the field
  // with a /> rather than the conventional way
  if ((endofnameandproperties-1 >= 0) && (propplus[endofnameandproperties-1] == '/')) {
    leftoverInput = propplus.substr(endofnameandproperties + 1);
  }
  else {

    // Get all the xml text after the end of the field begin carrot
    // symbols everything from here to the matching </myXmlField> is
    // stored in the field data.  This is a little tricky because there
    // may be a subfield with the same fieldName *inside* the field
    // data, so we need to make sure we have the right number of xml
    // field start tokens and field end tokens.
    std::string dataplus = propplus.substr(endofnameandproperties + 1);

    int numstart = 1;
    int numend = 0;
    int endofdata;
    while (numend != numstart) {
      endofdata = findNth(dataplus, "</" + fieldName + ">", numend+1);
      if (endofdata == dataplus.npos) {
        std::cerr << "getXMLField error: Can't find the " << numend+1 <<
          "-th occurance of </" << fieldName << "> in " << dataplus << std::endl;
        return false;
      }
      numend++;
      fieldData = dataplus.substr(0, endofdata);
      numstart = numSubstringOccurances(fieldData, "<" + fieldName + ">") +
        numSubstringOccurances(fieldData, "<" + fieldName + " ") + 1;
    }
    
    leftoverInput = dataplus.substr(endofdata+1);
  }
  
  return true;
}



std::string XMLUtils::writeXMLField(const std::string &fieldName,
                          const std::map<std::string, std::string> &propertiesAndValues,
                          const std::string &fieldData) {
  std::string s;
  s = "<" + fieldName;
  for (std::map<std::string, std::string>::const_iterator it = propertiesAndValues.begin(); it != propertiesAndValues.end(); ++it) {
    s = s + " " + it->first + "=\"" + it->second + "\"";
  }
  s = s + ">" + fieldData + "</" + fieldName + ">\n";
  return s;
}

