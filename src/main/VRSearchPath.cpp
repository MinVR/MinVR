#include "VRSearchPath.h"
#include "VRConfig.h"

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
#ifdef WIN32
  if ((desiredFile[0] == '/') && (_open(desiredFile.c_str(), O_RDONLY) >= 0))
#else
  if ((desiredFile[0] == '/') && (open(desiredFile.c_str(), O_RDONLY) >= 0))
#endif
      return desiredFile;

  for (std::list<std::string>::iterator it = _searchPath.begin();
       it != _searchPath.end(); it++) {
    std::string testFile = _selectFile(desiredFile, (*it));
#ifdef WIN32
  if (_open(testFile.c_str(), O_RDONLY) >= 0)
#else
  if (open(testFile.c_str(), O_RDONLY) >= 0)
#endif
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

std::string VRSearchPath::getFullFilenames(const std::string &desiredFile) const {

    std::string out;
    for (std::list<std::string>::const_iterator it = _searchPath.begin();
         it != _searchPath.end(); it++)
        out += _selectFile(desiredFile, (*it)) + ":";

    return out.substr(0, out.length() - 1);
}

///
// Here is the search path order that MinVR searches for plugins:
//
//    1. Plugin path specified in config ("PluginPath" in VRDataIndex)
//    2. Working directory (".")
//    3. In the MinVR installation pointed to by the $MINVR_ROOT$ envvar
//    4. In the MinVR installation prefix #defined in VRConfig.h (set by cmake)
//    5. In the MinVR build directory #defined in VRConfig.h (set by cmake)
VRSearchPlugin::VRSearchPlugin() {

  // 1. already added externally

  // 2. current working directory
  addPathEntry("./", false);

  // 3. installation pointed to by the $MINVR_ROOT$ envvar
  addPathEntry("${MINVR_ROOT}/lib/MinVR" MINVR_VERSION_STR "/", false);
  addPathEntry("${MINVR_ROOT}/bin/MinVR" MINVR_VERSION_STR "/", false);

  // 4. installation prefix #defined in VRConfig.h (set by cmake)
  addPathEntry(MINVR_INSTALL_PLUGIN_DIR "/", false);

  // 5. build directory #defined in VRConfig.h (set by cmake)
  addPathEntry(MINVR_BUILD_PLUGIN_DIR "/", false);
}


/// Same as VRSearchPath, but built to accommodate the specific semantics of
/// the configuration file naming.
VRSearchConfig::VRSearchConfig() {

  // 1. current working directory
  addPathEntry("./", false);

  // 2. config or minvr subdirs within current working directory
  addPathEntry("./config/", false);
  addPathEntry("./Config/", false);
  addPathEntry("./minvr/", false);
  addPathEntry("./MinVR/", false);

  // 3. installation pointed to by the $MINVR_ROOT$ envvar
  addPathEntry("${MINVR_ROOT}/share/MinVR" MINVR_VERSION_STR "/config", false);

  // 4. installation prefix #defined in VRConfig.h (set by cmake)
  addPathEntry(MINVR_INSTALL_CONFIG_DIR "/", false);

  // 5. build directory #defined in VRConfig.h (set by cmake)
  addPathEntry(MINVR_BUILD_CONFIG_DIR "/", false);
}


} // namespace
