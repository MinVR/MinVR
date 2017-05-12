#ifndef VRSEARCHPATH_H
#define VRSEARCHPATH_H

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <fcntl.h>

#include <config/VRDataIndex.h>

namespace MinVR {

/// This is a simple little class to manage the process of searching a path of
/// directories for a particular file.  MinVR uses subclasses of it both for
/// searching for plugin libraries and for searching for configuration files.
/// The class works as is for simple searching for files, but if you want to
/// add semantic restrictions to the file (i.e. "ends in '.xml' or 'in a
/// subdirectory called lib and ends in '.dylib', make a sub-class and
/// redefine the selectFile() method.
class VRSearchPath {
 protected:
  std::list<std::string> _searchPath;

  virtual std::string _selectFile(const std::string &file,
                                  const std::string &directory) {
    std::string validDirectory = directory;
    if (*(validDirectory.rbegin()) != '/') validDirectory += "/";
    return validDirectory + file;
  }

 public:
  VRSearchPath() {};

  /// \brief Add an entry to the search path.
  ///
  /// Adds the given directory name to the list of directories to search.
  /// Entries can use environment variables, bracketed with "${}".
  ///
  /// \param pathEntry The directory name to add.
  /// \param start A boolean value.  If true (default), add the entry at
  /// the beginning of the list.  Otherwise add it to the end.
  void addPathEntry(const std::string &pathEntry, bool start = true) {

    std::string entry;
    // Unpack any environment variables.  If they're in the entry, but not
    // defined, ignore the entry and get out.
    try {
      entry = MinVR::VRDataIndex::dereferenceEnvVars(pathEntry);
    } catch (...) {
      return;
    }

    if (start) {
      _searchPath.push_front(entry);
    } else {
      _searchPath.push_back(entry);
    }
  };

  /// \brief Create a search path from a single string.
  ///
  /// Adds a whole path from one colon-separated string.
  void digestPathString(const std::string &searchList) {

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
  };

  /// \brief Find a file.
  ///
  /// \return Returns a fully-resolved path name of the file in question,
  /// wherever it lies on the search path.  If the file is not found, the
  /// return value is empty.
  std::string findFile(const std::string &desiredFile) {
    for (std::list<std::string>::iterator it = _searchPath.begin();
         it != _searchPath.end(); it++) {
      std::string testFile = _selectFile(desiredFile, (*it));
      if (open(testFile.c_str(), O_RDONLY) >= 0)
        return testFile;
    }
    return "";
  }

  /// \brief Return path as a single string.
  ///
  /// Returns the search path as a single colon-separated string.
  std::string getPath() const {

    std::string out;
    for (std::list<std::string>::const_iterator it = _searchPath.begin();
         it != _searchPath.end(); it++)
      out += (*it) + ":";

    return out.substr(0, out.length() - 1);
  }

  friend std::ostream &operator<<(std::ostream &os, const VRSearchPath &p) {
    return os << p.getPath();
  };
};

/// Same as VRSearchPath, but built to accommodate the specific semantics of
/// the plugin naming.
class VRSearchPlugin : public VRSearchPath {

 protected:
  std::string _selectFile(const std::string &file,
                          const std::string &directory) {

    // "file" indicates the name of the plugin, which will appear in both the
    // directory name *and* the name of the library containing the plugin
    // code.
    std::string validDirectory = directory;
    if (*(directory.rbegin()) != '/') {
      validDirectory += "/" + file;
    } else {
      validDirectory += file;
    }

    std::string buildType = "";
#ifdef MinVR_DEBUG
    buildType = "d";
#endif

#if defined(WIN32)
    return validDirectory + "/bin/" + file + buildType + ".dll";

#elif defined(__APPLE__)
    return validDirectory + "/lib/lib" + file + buildType + ".dylib";

#else // Linux
    return validDirectory + "/lib/lib" + file + buildType + ".so";

#endif
  }

 public:
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
  VRSearchPlugin() {

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

};

/// Same as VRSearchPath, but built to accommodate the specific semantics of
/// the configuration file naming.
class VRSearchConfig : public VRSearchPath {

 protected:
  std::string _selectFile(const std::string &file,
                          const std::string &directory) {

    std::string validDirectory = directory;
    if (*(directory.rbegin()) != '/') {
      validDirectory += "/";
    }

    std::string validFile = file;
    // Does the file already have the suffix specified?
    if (validFile.find('.') == std::string::npos) validFile += ".minvr";

    return validDirectory + validFile;
  };

 public:
  VRSearchConfig() {
    // 1. current working directory
    addPathEntry("./", false);

    // 2. config subdir within current working directory
    addPathEntry("./config/", false);

    // 3. running from within the build tree from build/bin or tests-*/testname
    addPathEntry("../../config/", false);

    // 4. an installed version based on MINVR_ROOT envvar
    addPathEntry("${MINVR_ROOT}/config/", false);

    // 5. an installed version based on the INSTALL_PREFIX set with cmake
    addPathEntry(std::string(INSTALLPATH) + "/config/", false);
  }
};

}

#endif // VRSEARCHPATH_H
