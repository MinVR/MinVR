#include "config/VRDatum.h"

// If a 'make test' shows a test failing, try 'ctest -VV' to get more
// detail.  This will also print out whatever goes to std::cout.
// Use 'ctest -VV -R datum_1' or something like that to execute only
// one test.

// These tests confirm the setting and retrieving of data in the
// correct type, test the VRDatumConverter shortcut, and test that the
// type information is correct.
int testDatumInt();
int testDatumFloat();
int testDatumString();
int testDatumIntArray();
int testDatumFloatArray();
int testDatumStringArray();
int testDatumPushPop();
int testDatumContainer();
int testDatumArrayConversions();

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
    output = testDatumFloat();
    break;

  case 3:
    output = testDatumString();
    break;

  case 4:
    output = testDatumIntArray();
    break;

  case 5:
    output = testDatumFloatArray();
    break;

  case 6:
    output = testDatumStringArray();
    break;

  case 7:
    output = testDatumAttributes();
    break;

  case 8:
    output = testDatumContainer();
    break;

  case 9:
    output = testDatumPushPop();
    break;

  case 10:
    output = testDatumArrayConversions();
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

    MinVR::VRDatumInt a = MinVR::VRDatumInt(37);

    MinVR::VRDatum::VRAttributeList alist = a.getAttributeList();

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

    MinVR::VRDatumInt a = MinVR::VRDatumInt(37);

    // Is the type description correct?
    out = a.getDescription().compare("int");

    // Also implicitly testing the VRDatumConverter here.
    int b = a.getValue();

    out += (37 == b) ? 0 : 1;

    const int* pb = a.getValue();
    out += (37 == (*pb)) ? 0 : 1;

    float x = a.getValue();
    out += (x == 37.00) ? 0 : 1;

    // Is the type correct?
    out += (a.getType() == MinVR::VRCORETYPE_INT) ? 0 : 1;

    // Is the serialization correct?
    out += a.getValueString().compare("37");

    std::ostringstream buf;
    buf << a;
    out += a.getValueString().compare(buf.str());

    // This is for testing the VRDatumPtr.
    MinVR::VRInt c = 1234;
    MinVR::VRDatumPtr p = MinVR::CreateVRDatumInt(&c);

    MinVR::VRInt d = p->getValue();
    out += (1234 == d) ? 0 : 1;

    out += (p->getType() == MinVR::VRCORETYPE_INT) ? 0: 1;
    out += p->getValueString().compare("1234");

    std::ostringstream buf2;
    buf2 << p;
    out += p->getValueString().compare(buf2.str());
  }

  return out;
}

// See testDatumInt() above.
int testDatumFloat() {

  int out = 0;

  LOOP {
    MinVR::VRDatumFloat a = MinVR::VRDatumFloat(37.123f);

    out = a.getDescription().compare("float");

    float b = a.getValue();

    out += (37.123f == b) ? 0 : 1;

    const float* pb = a.getValue();
    out += (37.123f == (*pb)) ? 0 : 1;
    
    out += (a.getType() == MinVR::VRCORETYPE_FLOAT) ? 0 : 1;

    out += a.getValueString().compare("37.123001");

    int j = a.getValue();
    out += (j == 37) ? 0 : 1;

    std::ostringstream buf;
    buf << a;
    out += a.getValueString().compare(buf.str());

    // std::cout << "a:" << a << ",a.getValueString()=" << a.getValueString() << ",buf=" << buf.str() << ",j:" << j << ",out=" << out << std::endl;

  }

  return out;
}

// See testDatumInt() above.
int testDatumString() {

  int out = 0;

  LOOP {

    MinVR::VRDatumString a = MinVR::VRDatumString(std::string("this is a string"));

    out = a.getDescription().compare("string");

    std::string b = a.getValue();

    std::string c = "this is a string";

    out += c.compare(b);

    const std::string* pb = a.getValue();
    out += c.compare(*pb);
    
    out += (a.getType() == MinVR::VRCORETYPE_STRING) ? 0 : 1;

    out += a.getValueString().compare("this is a string");

    std::ostringstream buf;
    buf << a;
    out += a.getValueString().compare(buf.str());
  }

  return out;
}

// See testDatumInt() above.
int testDatumIntArray() {

  int out;

  LOOP {
    int someInts[] = {16,2,77,29};
    std::vector<int> f (someInts, someInts + sizeof(someInts) / sizeof(int) );

    MinVR::VRDatumIntArray a = MinVR::VRDatumIntArray(f);

    out = a.getDescription().compare("intarray");

    std::vector<int> b = a.getValue();

    out += (16 == b[0]) ? 0 : 1;
    out += (2 == b[1]) ? 0 : 1;
    out += (77 == b[2]) ? 0 : 1;
    out += (29 == b[3]) ? 0 : 1;

    const std::vector<int>* pb = a.getValue();

    out += (16 == (*pb)[0]) ? 0 : 1;
    out += (2 == (*pb)[1]) ? 0 : 1;
    out += (77 == (*pb)[2]) ? 0 : 1;
    out += (29 == (*pb)[3]) ? 0 : 1;

    out += (a.getType() == MinVR::VRCORETYPE_INTARRAY) ? 0 : 1;

    out += a.getValueString().compare("16,2,77,29");

    std::ostringstream buf;
    buf << a;
    out += a.getValueString().compare(buf.str());
  }

  return out;
}

// See testDatumInt() above.
int testDatumFloatArray() {

  int out;

  LOOP {
    float someFloats[] = {16.2f,2.71828f,77.3f,29.165f};
    std::vector<float> f (someFloats, someFloats + sizeof(someFloats) / sizeof(float) );

    MinVR::VRDatumFloatArray a = MinVR::VRDatumFloatArray(f);

    out = a.getDescription().compare("floatarray");

    std::vector<float> b = a.getValue();

    out += (b[2] == 77.3f) ? 0 : 1;
    //std::cout << "b[2]=" << b[2] << std::endl;

    const std::vector<float>* pb = a.getValue();
    out += ((*pb)[2] == 77.3f) ? 0 : 1;

    out += (a.getType() == MinVR::VRCORETYPE_FLOATARRAY) ? 0 : 1;

    a.setAttributeValue("separator", ";");

    //std::cout << a.getValueString() << std::endl;
    out += a.getValueString().compare("16.200001;2.718280;77.300003;29.165001");
    //std::cout << "a.getValueString()=" << a.getValueString() << std::endl;

    std::ostringstream buf;
    buf << a;
    out += a.getValueString().compare(buf.str());
    //std::cout << "buf=" << buf.str() << std::endl;
  }

  return out;
}

// See testDatumInt() above.
int testDatumStringArray() {

  int out = 0;

  LOOP {

    MinVR::VRStringArray f;

    f.push_back("one");
    f.push_back("two");
    f.push_back("three");
    f.push_back("four");
    f.push_back("five");

    MinVR::VRDatumStringArray a = MinVR::VRDatumStringArray(f);

    out = a.getDescription().compare("stringarray");

    MinVR::VRStringArray b = a.getValue();

    out += b[0].compare("one");
    out += b[1].compare("two");
    out += b[2].compare("three");
    out += b[3].compare("four");
    out += b[4].compare("five");

    const MinVR::VRStringArray* pb = a.getValue();

    out += (*pb)[0].compare("one");
    out += (*pb)[1].compare("two");
    out += (*pb)[2].compare("three");
    out += (*pb)[3].compare("four");
    out += (*pb)[4].compare("five");

    out += (a.getType() == MinVR::VRCORETYPE_STRINGARRAY) ? 0 : 1;

    a.setAttributeValue("separator", "/");

    out += a.getValueString().compare("one/two/three/four/five");

    std::ostringstream buf;
    buf << a;
    out += a.getValueString().compare(buf.str());
  }

  return out;
}

int testDatumArrayConversions() {

  int out = 0;

  LOOP {

    MinVR::VRDatumString a = MinVR::VRDatumString(std::string("this is a string"));

    out = a.getDescription().compare("string");

    std::string b = a.getValue();

    std::vector<std::string> c = a.getValue();

    out += c[0].compare(b);

    out += (a.getType() == MinVR::VRCORETYPE_STRING) ? 0 : 1;

    out += a.getValueString().compare("this is a string");

    MinVR::VRDatumInt d = MinVR::VRDatumInt(37);

    MinVR::VRIntArray e = d.getValue();

    out += (e[0] == (MinVR::VRInt)d.getValue()) ? 0 : 1;

    MinVR::VRDatumFloat f = MinVR::VRDatumFloat(3.1415926f);

    MinVR::VRFloatArray g = f.getValue();

    out += (g[0] = (MinVR::VRFloat)f.getValue()) ? 0 : 1;

  }

  return out;
}


int testDatumContainer() {

  int out = 0;

  // A VRContainer is just a std::list of std::strings.  It gets its
  // power by being a part of the VRDataIndex.  But we're not testing
  // the index here.

  LOOP {
    MinVR::VRContainer f;

    f.push_back("one");
    f.push_back("two");
    f.push_back("three");
    f.push_back("four");
    f.push_back("five");

    MinVR::VRDatumContainer a = MinVR::VRDatumContainer(f);

    out = a.getDescription().compare("container");

    MinVR::VRContainer b = a.getValue();

    out += b.front().compare("one");
    b.pop_front();
    out += b.front().compare("two");
    b.pop_front();
    out += b.front().compare("three");
    b.pop_front();
    out += b.front().compare("four");
    b.pop_front();
    out += b.front().compare("five");

    out += (a.getType() == MinVR::VRCORETYPE_CONTAINER) ? 0 : 1;

    std::ostringstream buf;
    buf << a;
    out += a.getValueString().compare(buf.str());
  }

  return out;
}


int testDatumPushPopInt() {

  int out = 0;
  std::string testString = " four=\"4\" one=\"1\" three=\"3\" two=\"2\"";
  std::string pushTestString = " four=\"4\" one=\"01\" three=\"03\" two=\"2\"";

  LOOP {

    MinVR::VRInt f = 5;
    MinVR::VRDatumInt a = MinVR::VRDatumInt(f);

    // We'll try pushing and popping the attribute list here, too.
    MinVR::VRDatum::VRAttributeList alist = a.getAttributeList();

    alist["one"] = "11";
    alist["two"] = "2";
    alist["three"] = "3";

    a.setAttributeList(alist);

    a.setAttributeValue("one", "1");
    a.setAttributeValue("four", "4");

    a.push();
    a.setValue(37);
    MinVR::VRInt g = a.getValue();
    out += (37 == g) ? 0 : 1;

    a.setAttributeValue("one", "01");
    a.setAttributeValue("three", "03");

    std::string moreString = a.getAttributeListAsString();
    out += moreString.compare(pushTestString);

    a.pop();
    MinVR::VRInt h = a.getValue();
    out += (5 == h) ? 0 : 1;

    std::string outString = a.getAttributeListAsString();
    out += outString.compare(testString);
  }

  return out;
}

int testDatumPushPopFloat() {

  int out = 0;

  LOOP {

    MinVR::VRFloat f = 3.1415926f;
    MinVR::VRDatumFloat a = MinVR::VRDatumFloat(f);

    a.push();
    a.setValue(2.71828f);
    MinVR::VRFloat g = a.getValue();
    out += (2.71828f == g) ? 0 : 1;

    a.pop();
    MinVR::VRFloat h = a.getValue();
    out += (3.1415926f == h) ? 0 : 1;
  }

  return out;
}

int testDatumPushPopString() {

  int out = 0;

  LOOP {

    MinVR::VRString f = "hello";
    MinVR::VRDatumString a = MinVR::VRDatumString(f);

    a.push();
    a.setValue("good-bye");
    MinVR::VRString g = a.getValue();
    out += g.compare("good-bye");

    a.pop();
    MinVR::VRString h = a.getValue();
    out += h.compare("hello");
  }

  return out;
}

int testDatumPushPopIntArray() {

  int out = 0;

  LOOP {

    int someInts[] = {16,25,77,29};
    MinVR::VRIntArray f (someInts, someInts + sizeof(someInts) / sizeof(int) );

    MinVR::VRDatumIntArray a = MinVR::VRDatumIntArray(f);

    a.push();

    int moreInts[] = {11,22,33,44};
    MinVR::VRIntArray g (moreInts, moreInts + sizeof(moreInts) / sizeof(int) );
    a.setValue(g);

    MinVR::VRIntArray b = a.getValue();
    out += (11 == b[0]) ? 0 : 1;
    out += (22 == b[1]) ? 0 : 1;
    out += (33 == b[2]) ? 0 : 1;
    out += (44 == b[3]) ? 0 : 1;

    a.pop();

    MinVR::VRIntArray c = a.getValue();
    out += (16 == c[0]) ? 0 : 1;
    out += (25 == c[1]) ? 0 : 1;
    out += (77 == c[2]) ? 0 : 1;
    out += (29 == c[3]) ? 0 : 1;
  }

  return out;
}

int testDatumPushPopFloatArray() {

  int out = 0;

  LOOP {

    float someFloats[] = {3.14159f,2.71828f,1.41459f,0.142857f};
    MinVR::VRFloatArray f (someFloats, someFloats + sizeof(someFloats)/sizeof(float) );

    MinVR::VRDatumFloatArray a = MinVR::VRDatumFloatArray(f);

    a.push();

    float moreFloats[] = {1.234f,2.345f,3.456f,4.567f};
    MinVR::VRFloatArray g (moreFloats, moreFloats + sizeof(moreFloats)/sizeof(float) );
    a.setValue(g);

    MinVR::VRFloatArray b = a.getValue();
    out += (1.234f == b[0]) ? 0 : 1;
    out += (2.345f == b[1]) ? 0 : 1;
    out += (3.456f == b[2]) ? 0 : 1;
    out += (4.567f == b[3]) ? 0 : 1;

    a.pop();

    MinVR::VRFloatArray c = a.getValue();
    out += (3.14159f == c[0]) ? 0 : 1;
    out += (2.71828f == c[1]) ? 0 : 1;
    out += (1.41459f == c[2]) ? 0 : 1;
    out += (0.142857f == c[3]) ? 0 : 1;
  }

  return out;
}

int testDatumPushPopStringArray() {

  int out = 0;

  LOOP {

    MinVR::VRStringArray f;

    f.push_back("one");
    f.push_back("two");
    f.push_back("three");
    f.push_back("four");
    f.push_back("five");

    MinVR::VRDatumStringArray a = MinVR::VRDatumStringArray(f);

    a.push();

    MinVR::VRStringArray g;

    g.push_back("six");
    g.push_back("seven");
    g.push_back("eight");
    g.push_back("nine");
    g.push_back("ten");

    a.setValue(g);


    MinVR::VRStringArray b = a.getValue();

    out += b[0].compare("six");
    out += b[1].compare("seven");
    out += b[2].compare("eight");
    out += b[3].compare("nine");
    out += b[4].compare("ten");

    a.pop();

    MinVR::VRStringArray c = a.getValue();

    out += c[0].compare("one");
    out += c[1].compare("two");
    out += c[2].compare("three");
    out += c[3].compare("four");
    out += c[4].compare("five");

  }

  return out;
}

int testDatumPushPopContainer() {

  int out = 0;

  // A VRContainer is just a std::list of std::strings.  It gets its
  // power by being a part of the VRDataIndex.  But we're not testing
  // the index here.

  LOOP {
    MinVR::VRContainer f;

    f.push_back("one");
    f.push_back("two");
    f.push_back("three");
    f.push_back("four");
    f.push_back("five");

    MinVR::VRDatumContainer a = MinVR::VRDatumContainer(f);

    out = a.getDescription().compare("container");

    a.push();

    MinVR::VRContainer g;

    g.push_back("three");
    g.push_back("six");
    g.push_back("seven");

    a.addToValue(g);

    MinVR::VRContainer c = a.getValue();

    out += (c.size() != 7);

    out += c.front().compare("one");
    c.pop_front();
    out += c.front().compare("two");
    c.pop_front();
    out += c.front().compare("three");
    c.pop_front();
    out += c.front().compare("four");
    c.pop_front();
    out += c.front().compare("five");
    c.pop_front();
    out += c.front().compare("six");
    c.pop_front();
    out += c.front().compare("seven");

    a.pop();

    MinVR::VRContainer b = a.getValue();

    out += (b.size() != 5);

    out += b.front().compare("one");
    b.pop_front();
    out += b.front().compare("two");
    b.pop_front();
    out += b.front().compare("three");
    b.pop_front();
    out += b.front().compare("four");
    b.pop_front();
    out += b.front().compare("five");

    out += (a.getType() == MinVR::VRCORETYPE_CONTAINER) ? 0 : 1;

  }

  return out;
}



int testDatumPushPop() {

  int out = 0;

  out += testDatumPushPopInt();
  out += testDatumPushPopFloat();
  out += testDatumPushPopString();
  out += testDatumPushPopIntArray();
  out += testDatumPushPopFloatArray();
  out += testDatumPushPopStringArray();
  out += testDatumPushPopContainer();

  return out;
}
