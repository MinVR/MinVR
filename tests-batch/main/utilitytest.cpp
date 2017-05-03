#include "main/VRSearchPath.h"

int testSearchPath();

// Make this a large number to get decent timing data.
#define LOOP for (int loopctr = 0; loopctr < 1; loopctr++)

int utilitytest(int argc, char* argv[]) {

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
    output = testSearchPath();
    break;

  default:
    std::cout << "Test #" << choice << " does not exist!\n";
    output = -1;
  }

  return output;
}

#ifndef WIN32
std::string executeShellCommand(const std::string cmd) {

  char buffer[128];
  std::string result = "";
  FILE* pipe = popen(cmd.c_str(), "r");
  if (!pipe) throw std::runtime_error("popen() failed!");
  try {
    while (!feof(pipe)) {
      if (fgets(buffer, 128, pipe) != NULL)
        result += buffer;
    }
  } catch (...) {
    pclose(pipe);
    throw;
  }
  pclose(pipe);

  return result;
}
#endif // WIN32

int testSearchPath() {

#ifdef WIN32
  // Don't know how to test this on Windows
  int out = 0;

#else

  int out = 0;

  executeShellCommand("mkdir -p testSearch");
  executeShellCommand("mkdir -p testSearch/test1");
  executeShellCommand("mkdir -p testSearch/test2");
  executeShellCommand("mkdir -p testSearch/test2/test3");
  executeShellCommand("mkdir -p testSearch/test2/test4");
  executeShellCommand("mkdir -p testSearch/test2/test4/test5");
  executeShellCommand("echo hello >testSearch/test2/test4/target.txt");
  executeShellCommand("echo hello >testSearch/test2/test3/target.txt");

  MinVR::VRSearchPath sp;
  sp.addPathEntry("testSearch");
  sp.addPathEntry("testSearch/test1");
  sp.addPathEntry("testSearch/test2");
  sp.addPathEntry("testSearch/test2/test3");
  sp.addPathEntry("testSearch/test2/test4");
  sp.addPathEntry("testSearch/test2/test4/test5");

  std::cout << "result:" << sp.findFile("target.txt") << std::endl;
  std::cout << "path:" << sp << std::endl;

  out += sp.findFile("target.txt").compare("testSearch/test2/test3/target.txt");

  out += sp.getPrintString().compare("testSearch:testSearch/test1:testSearch/test2:testSearch/test2/test3:testSearch/test2/test4:testSearch/test2/test4/test5");

#endif

  executeShellCommand("rm -rf testSearch");

  return out;
}
