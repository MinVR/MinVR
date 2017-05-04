#ifndef VRSEARCHPATH_H
#define VRSEARCHPATH_H

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <fcntl.h>

#include <config/VRDataIndex.h>

namespace MinVR {

/// This is a simple little class to manage the process of searching a
/// path of directories for a particular file.  MinVR uses it both for
/// searching for plugin libraries and for searching for configuration
/// files.
class VRSearchPath {
 private:
  std::list<std::string> _searchPath;

 public:
  VRSearchPath() {};

  /// \brief Add an entry to the search path.
  ///
  /// Adds the given directory name to the list of directories to search.
  /// Entries can use environment variables, bracketed with "${}".
  ///
  /// \param pathEntry The directory name to add.
  /// \param start A boolean value.  If true, add the entry at the beginning
  /// of the list.  Otherwise add it to the end.
  void addPathEntry(const std::string &pathEntry, bool start = false) {

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
    std::stringstream ss(searchList);
    std::string singlePath;
    while (std::getline(ss, singlePath, ':')) {
      addPathEntry(singlePath);
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
      std::string testFile = (*it) + "/" + desiredFile;
      if (open(testFile.c_str(), O_RDONLY) >= 0)
        return testFile;
    }
    return "";
  }

  /// \brief Find a file.
  ///
  /// This is the same as findFile, with some OS-specific extra semantics
  /// in the name.
  /// \return Returns a fully-resolved path name of the library in question,
  /// wherever it lies on the search path.  If the library is not found, the
  /// return value is empty.
  std::string findLib(const std::string &desiredLib) {

    std::string libName;

#if defined(WIN32)
    // We support just asking for the library, or spelling out the whole name.
    // So here's some logic to figure out which one we've got.
    if ((desiredLib.size() > 4) &&
        (desiredLib.substr(desiredLib.size() - 4).compare(".dll") == 0)) {
      libName = desiredLib;
    } else {
      libName = desiredLib +
#ifdef MinVR_DEBUG
        "d" +
#endif
        ".dll";
    }

    return findFile("bin/" + libName);
#elif defined(__APPLE__)
    // See above about figuring out which kind of library name we've got.
    if ((desiredLib.size() > 6) &&
        (desiredLib.substr(desiredLib.size() - 6).compare(".dylib") == 0)) {
      libName = desiredLib;
    } else {
      libName = desiredLib +
#ifdef MinVR_DEBUG
        "d" +
#endif
        ".dylib";
    }
    if (libName.substr(0,3).compare("lib") != 0) {
      libName = "lib" + libName;
    }

    return findFile("lib/" + libName);
#else
    // See above about figuring out which kind of library name we've got.
    if ((desiredLib.size() > 3) &&
        (desiredLib.substr(desiredLib.size() - 3).compare(".so") == 0)) {
      libName = desiredLib;
    } else {
      libName = desiredLib +
#ifdef MinVR_DEBUG
        "d" +
#endif
        ".so";
    }
    if (libName.substr(0,3).compare("lib") != 0) {
      libName = "lib" + libName;
    }

    return findFile("lib/" + libName);
#endif
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

}

#endif // VRSEARCHPATH_H
