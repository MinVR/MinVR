#include <iostream>
#include <stdlib.h>
#include "config/VRDataIndex.h"
#include <malloc/malloc.h>

int main(int argc, char** argv) {

  VRDataIndex *n = new VRDataIndex;

  struct malloc_introspection_t	*in;
  
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
  
  // This should be identified by an environment variable, whose value
  // is decoded at this level. 
  n->processXMLFile("${MVRHOME}/tests/config/test.xml", "/");
  
  n->printStructure();
  
  std::cout << "all done: " << std::endl;

  delete n;
}
