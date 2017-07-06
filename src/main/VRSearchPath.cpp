#include "VRSearchPath.h"

namespace MinVR {

void VRSearchPath::addPathEntry(const std::string &pathEntry, bool start) {

  std::string entry;
  // Unpack any environment variables.  If they're in the entry, but not
  // defined, ignore the entry and get out.
  try {
    entry = VRDataIndex::dereferenceEnvVars(pathEntry);
  } catch (...) {
    return;
  }

  if (start) {
    _searchPath.push_front(entry);
  } else {
    _searchPath.push_back(entry);
  }
}

void VRSearchPath::digestPathString(const std::string &searchList) {

  // If the searchList already has stuff in it, put this list at the beginning.
  std::list<std::string> newList;

  std::stringstream ss(searchList);
  std::string singlePath;
  while (std::getline(ss, singlePath, ':')) {
    // Add them to the end of the line.
    newList.push_back(singlePath);
  }

  if (_searchPath.empty()) {
    _searchPath = newList;
  } else {
    _searchPath.insert(_searchPath.begin(), newList.begin(), newList.end());
  }
}

std::string VRSearchPath::findFile(const std::string &desiredFile) {

  // If this is an absolute path name (starts with a /), just return
  // the file name intact, if it exists.
  if ((desiredFile[0] == '/') && (open(desiredFile.c_str(), O_RDONLY) >= 0))
    return desiredFile;

  for (std::list<std::string>::iterator it = _searchPath.begin();
       it != _searchPath.end(); it++) {
    std::string testFile = _selectFile(desiredFile, (*it));
    if (open(testFile.c_str(), O_RDONLY) >= 0)
      return testFile;
  }
  return "";
}

std::string VRSearchPath::getPath() const {

  std::string out;
  for (std::list<std::string>::const_iterator it = _searchPath.begin();
       it != _searchPath.end(); it++)
    out += (*it) + ":";

  return out.substr(0, out.length() - 1);
}

///
// Here is the search path order that MinVR searches for plugins:
//
//    1. Plugin path specified in config ("/PluginPath" in VRDataIndex)
//    2. Working directory (".")
//    3. <Working directory>/plugins ("./plugins")
//    4. Custom user defined paths (i.e. vrmain->addPluginSearchPath(mypath))
//    5. <Binary directory>/../plugins ("build/bin/../plugins")
//    6. <Install directory>/plugins ("install/plugins")
//    7. <$MINVR_ROOT>/plugins ("$MINVR_ROOT/plugins")
VRSearchPlugin::VRSearchPlugin() {

  // 1. current working directory
  addPathEntry("./", false);

  // 2. config subdir within current working directory
  addPathEntry("./plugins/", false);

  // 3. running from within the build tree from build/bin or tests-*/testname
  addPathEntry("../plugins/", false);

  // 4. an installed version based on MINVR_ROOT envvar
  addPathEntry("${MINVR_ROOT}/plugins/", false);

  // 5. an installed version based on the INSTALL_PREFIX set with cmake
  addPathEntry(std::string(INSTALLPATH) + "/plugins", false);

}

/// Same as VRSearchPath, but built to accommodate the specific semantics of
/// the configuration file naming.
VRSearchConfig::VRSearchConfig() {

  // 1. current working directory
  addPathEntry("./", false);

  // 2. config subdir within current working directory
  addPathEntry("./config/", false);

  // 3. running from within the build tree from build/bin or tests-*/testname
  addPathEntry("../config/", false);
  addPathEntry("../../config/", false);

  // 4. an installed version based on MINVR_ROOT envvar
  addPathEntry("${MINVR_ROOT}/config/", false);

  // 5. an installed version based on the INSTALL_PREFIX set with cmake
  addPathEntry(std::string("${CMAKE_INSTALL_PREFIX}") + "/config/", false);
}

}
