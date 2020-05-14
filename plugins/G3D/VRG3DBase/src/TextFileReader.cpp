
#include "../include/TextFileReader.H"

namespace MinVR {

using namespace G3D;

bool
TextFileReader::getTextUntilToken(const std::string &stopToken, std::string &readText, bool eatToken)
{
  while (true) {
    int pos = textBuffer.find(stopToken);
    if (pos != textBuffer.npos) {
      readText = textBuffer.substr(0, pos);
      textBuffer = textBuffer.substr(pos);

      if (eatToken) {
        if (textBuffer.length() <= stopToken.length()) {
          textBuffer = std::string("");
        }
        else {
          textBuffer = textBuffer.substr(stopToken.length());
        }
      }
      return true;
    }
    else if (eof()) {
      return false;
    }
    else {
      getMoreFromFile();
    }
  }
  return false;
}

bool 
TextFileReader::peekAtTextUntilToken(const std::string &stopToken, std::string &readText) 
{
  while (true) {
    int pos = textBuffer.find(stopToken);
    if (pos != textBuffer.npos) {
      readText = textBuffer.substr(0, pos);
      return true;
    }
    else if (eof()) {
      return false;
    }
    else {
      getMoreFromFile();
    }
  }
  return false;
}


bool
TextFileReader::getXMLHeader(std::string &fieldName,
    Table<std::string, std::string> &propertiesAndValues)
{
  // Clear out the table of values 
  propertiesAndValues.clear();

  // the next XML token should be the start of a field
  TextFileReader::XMLTokenType type;
  peekAtNextXMLToken(type);
  if(type == TextFileReader::XMLEndField){
	  std::string throwAway;
	  getTextUntilToken("<", throwAway, true);
	  int bracePos = textBuffer.find(">");
	  fieldName = textBuffer.substr(0,bracePos);
	  textBuffer = textBuffer.substr(bracePos+1);
	  return true;
  }

  if(type == TextFileReader::XMLError){
	  fieldName = "End of file Reached";
	  return true;
  }
  alwaysAssertM(type == TextFileReader::XMLStartField, "Expected start of XML field");

  // Look for the beginning of the field
  std::string throwAway;
  if (!getTextUntilToken("<", throwAway, true)) {
    return false;
  }

  // Read in the field name which ends in either a space or a > character
  int spacePos = textBuffer.find(" ");
  int bracePos = textBuffer.find(">");
  if ((spacePos == textBuffer.npos) && (bracePos == textBuffer.npos)) {
    getMoreFromFile();
    spacePos = textBuffer.find(" ");
    bracePos = textBuffer.find(">");
  }
  if ((spacePos == textBuffer.npos) && (bracePos == textBuffer.npos)) {
    alwaysAssertM(false, "Can't find end of fieldname.");
  }
  int endOfName = iMinNonNeg(spacePos, bracePos);

  fieldName = textBuffer.substr(0,endOfName);
  textBuffer = textBuffer.substr(endOfName+1);

  if (spacePos == endOfName) {
    // We have properties..

    // Get the substring that contains all the properties of the field
    std::string properties;
    if (!getTextUntilToken(">", properties, true)) {
      return false;
    }

    //cout << "properties=" << properties << endl;
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
      propertiesAndValues.set(name,value);
      properties = valplus.substr(end+1);
    }
  }

  return true;
}


//  enum XMLTokenType { XMLStartField, XMLEndField, XMLFieldData, XMLError }

bool 
TextFileReader::peekAtNextXMLToken(XMLTokenType &type)
{
  std::string nextText;
  if (!peekAtTextUntilToken("<", nextText)) {
    type=XMLError;
    return true;
  }

  // see if the text before the < character contains anything but whitespace
  bool onlywhitespace = true;
  int i=0;
  while ((i<nextText.length()) && (onlywhitespace)) {
    if ((nextText[i] != ' ') && (nextText[i] != '\t') && (nextText[i] != '\r') && (nextText[i] != '\n')) {
      onlywhitespace = false;
    }
    i++;
  }

  //debugAssert(textBuffer.length() > nextText.length()+1);
  if (textBuffer.length() <= nextText.length()+1) {
    // What's going on here????
    //cout << "nt=" << nextText << endl;
    //cout << "tb=" << textBuffer << endl;
  }
  bool gotslash = (textBuffer[nextText.length()+1] == '/');
  
  if ((gotslash) && (onlywhitespace)) {
    type = XMLEndField;
  }
  else if ((gotslash) && (!onlywhitespace)) {
    type = XMLFieldData;
  }
  else if ((!gotslash) && (onlywhitespace)) {
    type = XMLStartField;
  }
  else if ((!gotslash) && (!onlywhitespace)) {
    type = XMLError;
    return false;
  }
  return true;
}

bool 
TextFileReader::peekAtNextXMLStartFieldName(std::string &fieldName)
{
  std::string nextText;
  if (!peekAtTextUntilToken("<", nextText)) {
    return false;
  }

  int start = nextText.length() + 1;
  std::string namePlus = textBuffer.substr(start);

  int spacePos = namePlus.find(" ");
  int bracePos = namePlus.find(">");
  if ((spacePos == namePlus.npos) && (bracePos == namePlus.npos)) {
    getMoreFromFile();
    namePlus = textBuffer.substr(start);
    spacePos = namePlus.find(" ");
    bracePos = namePlus.find(">");
  }
  if ((spacePos == namePlus.npos) && (bracePos == namePlus.npos)) {
    alwaysAssertM(false, "Can't find end of fieldname.");
  }
  int endOfName = iMinNonNeg(spacePos, bracePos);

  fieldName = namePlus.substr(0,endOfName);
  return true;
}

bool 
TextFileReader::getXMLFieldData(const std::string &fieldName, std::string &fieldData)
{
  if (!getTextUntilToken("</" + fieldName + ">", fieldData, false)) {
    return false;
  }
  return true;
}

bool 
TextFileReader::eatXMLFieldClose(const std::string &fieldName)
{
  std::string throwAway;
  if (!getTextUntilToken("</" + fieldName + ">", throwAway, true)) {
    return false;
  }
  return true;
}


bool
TextFileReader::getXMLField(std::string &fieldName, 
                            Table<std::string, std::string> &propertiesAndValues, 
                            std::string &fieldData)
{
  if (!getXMLHeader(fieldName, propertiesAndValues)) {
    alwaysAssertM(false, "Error getting XML header for field " + fieldName);
    return false;
  }

  TextFileReader::XMLTokenType type;
  if (!peekAtNextXMLToken(type)) {
    alwaysAssertM(false, "Error peeking at next XML token for field " + fieldName);
    return false;
  }

  if (type == TextFileReader::XMLFieldData) {
    if (!getXMLFieldData(fieldName, fieldData)) {
      alwaysAssertM(false, "Error getting XML field data for field " + fieldName);
      return false;
    }
  }
  
  if (!eatXMLFieldClose(fieldName)) {
    alwaysAssertM(false, "Error eating close of XML field for field " + fieldName);
    return false;
  }
  return true;
}


} // end namespace

