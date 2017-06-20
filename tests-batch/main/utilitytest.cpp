#include "main/VRSearchPath.h"

int testSearchPath();

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

  // Set up a test tree of directories and subdirectories.  Put a target file
  // in there, and a decoy.
  executeShellCommand("mkdir -p testSearch");
  executeShellCommand("mkdir -p testSearch/test1");
  executeShellCommand("mkdir -p testSearch/test2");
  executeShellCommand("mkdir -p testSearch/test2/test3");
  executeShellCommand("mkdir -p testSearch/test2/test4");
  executeShellCommand("mkdir -p testSearch/test2/test4/test5");
  executeShellCommand("echo hello >testSearch/test2/test4/target.txt");
  executeShellCommand("echo hello >testSearch/test2/test3/target.txt");

  std::string libRoot = "Henry";
#ifdef __APPLE__
  std::string libName = "lib" + libRoot +
#ifdef MinVR_DEBUG
    "d" +
#endif
    ".dylib";
#else
  std::string libName = "lib" + libRoot +
#ifdef MinVR_DEBUG
    "d" +
#endif
    ".so";
#endif

  // Extend the directory structure for libraries.
  executeShellCommand("mkdir -p testSearch/test2/test3/lib");
  executeShellCommand("mkdir -p testSearch/test2/test4/lib");
  executeShellCommand("echo hello >testSearch/test2/test3/lib/" + libName);

  executeShellCommand("mkdir -p testSearch/test2/test4/" + libRoot + "/lib");
  executeShellCommand("echo hello >testSearch/test2/test4/" + libRoot + "/lib/" + libName);

    // Plant a couple of config files
  executeShellCommand("echo hello >testSearch/test2/test3/Chester.minvr");
  executeShellCommand("echo hello >testSearch/test2/test3/Chester.xml");


  // Set up the search path.
  MinVR::VRSearchPath sp;
  sp.addPathEntry("testSearch");
  sp.addPathEntry("testSearch/test1");
  sp.addPathEntry("testSearch/test2");
  sp.addPathEntry("testSearch/test2/test3");
  sp.addPathEntry("testSearch/test2/test4");
  sp.addPathEntry("testSearch/test2/test4/test5");

  // Do we find the file we're after?
  out += sp.findFile("target.txt").compare("testSearch/test2/test4/target.txt");

  std::cout << "result:" << sp.findFile("target.txt") << " out:" << out << std::endl;
  std::cout << "path:" << sp << std::endl;

  // Does the colon-separated search path look right?
  out += sp.getPath().compare("testSearch/test2/test4/test5:testSearch/test2/test4:testSearch/test2/test3:testSearch/test2:testSearch/test1:testSearch");

  // Test whether the digestPathString works properly.  This should result in an
  // sp2 object the same as the sp object, so the tests are the same.
  MinVR::VRSearchPath sp2;
  sp2.digestPathString(sp.getPath());

  out += sp2.findFile("target.txt").compare("testSearch/test2/test4/target.txt");

  std::cout << "path:" << sp2 << " out:" << out << std::endl;

  out += sp2.getPath().compare("testSearch/test2/test4/test5:testSearch/test2/test4:testSearch/test2/test3:testSearch/test2:testSearch/test1:testSearch");

  std::cout << "result:" << sp2.findFile("target.txt") << " out:" << out << std::endl;

  // Now we search for plugins and config files, which have some extra
  // semantic issues.


  std::cout << "libRoot:" << libRoot<< "libName:" << libName << std::endl;

  // Look for a plugin, using the root name.
  MinVR::VRSearchPlugin spp;
  spp.digestPathString(sp.getPath());

  std::cout << "spp:" << spp.findFile(libRoot) << std::endl;

  // Did we find it?
  out += spp.findFile(libRoot).compare("testSearch/test2/test4/Henry/lib/libHenryd.dylib");

  MinVR::VRSearchConfig spc;
  spc.digestPathString(sp.getPath());

  // If we don't specify the suffix, we get the default ".minvr" suffix.  If
  // we do specify a suffix, we get that.
  out += spc.findFile("Chester").compare("testSearch/test2/test3/Chester.minvr");
  std::cout << "spc:" << spc.findFile("Chester") << " out:" << out << std::endl;

  out += spc.findFile("Chester.xml").compare("testSearch/test2/test3/Chester.xml");

  std::cout << "spc:" << spc.findFile("Chester.xml") << " out:" << out << std::endl;
  
  executeShellCommand("rm -rf testSearch");

#endif

  return out;
}
