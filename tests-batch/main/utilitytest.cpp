#include "main/VRSearchPath.h"
#include "main/VRMain.h"

int testSearchPath();
int testCommandLineParse();

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

  case 2:
    output = testCommandLineParse();
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
    executeShellCommand("pwd > /Users/keefe/ivlab-github/sw/MinVR/MinVR/build/wd.txt");
  executeShellCommand("mkdir -p testSearch/test2/test3/lib");
  executeShellCommand("mkdir -p testSearch/test2/test4/lib");
  executeShellCommand("echo hello >testSearch/test2/test3/lib/" + libName);

//  executeShellCommand("mkdir -p testSearch/test2/test4/" + libRoot + "/lib");
//  executeShellCommand("echo hello >testSearch/test2/test4/" + libRoot + "/lib/" + libName);

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

  // What about an absolute path name?
  char p[200];
  std::string rel = "testSearch/test2/test3/target.txt";
  std::string absolutePath = std::string(getcwd(p, 200)) + "/" + rel;
  std::cout << "checking on absolute path:" << absolutePath << std::endl;

  std::string t = sp.findFile(absolutePath);
  out += t.substr(t.find("testSearch")).compare(rel);

  // Make the absolute path name fail.
  absolutePath += "d";
  t = sp.findFile(absolutePath);
  // Should be empty string.
  out += t.size();

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

  std::cout << "libRoot:" << libRoot<< "->libName:" << libName << std::endl;

  // Look for a plugin, using the root name.
  MinVR::VRSearchPlugin spp;
  sp.addPathEntry("testSearch/test2/test3/lib");
  std::cout << "sp: " << sp.getPath() << std::endl;
  spp.digestPathString(sp.getPath());

  std::cout << "plug search path: " << spp << std::endl;

  std::cout << "spp:" << spp.findFile(libRoot) << std::endl;

  // Did we find it?
  std::string compareString = "testSearch/test2/test3/lib/lib" + libRoot +
#ifdef MinVR_DEBUG
    "d" +
#endif
#ifdef __APPLE__
    ".dylib"
#else
    ".so"
#endif
    ;
  std::cout << "compare: " << compareString << std::endl;

  out += spp.findFile(libRoot).compare(compareString);
  std::cout << "spp:" << spp.findFile(libRoot) << " out:" << out << std::endl;

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

class testParse : public MinVR::VRParseCommandLine {

public:
  int testCount;

  testParse() { testCount = 1; };

  void setConfigValue(const std::string &keyAndValStr) {

    std::cout << "SET CONFIG:" << keyAndValStr << std::endl;
    testCount += 11;
  };

  void loadConfig(const std::string &configName) {

    std::cout << "LOAD CONFIG:" << configName << std::endl;
    testCount += 5;
  };

  void loadDefaultConfig() {
    std::cout << "LOAD DEFAULT CONFIG" << std::endl;
    testCount += 0;
  }
};


int testCommandLineParse() {

  int out = 0;

  char* argStrings[10];
  int numArgs = 0;
  std::string arg;


  //1./////////////////////////////////////////////////////////////////////////
  // Test a typical line.
  std::string testCommand = "program argument1 argument2 -k 2 --load-config default";
  std::cout << "Test typical line:" << testCommand << std::endl;

  std::stringstream argStream(testCommand);
  while (argStream >> arg) {

    if (arg.size() > 0) {
      argStrings[numArgs] = (char*)malloc(arg.size() + 2);
      strcpy(argStrings[numArgs++], arg.c_str());
    }
  }

  // Parse it.
  testParse* tp = new testParse();
  tp->parseCommandLine(numArgs, argStrings);

  // We tried to only call loadConfig.
  if (tp->testCount != 6) out++;

  std::cout << "leftovers: " << tp->getLeftoverArgc() << ": ";

  for (int i = 0; i < tp->getLeftoverArgc(); i++) {
    std::cout << tp->getLeftoverArgv()[i] << ", ";
  }
  std::cout << std::endl;

  // Should have three leftovers, and the third called argument2.
  if ((tp->getLeftoverArgc() != 5) ||
      (strcmp(tp->getLeftoverArgv()[2], "argument2") != 0)) out++;

  // The original line should match the testCommand, not counting the extra space
  // at the end of the line that isn't worth our time to squeeze out.
  std::cout << "test Line>" << testCommand << "<" << std::endl;
  std::cout << "original line>" << tp->getOriginalCommandLine() << "<" << std::endl;
  if ((tp->getOriginalCommandLine().compare(0, testCommand.size(), testCommand)) != 0) out++;

  // Save this for the next test.
  std::string saveOriginalCommandLine = tp->getOriginalCommandLine();

  std::cout << "leftover line>" << tp->getLeftoverCommandLine() << "<" << std::endl;

  // The leftovers should match the first part of testCommand.
  if ((tp->getLeftoverCommandLine().substr(0, 27).compare(testCommand.substr(0, 27))) != 0) out++;

  std::cout << "MADE IT TO HERE WITH OUT=" << out << std::endl;
  delete tp;

  //1a./////////////////////////////////////////////////////////////////////////
  // Test another typical line, but using an equal sign.
  testCommand = "program argument1 argument2 -k 2 --load-config=default";
  std::cout << "Test another typical line:" << testCommand << std::endl;

  numArgs = 0;
  argStream.clear();
  argStream.str("");
  argStream << testCommand;
  while (argStream >> arg) {

    if (arg.size() > 0) {
      argStrings[numArgs] = (char*)malloc(arg.size() + 2);
      strcpy(argStrings[numArgs++], arg.c_str());
    }
  }

  // Parse it.
  tp = new testParse();
  tp->parseCommandLine(numArgs, argStrings);

  // We tried to only call loadConfig.
  if (tp->testCount != 6) out++;

  std::cout << "1a leftovers: " << tp->getLeftoverArgc() << ": ";

  for (int i = 0; i < tp->getLeftoverArgc(); i++) {
    std::cout << tp->getLeftoverArgv()[i] << ", ";
  }
  std::cout << std::endl;

  // Should have five leftovers, and the third called argument2.
  if ((tp->getLeftoverArgc() != 5) ||
      (strcmp(tp->getLeftoverArgv()[2], "argument2") != 0)) out++;

  // The original line should match the testCommand, not counting the extra space
  // at the end of the line that isn't worth our time to squeeze out.
  std::cout << "test Line>" << testCommand << "<" << std::endl;
  std::cout << "original line>" << tp->getOriginalCommandLine() << "<" << std::endl;
  std::cout << "saved line   >" << saveOriginalCommandLine.substr(0,46) << "<" << std::endl;
  if ((tp->getOriginalCommandLine().substr(0, 46).compare(saveOriginalCommandLine.substr(0,46))) != 0) out++;

  std::cout << "leftover line>" << tp->getLeftoverCommandLine() << "<" << std::endl;

  // The leftovers should match the first part of testCommand.
  if ((tp->getLeftoverCommandLine().substr(0, 27).compare(testCommand.substr(0, 27))) != 0) out++;

  std::cout << "MADE IT TO HERE WITH OUT=" << out << std::endl;
  delete tp;

  //1b./////////////////////////////////////////////////////////////////////////
  // Test another typical line, but using no execution.
  testCommand = "program argument1 --no-execute argument2 --set-value name=Tom --set-value lesson=first --load-config=default";
  std::cout << "Test another typical line, but with --no-execute:" << testCommand << std::endl;

  numArgs = 0;
  argStream.clear();
  argStream.str("");
  argStream << testCommand;
  while (argStream >> arg) {

    if (arg.size() > 0) {
      argStrings[numArgs] = (char*)malloc(arg.size() + 2);
      strcpy(argStrings[numArgs++], arg.c_str());
    }
  }

  // Parse it.
  tp = new testParse();
  bool ret = tp->parseCommandLine(numArgs, argStrings);

  // Check the return value.
  if (ret) out++;

  // The config calls are still executed.
  if (tp->testCount != 28) out++;

  std::cout << "1b leftovers: " << tp->getLeftoverArgc() << ": ";

  for (int i = 0; i < tp->getLeftoverArgc(); i++) {
    std::cout << tp->getLeftoverArgv()[i] << ", ";
  }
  std::cout << std::endl;

  // Should have three leftovers, and the third called argument2.
  if ((tp->getLeftoverArgc() != 3) ||
      (strcmp(tp->getLeftoverArgv()[2], "argument2") != 0)) out++;

  std::cout << "MADE IT TO HERE WITH OUT=" << out << std::endl;
  delete tp;

  //2./////////////////////////////////////////////////////////////////////////
  std::cout << "Now let's try an encoded command line." << std::endl;

  tp = new testParse();

  std::string unencoded = "command arg1 arg2 ";
  std::string encoded = tp->argsToData("-s SetupsToStart=doThisOne -c Wonderful");

  numArgs = 0;
  std::stringstream as(unencoded + encoded);

  while (as >> arg) {

    if (arg.size() > 0) {
      argStrings[numArgs] = (char*)malloc(arg.size() + 2);
      strcpy(argStrings[numArgs++], arg.c_str());
    }
  }

  tp->parseCommandLine(numArgs, argStrings);

  // Should have called loadConfig and setConfigValue.
  if (tp->testCount != 17) out++;

  std::cout << "original line>" << tp->getOriginalCommandLine() << "<" << std::endl;
  std::cout << "leftover line>" << tp->getLeftoverCommandLine() << "<" << std::endl;

  // Should have three leftovers, and the third called arg2.
  if ((tp->getLeftoverArgc() != 3) ||
      (strcmp(tp->getLeftoverArgv()[2], "arg2") != 0)) out++;




  std::cout << "MADE IT TO HERE WITH OUT=" << out << std::endl;
  delete tp;

  //3./////////////////////////////////////////////////////////////////////////
  // Catching errors?
  std::cout << "Try catching errors." << std::endl;

  tp = new testParse();

  // Test a typical line with an error.
  numArgs = 0;
  argStream.clear();
  argStream.str("");
  argStream << "program argument1 -c";
  while (argStream >> arg) {

    if (arg.size() > 0) {
      argStrings[numArgs] = (char*)malloc(arg.size() + 2);
      strcpy(argStrings[numArgs++], arg.c_str());
    }
  }

  // Parse it.  Should cause an exception because there's no argument
  // for the -c.
  try {
    tp->parseCommandLine(numArgs, argStrings);
    std::cout << "no exception, no catch!" << std::endl;
    out += 1;
  } catch (const MinVR::VRError& e) {
    std::cout << "caught exception: " << e.what() << std::endl;
  }

  std::cout << "MADE IT TO HERE WITH OUT=" << out << std::endl;
  delete tp;

  //4./////////////////////////////////////////////////////////////////////////
  // Test a typical line after turning off parsing.
  std::cout << "Test with no parsing." << std::endl;

  tp = new testParse();

  numArgs = 0;
  argStream.clear();
  argStream.str("");
  argStream << "program argument1 -s hello=34";
  while (argStream >> arg) {

    if (arg.size() > 0) {
      argStrings[numArgs] = (char*)malloc(arg.size() + 2);
      strcpy(argStrings[numArgs++], arg.c_str());
    }
  }

  // Turn off command line parsing.
  tp->noParsing();

  // The 'noParsing()' method should make the option name strings empty.
  if (!tp->getSetConfigValueShort().empty()) out++;

  tp->parseCommandLine(numArgs, argStrings);

  std::cout << "original line>" << tp->getOriginalCommandLine() << "<" << std::endl;
  std::cout << "leftover line>" << tp->getLeftoverCommandLine() << "<" << std::endl;

  // Should not have called loadConfig and setConfigValue.
  if (tp->testCount != 1) out++;

  // Should have four leftovers, and the fourth called config.
  if ((tp->getLeftoverArgc() != 4) ||
      (strcmp(tp->getLeftoverArgv()[3], "hello=34") != 0)) out++;

  std::cout << "MADE IT TO HERE WITH OUT=" << out << std::endl;
  delete tp;

  //5./////////////////////////////////////////////////////////////////////////
  // Another test, turning off parsing, but making sure it still
  // parses the MINVR_DATA business.
  std::cout << "Test MINVR_DATA with parsing turned off." << std::endl;
  tp = new testParse();

  numArgs = 0;
  argStream.clear();
  argStream.str("");
  unencoded = "command arg1 arg2 ";
  encoded = tp->argsToData("-s SetupsToStart=doThisOne -c Wonderful");
  argStream << unencoded + encoded;

  while (argStream >> arg) {

    if (arg.size() > 0) {
      argStrings[numArgs] = (char*)malloc(arg.size() + 2);
      strcpy(argStrings[numArgs++], arg.c_str());
    }
  }

  tp->noParsing();
  tp->parseCommandLine(numArgs, argStrings);

  // The only parsing should have been the MINVR_DATA thing, so we
  // should *not* have called loadConfig or setConfigValue.
  if (tp->testCount != 1) out++;

  // There should be seven leftovers from the original command line,
  // and the last is Wonderful.
  if ((tp->getLeftoverArgc() != 7) ||
      (strcmp(tp->getLeftoverArgv()[6], "Wonderful") != 0)) out++;

  std::cout << "original line>" << tp->getOriginalCommandLine() << "<" << std::endl;
  std::cout << "leftover line>" << tp->getLeftoverCommandLine() << "<" << std::endl;

  std::cout << "MADE IT TO HERE WITH OUT=" << out << std::endl;
  delete tp;

  // Any failures along the way should make this non-zero.
  return out;
}
