#include "config/VRDatum.h"

int testDatums();

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
  
  switch(choice) {
  case 1:
    output = testDatums();
    break;

    // Add case statements to handle values of 2-->10
  default:
    std::cout << "Test #" << choice << " does not exist!\n";
    output = -1;
  }
  
  return output;
}

int testDatums() {


  return 0;
}
