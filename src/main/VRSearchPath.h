#ifndef VRSEARCHPATH_H
#define VRSEARCHPATH_H

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <fcntl.h>

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
  ///
  /// \param pathEntry The directory name to add.
  /// \param start A boolean value.  If true, add the entry at the beginning
  /// of the list.  Otherwise add it to the end.
  void addPathEntry(const std::string &pathEntry, bool start = false) {
    if (start) {
      _searchPath.push_front(pathEntry);
    } else {
      _searchPath.push_back(pathEntry);
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

  /// \brief Return path as a single string.
  ///
  /// Returns the search path as a single colon-separated string.
  std::string getPrintString() const {

    std::string out;
    for (std::list<std::string>::const_iterator it = _searchPath.begin();
         it != _searchPath.end(); it++)
      out += (*it) + ":";

    return out.substr(0, out.length() - 1);
  }

  friend std::ostream &operator<<(std::ostream &os, const VRSearchPath &p) {
    return os << p.getPrintString();
  };
};

}

#endif // VRSEARCHPATH_H
