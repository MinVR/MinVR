//  Copyright Regents of the University of Minnesota and Brown University, 2010.  All rights are reserved.


#ifndef CONFIGMAP_H
#define CONFIGMAP_H

#include "CommonInc.H"

#include "G3DOperators.h"

namespace MinVR {


#if (defined(__SUNPRO_CC) && __SUNPRO_CC >= 0x500) || \
    (defined(__GNUC__) && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)))
#define QUOTED_STRING const char *
#else
#define QUOTED_STRING char *
#endif


/**
   ConfigVal's and config files description:

   ConfigVal's are an easy way to access program settings that can be
   stored in a configuration file.  They are all key=value pairs where
   key and value are both originally std::strings.  But, the value
   string can be easily reinterpreted by any class that overrides the
   stream >> and << operators.  The ConfigVal() function figures out
   what type to try to convert to by the type of the second parameter
   to the function.  This parameter also specifies the default value
   to return if the key is not found in the ConfigMap::_map Table.

   To use a config file, call ConfigMap::readFile() with the filename
   of a text file to read.  The format of each line of the file should be:

   key value

   where key has no spaces in it.  value is everything after the space
   until the end of the line, so it can have spaces in it.  Any line
   that *starts* with the character # is a comment and is ignored.
   A single \ character means cut the line here and continue on the
   next line.  Everything after the \ is ignored and the text on the
   next line is appended to the text on the current line.  You can
   escape the \ operator with a \\ to get a single \ character.

   Additionally, any value X appearing inside the special character
   sequence $(X) will be replaced by the value of the environment
   variable named X.  If X is not defined, it will be replaced by a
   null string.  If X takes the form of the path to a file and you're
   running in Windows under cygwin then X will be automatically
   converted from the form /cygdrive/c/blah/blah to the more Windows
   friendly form c:/blah/blah to stay compatible with the Visual C++
   compiler.

   Additionally, if you put a += after the key and the key has already
   been defined, then the value is appended to the key's current value.
   If you don't have the += and the same key is read in from a file,
   then the new value overwrites the old.  For example:

   mykey1     value1
   mykey1     value2
   mykey2     value3
   mykey2+=   value4

   The value of mykey1 will be "value2".  The value of mykey2 will be
   "value3 value4".


   --- inside config-file.cfg ---

   MyLength  0.4
   MyVector (0.2, 0.4, 0.3)

   --- end of file ---

   Log *log = new Log("log.txt");
   ConfigMapRef myMap = new ConfigMap("config-file.cfg", log);
   double l  = myMap->get("MyLength", 0.0);
   Vector3 v = myMap->get("MyVector", Vector3::zero());

*/


/// This class stores the data for the ConfigVal accessor functions
/// Essentially, this is a big associative array std::string's map to
/// std::strings.  The ConfigVal function is used to access values and
/// reinterpret them as different types.  The key/value pairs can be
/// read in from a file(s), or set manually with the set function.
typedef G3D::ReferenceCountedPointer<class ConfigMap> ConfigMapRef;
class ConfigMap : public G3D::ReferenceCountedObject
{
public:
  /// Creates an empty map
  PLUGIN_API ConfigMap(G3D::Log *log) {
    _log = log;
  }

  /// Creates a map by reading in the config file
  PLUGIN_API ConfigMap(const std::string &filename, G3D::Log *log) {
    _log = log;
    readFile(filename);
  }

  /// Create a map by reading in arguments that specify files to load
  /// and/or individual mappings
  PLUGIN_API ConfigMap(int argc, char **argv, G3D::Log *log, bool exitOnUnrecognizedArgument);

  PLUGIN_API virtual ~ConfigMap() {}

  PLUGIN_API void printArgumentHelpAndExit(const std::string &programName);
  PLUGIN_API bool readFile(const std::string &filename);

  template <class T>
  bool retypeString(const std::string &str, T &val) {
    std::istringstream is(str.c_str());
    is >> val;
    if (!is) {
      return false;
    }
    else {
      return true;
    }
  }

  template <class VALTYPE>
  VALTYPE get(std::string keyString, const VALTYPE &defaultVal) {
    if (containsKey(keyString)) {
      std::string valString = getValue(keyString);
      VALTYPE val;
      bool ok = retypeString(valString, val);
      if (ok) {
        return val;
      }
      else {
        if (_log) {
          _log->printf("ConfigMap Error: cannot remap '%s'", valString.c_str());
        }
        else {
          printf("ConfigMap Error: cannot remap '%s'", valString.c_str());
        }
        return defaultVal;
      }
    }
    else {
      if (_log) {
        _log->printf("ConfigMap Warning: no mapping for '%s'.\n", keyString.c_str());
      }
      else {
        printf("ConfigMap Warning: no mapping for '%s'.\n", keyString.c_str());
      }
      return defaultVal;
    }
  }

  std::string get(QUOTED_STRING keyString, QUOTED_STRING defaultVal) {
    if (containsKey(keyString))
      return replaceEnvVars(getValue(keyString));
    else {
      if (_log) {
        _log->printf("ConfigMap Warning: no mapping for '%s'.\n", keyString);
      }
      else {
        printf("ConfigMap Warning: no mapping for '%s'.\n", keyString);
      }
      return replaceEnvVars(defaultVal);
    }
  }

  std::string get(QUOTED_STRING keyString, std::string defaultVal) {
    if (containsKey(keyString))
      return replaceEnvVars(getValue(keyString));
    else {
      if (_log) {
        _log->printf("ConfigMap Warning: no mapping for '%s'.\n", keyString);
      }
      else {
        printf("ConfigMap Warning: no mapping for '%s'.\n", keyString);
      }
      return replaceEnvVars(defaultVal);
    }
  }

  std::string get(std::string keyString, QUOTED_STRING defaultVal) {
    if (containsKey(keyString))
      return replaceEnvVars(getValue(keyString));
    else {
      if (_log) {
        _log->printf("ConfigMap Warning: no mapping for '%s'.\n", keyString.c_str());
      }
      else {
        printf("ConfigMap Warning: no mapping for '%s'.\n", keyString.c_str());
      }
      return replaceEnvVars(defaultVal);
    }
  }

  std::string get(std::string keyString, std::string defaultVal) {
    if (containsKey(keyString))
      return replaceEnvVars(getValue(keyString));
    else {
      if (_log) {
        _log->printf("ConfigMap Warning: no mapping for '%s'.\n", keyString.c_str());
      }
      else {
        printf("ConfigMap Warning: no mapping for '%s'.\n", keyString.c_str());
      }
      return replaceEnvVars(defaultVal);
    }
  }

  PLUGIN_API bool         containsKey(const std::string &keyString);
  PLUGIN_API std::string  getValue(const std::string &keyString);
  PLUGIN_API void         set(const std::string &key, const std::string &value);
  PLUGIN_API void         debugPrint();

private:
  G3D::Table<std::string, std::string> _map;
  G3D::Log* _log;
};


} // end namespace


#endif

