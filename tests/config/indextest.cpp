#include "config/VRDataIndex.h"

int testIndexSerialize();
int testIndexSerializeIntArray();
int testIndexSerializeIntArraySep();
int testIndexPrintDoubleArray();
int testIndexLotsaEntries();

// Make this a large number to get decent timing data.
#define LOOP for (int loopctr = 0; loopctr < 10; loopctr++)

int indextest(int argc, char* argv[]) {
  
  int defaultchoice = 1;
  
  int choice = defaultchoice;

  if (argc > 1) {
    if(sscanf(argv[1], "%d", &choice) != 1) {
      printf("Couldn't parse that input as a number\n");
      return -1;
    }
  }

  int output;
  
  switch(choice) {
  case 1:
    output = testIndexSerialize();
    break;
    
  case 2:
    output = testIndexSerializeIntArray();
    break;

  case 3:
    output = testIndexSerializeIntArraySep();
    break;

  case 4:
    output = testIndexPrintDoubleArray();
    break;

  case 5:
    output = testIndexLotsaEntries();
    break;
    
  default:
    std::cout << "Test #" << choice << " does not exist!\n";
    output = -1;
  }
  
  return output;
}

VRDataIndex * setupIndex() {

  VRDataIndex *n = new VRDataIndex;

  VRInt a = 4;
  VRDouble b = 3.1415926;
  
  n->addData("/george/a0", a);
  n->addData("/george/a1", a + 1);
  n->addData("/george/a2", a + 2);
  n->addData("/george/a3", a + 3);
  n->addData("/george/a4", a + 4);
  n->addData("/george/a5", a + 5);
  n->addData("/george/a6", a + 6);
  n->addData("/george/a7", a + 7);
  n->addData("/george/a8", a + 8);
  n->addData("/george/a9", a + 9);

  n->addData("/martha/b0", b);
  n->addData("/martha/b1", b * 1);
  n->addData("/martha/b2", b * 2);
  n->addData("/martha/b3", b * 3);
  n->addData("/martha/b4", b * 4);
  n->addData("/martha/b5", b * 5);
  n->addData("/martha/b6", b * 6);
  n->addData("/martha/b7", b * 7);
  n->addData("/martha/b8", b * 8);
  n->addData("/martha/b9", b * 9);

  VRString c = "abigail";
  n->addData("/john/c0", c + "0");
  n->addData("/john/c1", c + "1");
  n->addData("/john/c2", c + "2");
  n->addData("/john/c3", c + "3");
  n->addData("/john/c4", c + "4");
  n->addData("/john/c5", c + "5");
  n->addData("/john/c6", c + "6");
  n->addData("/john/c7", c + "7");
  n->addData("/john/c8", c + "8");
  n->addData("/john/c9", c + "9");

  std::vector<double>d;
  d.push_back(1.2);
  d.push_back(2.3);
  d.push_back(3.4);
  d.push_back(4.5);
  d.push_back(5.6);

  n->addData("/donna/d0", d);
  
  // This file is specified using the WORKING_DIRECTORY option in the
  // ctest framework.  See the CMakeLists.txt file in this directory,
  // and look for the add_test command.
  n->processXMLFile("test.xml", "/");
  
  return n;
}

int testIndexSerializeIntArray() {

  std::string testString = "<atestarray type=\"intarray\">0@1@2@3@4@5@6@7@8@9@10@11@12@13@14@15@16@17@18@19@20@21@22@23@24@25@26@27@28@29@30@31@32@33@34@35@36@37@38@39@40@41@42@43@44@45@46@47@48@49@50@51@52@53@54@55@56@57@58@59@60@61@62@63@64@65@66@67@68@69@70@71@72@73@74@75@76@77@78@79@80@81@82@83@84@85@86@87@88@89@90@91@92@93@94@95@96@97@98@99</atestarray>";

  int out = 0;

  LOOP {
    VRDataIndex *n = setupIndex();

    std::vector<int>e;

    for (int i = 0; i < 100; i++) {
      e.push_back(i);
    }

    n->addData("/george/atestarray", e);
  
    std::string output = n->serialize("atestarray", "/george");
  
    delete n;

    out = output.compare(testString);
  }

  return out;
    
}

int testIndexSerializeIntArraySep() {

  std::string testString = "<atestarray type=\"intarray\" separator=\",\">0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99</atestarray>";

  int out = 0;

  LOOP {
    VRDataIndex *n = setupIndex();

    std::vector<int>e;

    for (int i = 0; i < 100; i++) {
      e.push_back(i);
    }

    n->addData("/george/atestarray", e);
    n->getDatum("/george/atestarray")->setAttributeValue("separator", ",");

    std::string output = n->serialize("atestarray", "/george");
  
    delete n;

    out += output.compare(testString);
  }

  return out;    
}

int testIndexSerialize() {

  std::string testString = "<MVR type=\"container\"><Server type=\"container\"><Port type=\"string\">3490</Port><Host type=\"string\">localhost</Host><NumClients type=\"int\">1</NumClients></Server><VRPlugins type=\"container\"><MinVRDefaultPlugins type=\"container\"><Names type=\"stringarray\">MinVR_GLFW@MinVR_OpenGL@MinVR_Threading</Names><Data type=\"doublearray\" separator=\",\">1.200000,2.300000,3.400000,4.500000,5.600000</Data></MinVRDefaultPlugins></VRPlugins><VRDisplayDevices type=\"container\"><ThreadedDisplay type=\"container\"><displayType type=\"string\">thread_group</displayType><Display1 type=\"container\"><allowThreading type=\"int\">1</allowThreading><displayType type=\"string\" val=\"heavy\">glfw_display</displayType><xOffset type=\"int\">600</xOffset><yOffset type=\"int\">0</yOffset><width type=\"int\">200</width><height type=\"int\">200</height></Display1><Display2 type=\"container\"><displayType type=\"string\">glfw_display</displayType><allowThreading type=\"int\">1</allowThreading><xOffset type=\"int\">600</xOffset><yOffset type=\"int\">250</yOffset><width type=\"int\">200</width><height type=\"int\">200</height><stereoFormatter type=\"container\"><displayType type=\"string\">sideBySideStereo</displayType></stereoFormatter></Display2><Display3 type=\"container\"><displayType type=\"string\">glfw_display</displayType><allowThreading type=\"int\">1</allowThreading><xOffset type=\"int\">600</xOffset><yOffset type=\"int\">450</yOffset><width type=\"int\">200</width><height type=\"int\">200</height><stereoFormatter type=\"container\"><displayType type=\"string\">sideBySideStereo</displayType></stereoFormatter></Display3></ThreadedDisplay><MainDisplay type=\"container\"><displayType type=\"string\">glfw_display</displayType><xOffset type=\"int\">800</xOffset><yOffset type=\"int\">0</yOffset><width type=\"int\">300</width><height type=\"int\">600</height></MainDisplay><OtherDisplay type=\"container\"><displayType type=\"string\">glfw_display</displayType><xOffset type=\"int\">0</xOffset><yOffset type=\"int\">0</yOffset><width type=\"int\">600</width><height type=\"int\">600</height><stereoFormatter type=\"container\"><displayType type=\"string\">sideBySideStereo</displayType><topViewport type=\"container\"><displayType type=\"string\">viewport</displayType><xOffset type=\"int\">0</xOffset><yOffset type=\"int\">300</yOffset><width type=\"int\">600</width><height type=\"int\">300</height></topViewport><bottomViewport type=\"container\"><displayType type=\"string\">viewport</displayType><xOffset type=\"int\">0</xOffset><yOffset type=\"int\">0</yOffset><width type=\"int\">600</width><height type=\"int\">300</height></bottomViewport></stereoFormatter></OtherDisplay><radius type=\"double\">20.000000</radius><Display1 type=\"container\"><radius type=\"double\">7.000000</radius><xOffset type=\"int\">0</xOffset><yOffset type=\"int\">300</yOffset></Display1><Display2 type=\"container\"><xOffset type=\"int\">0</xOffset><yOffset type=\"int\">300</yOffset></Display2></VRDisplayDevices></MVR>";

  int out = 0;

  LOOP {
  
    VRDataIndex *n = setupIndex();

    std::string output = n->serialize("/MVR");

    delete n;

    out += output.compare(testString);
  }

  return out;
  
}

int testIndexPrintDoubleArray() {

  std::string testString = " | donna\n |  | d0 = 1.200000@2.300000@3.400000@4.500000@5.600000 (doublearray)\n";

  int out;

  LOOP {
  
    VRDataIndex *n = setupIndex();

    std::string output = n->printStructure("/donna",200);  
    delete n;

    out += output.compare(testString);
  }

  return out;  
}
  
int testIndexLotsaEntries() {

  int out = 0;

  VRDataIndex *n = setupIndex();

  LOOP {
    std::stringstream name;
    name << "/henry/entry" << loopctr;

    n->addData(name.str(), loopctr);   
  }

  LOOP {
    std::stringstream name;
    name << "/henry/entry" << loopctr;

    out += ((loopctr == (int)n->getValue(name.str())) ? 0 : 1);
  }  

  return out;
}
  
