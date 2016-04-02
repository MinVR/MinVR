#include "config/VRDatum.h"

// If a 'make test' shows a test failing, try 'ctest -VV' to get more
// detail.  This will also print out whatever goes to std::cout.

// These tests confirm the setting and retrieving of data in the
// correct type, test the VRDatumConverter shortcut, and test that the
// type information is correct.
int testDatumInt();
int testDatumDouble();
int testDatumString();
int testDatumIntArray();
int testDatumDoubleArray();
int testDatumStringArray();
int testDatumPushPop();
//int testDatumContainer();

// Mucks around with the attribute lists.
int testDatumAttributes();

// You can make this long to get better timing data.
#define LOOP for (int loopctr = 0; loopctr < 10; loopctr++)

// This is the primary test runner.  Accepts two args, selects one of
// the tests to run, runs it, returns the result.  It is operated by
// the CMake-generated RunSomeIndexTests binary.  See the
// CMakeLists.txt for where that is generated, but it's pretty much
// CMake magic.
int datumtest(int argc, char* argv[]) {
  
  int defaultchoice = 1;
  
  int choice = defaultchoice;

  if (argc > 1) {
    if(sscanf(argv[1], "%d", &choice) != 1) {
      printf("Couldn't parse that input as a number\n");
      return -1;
    }
  }

  int output;

  // When you add a case here, run it by adding a number to the
  // sequence in this directory's CMakeLists.txt file.
  switch(choice) {
  case 1:
    output = testDatumInt();
    break;

  case 2:
    output = testDatumDouble();
    break;

  case 3:
    output = testDatumString();
    break;

  case 4:
    output = testDatumIntArray();
    break;

  case 5:
    output = testDatumDoubleArray();
    break;

  case 6:
    output = testDatumStringArray();
    break;

  case 7:
    output = testDatumAttributes();
    break;

  case 8:
    output = testDatumPushPop();
    break;
    
    // Add case statements to handle values of 2-->10
  default:
    std::cout << "Test #" << choice << " does not exist!\n";
    output = -1;
  }
  
  return output;
}

// Test the setting and retrieving of attributes.
int testDatumAttributes() {

  int out = 0;

  LOOP {
    std::string testString = " four=\"4\" one=\"1\" three=\"3\" two=\"2\"";
  
    VRDatumInt a = VRDatumInt(37);
  
    VRDatum::VRAttributeList alist = a.getAttributeList();

    alist["one"] = "11";
    alist["two"] = "2";
    alist["three"] = "3";

    a.setAttributeList(alist);

    a.setAttributeValue("one", "1");
    a.setAttributeValue("four", "4");

    std::string outString = a.getAttributeListAsString();

    //  std::cout << outString << std::endl;
    //  std::cout << testString << std::endl;
  
    out += outString.compare(testString);

  }
  
  return out;
}


// Test setting and retrieving values.
int testDatumInt() {

  int out = 0;

  LOOP {
  
    VRDatumInt a = VRDatumInt(37);

    // Is the type description correct?
    out = a.getDescription().compare("int");
    
    // Also implicitly testing the VRDatumConverter here.
    int b = a.getValue();
  
    out += (37 == b) ? 0 : 1;

    // Is the type correct?
    out += (a.getType() == VRCORETYPE_INT) ? 0 : 1;

    // Is the serialization correct?
    out += a.getValueAsString().compare("37");
  }
    
  return out;
}

// See testDatumInt() above.
int testDatumDouble() {

  int out = 0;

  LOOP {
    VRDatumDouble a = VRDatumDouble(37.123);

    out = a.getDescription().compare("double");

    double b = a.getValue();
  
    out += (37.123 == b) ? 0 : 1;
  
    out += (a.getType() == VRCORETYPE_DOUBLE) ? 0 : 1;

    out += a.getValueAsString().compare("37.123000");
    // std::cout << a.getValueAsString() << std::endl;
  }
    
  return out;
}

// See testDatumInt() above.
int testDatumString() {

  int out = 0;

  LOOP {
  
    VRDatumString a = VRDatumString(std::string("this is a string"));

    out = a.getDescription().compare("string");

    std::string b = a.getValue();

    std::string c = "this is a string";
  
    out += c.compare(b);

    out += (a.getType() == VRCORETYPE_STRING) ? 0 : 1;

    out += a.getValueAsString().compare("this is a string");
  }
  
  return out;
}

// See testDatumInt() above.
int testDatumIntArray() {

  int out;

  LOOP {
    int someInts[] = {16,2,77,29};
    std::vector<int> f (someInts, someInts + sizeof(someInts) / sizeof(int) );
  
    VRDatumIntArray a = VRDatumIntArray(f);
    
    out = a.getDescription().compare("intarray");

    std::vector<int> b = a.getValue();
  
    out += (77 == b[2]) ? 0 : 1;

    out += (a.getType() == VRCORETYPE_INTARRAY) ? 0 : 1;

    out += a.getValueAsString().compare("16@2@77@29");
  }
  
  return out;
}

// See testDatumInt() above.
int testDatumDoubleArray() {

  int out;

  LOOP {
    double someDoubles[] = {16.2,2.71828,77.3,29.165};
    std::vector<double> f (someDoubles, someDoubles + sizeof(someDoubles) / sizeof(double) );
 
    VRDatumDoubleArray a = VRDatumDoubleArray(f);

    out = a.getDescription().compare("doublearray");

    std::vector<double> b = a.getValue();
    
    out += (b[2] == 77.3) ? 0 : 1;

    out += (a.getType() == VRCORETYPE_DOUBLEARRAY) ? 0 : 1;

    a.setAttributeValue("separator", ",");
  
    //std::cout << a.getValueAsString() << std::endl;
    out += a.getValueAsString().compare("16.200000,2.718280,77.300000,29.165000");
  }
  
  return out;
}

// See testDatumInt() above.
int testDatumStringArray() {

  int out = 0;

  LOOP {
  
    std::vector<std::string> f;

    f.push_back("one");
    f.push_back("two");
    f.push_back("three");
    f.push_back("four");
    f.push_back("five");
    
    VRDatumStringArray a = VRDatumStringArray(f);

    out = a.getDescription().compare("stringarray");

    std::vector<std::string> b = a.getValue();
  
    out += b[2].compare("three");

    out += (a.getType() == VRCORETYPE_STRINGARRAY) ? 0 : 1;

    a.setAttributeValue("separator", "/");
  
    out += a.getValueAsString().compare("one/two/three/four/five");
  }
  
  return out;
}

int testDatumPushPopInt() {

  int out = 0;

  LOOP {

    VRInt f = 5;
    VRDatumInt a = VRDatumInt(f);

    a.push();
    a.setValue(37);
    VRInt g = a.getValue();
    out += (37 == g) ? 0 : 1;

    a.pop();
    VRInt h = a.getValue();
    out += (5 == h) ? 0 : 1;
  }
  
  return out;
}

int testDatumPushPopDouble() {

  int out = 0;

  LOOP {

    VRDouble f = 3.1415926;
    VRDatumDouble a = VRDatumDouble(f);

    a.push();
    a.setValue(2.71828);
    VRDouble g = a.getValue();
    out += (2.71828 == g) ? 0 : 1;

    a.pop();
    VRDouble h = a.getValue();
    out += (3.1415926 == h) ? 0 : 1;
  }
  
  return out;
}

int testDatumPushPopString() {

  int out = 0;

  LOOP {

    VRString f = "hello";
    VRDatumString a = VRDatumString(f);

    a.push();
    a.setValue("good-bye");
    VRString g = a.getValue();
    out += g.compare("good-bye");

    a.pop();
    VRString h = a.getValue();
    out += h.compare("hello");
  }
  
  return out;
}

int testDatumPushPop() {

  int out = 0;

  out += testDatumPushPopInt();
  out += testDatumPushPopDouble();
  out += testDatumPushPopString();

  return out;  
}
