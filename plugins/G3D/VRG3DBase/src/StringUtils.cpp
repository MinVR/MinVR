
#include "../include/StringUtils.H"

using namespace G3D;


#define BIGNUMBER 9999
int 
iMinNonNeg(int i1, int i2) 
{
  if (i1 < 0)
    i1 = BIGNUMBER;
  if (i2 < 0)
    i2 = BIGNUMBER;

  if (i1 < i2)
    return i1;
  else
    return i2;
}


std::string
joinIntoString( const Array<std::string>& in, const std::string& delimiter )
{
  std::string s;
  if ( in.size() ) {
    s = in[0];
    for ( int i = 1; i < in.size(); i++ )
      s = s + delimiter + in[i];
  }
  return s;
}



Array< Array< std::string > >
readDelimitedData(const std::string &csvString, const std::string &delimiter,
                  bool removeQuotes)
{
  Array < Array< std::string> > data;
  std::string csv;

  // first remove '\r' characters to make DOS/Windows style returns
  // look like unix
  for (int i=0;i<csvString.length();i++) {
    if (csvString[i] != '\r')
      csv = csv + csvString[i];
  }

  bool done=false;
  while ((!done) && (csv.length())) {
    int nl = csv.find("\n");
    int l = csv.length();
    int endofline = iMinNonNeg(nl, l);
    if (endofline > 0) {
      std::string line = csv.substr(0,endofline);
      Array<std::string> dataLine;
      
      while (line.length() > 0) {
        int end = iMinNonNeg(line.find(delimiter), line.length());
        std::string item = line.substr(0,end);
        if ((removeQuotes) && (item[0] == '"') && (item[item.length()-1] == '"'))
          item = item.substr(1,item.length()-2);
        dataLine.append(item);
        if (end+1 >= line.length()) {
          if (line[line.length()-1] == ',')
            dataLine.append("");
          line = "";
        }
        else
          line = line.substr(end+1);
      }

      if (dataLine.size())
        data.append(dataLine);
      
      csv = csv.substr(endofline+1);
    }
    else
      done = true;
  }
  
  return data;
}


bool
popUntilSemicolon(std::string &in, std::string &popped)
{
  int semi = in.find(";");
  if (semi == in.npos)
    return false;
  else {
    popped = in.substr(0,semi);
    in = in.substr(semi+1);
    return true;
  }
}



bool 
popNextToken(std::string &in, std::string &token, bool returnFalseOnSemiColon)
{
  in = trimWhitespace(in);

  // if no more tokens, return false
  if (in.size() == 0) {
    return false;
  }
  else if ((in[0] == ';') && (returnFalseOnSemiColon)) {
    in = in.substr(1);
    return false;
  }

  int end = in.size();
  end = iMinNonNeg(end, in.find(" "));
  end = iMinNonNeg(end, in.find('\t'));
  end = iMinNonNeg(end, in.find(";"));
  end = iMinNonNeg(end, in.find(","));
  end = iMinNonNeg(end, in.find('\n'));
  end = iMinNonNeg(end, in.find('\r'));

  token = in.substr(0,end);
  in = in.substr(end);
  return (token.size() > 0);
}

std::string 
convertNewlinesAndTabsToSpaces(std::string input) 
{
  int i = 0;
  while (i < (int)input.size()) {
    if ((input[i] == '\r') || (input[i] == '\n') || (input[i] == '\t'))
      input[i] = ' ';
    i++;
  }
  return input;
}


std::string 
spacesString(int num)
{
  std::string s;
  for (int i=0;i<num;i++)
    s = s + " ";
  return s;
}


int
numSubstringOccurances(const std::string &str, const std::string &substr)
{
  int n = 0;
  std::string s = str;
  while (s.find(substr) != s.npos) {
    int start = s.find(substr);
    s = s.substr(start+1);
    n++;
  }
  return n;
}


int
findNth(const std::string &str, const std::string &substr, const int n)
{
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


bool  
getXMLField(const std::string &input, const std::string &fieldName,
	    Table<std::string, std::string> &propertiesAndValues,
	    std::string &fieldData,
	    std::string &leftoverInput)
{
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
    propertiesAndValues.set(name,value);
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
        cout << "getXMLField error: Can't find the " << numend+1 <<
          "-th occurance of </" << fieldName << "> in " << dataplus << endl;
        return false;
      }
      //debugAssert(endofdata != dataplus.npos);
      numend++;
      fieldData = dataplus.substr(0, endofdata);
      numstart = numSubstringOccurances(fieldData, "<" + fieldName + ">") +
        numSubstringOccurances(fieldData, "<" + fieldName + " ") + 1;
    }
    
    leftoverInput = dataplus.substr(endofdata+1);
  }
  
  return true;
}



std::string  
writeXMLField(const std::string &fieldName,
	      const Table<std::string, std::string> &propertiesAndValues,
	      const std::string &fieldData)
{
  std::string s;
  s = "<" + fieldName;
  Array<std::string> keys = propertiesAndValues.getKeys();
  for (int i=0;i<keys.size();i++) {
    s = s + " " + keys[i] + "=\"" + propertiesAndValues[keys[i]] + "\"";
  }
  s = s + ">" + fieldData + "</" + fieldName + ">\n";
  return s;
}


char**
stringArrayTo2DCharArray(const Array<std::string> &a)
{
  int n = a.size();

  if (n == 0)
    return NULL;

  cout << "n=" << n << endl;

  char **ptr;
  ptr = new char * [n];
  for (int i=0;i<n;i++) {
    //ptr[i] = new char[strlen(a[i].c_str())+1];
    ptr[i] = new char[1024];
    strcpy(ptr[i], a[i].c_str());
    cout << i << " " << ptr[i] << endl;
  }
  return ptr;
}


void
delete2DCharArray(char **ptr, int size)
{
  for (int i=0;i<size;i++)
    delete [] ptr[i];
  delete [] ptr;
}

