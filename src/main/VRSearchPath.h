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
  // This would be better if the constructor could hold an arbitrary
  // number of simple strings.  Can we do that?
  VRSearchPath() {};

  void addPathEntry(const std::string &pathEntry) {
    _searchPath.push_back(pathEntry);
  };

  // Adds a whole path as one colon-separated string.
  void digestPathString(const std::string &searchList) {
    std::stringstream ss(searchList);
    std::string singlePath;
    while (std::getline(ss, singlePath, ':')) {
      addPathEntry(singlePath);
    }
  };

  // Returns a fully-resolved path name of the file in question,
  // wherever it lies on the search path.  If the file is not found,
  // the return value is empty.
  std::string findFile(const std::string &desiredFile) {
    for (std::list<std::string>::iterator it = _searchPath.begin();
         it != _searchPath.end(); it++) {
      std::string testFile = (*it) + "/" + desiredFile;
      if (open(testFile.c_str(), O_RDONLY) >= 0)
        return testFile;
    }
    return "";
  }

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
