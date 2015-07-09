// -*-c++-*-
#include <vector>
#include <string>
using namespace std;

class MinVRSettings 
{

 private:
  // Setting up a vector of name and value pairs, where the value can
  // be strings, ints, or doubles.
  std::vector<string> settingNames;
  std::vector<int> settingFlags;
  std::vector<string> settingValues;
  std::vector<int> settingIntValues;
  std::vector<double> settingFloatValues;

 public:
  MinVRSettings(); // Constructor need not do much
  MinVRSettings(string settingFileName); // Populate settings
                                              // with file contents.


  string getValue(string settingName);
  int    getValue(string settingName);
  double getValue(string settingName);

  // Return value is some indicator of success/failure/type mismatch/etc
  int setValue(string settingName, string settingValue);
  int setValue(string settingName, int settingValue);
  int setValue(string settingName, double settingValue);

  int readValues(string settingFileName);

  int writeValues(string settingFileName);

  // Incorporate a collection of settings into this collection.
  int incorporateSettings(MinVRSettings settings);


};


// Inherits from MinVRSettings, provides names and default values.
class MinVRCoreSettings : public MinVRSettings {

 public:
  MinVRSettings() {
    // set names and default values
  };
};

//usage could then be something like this:

  /*****************************************/
  // Set the basic collection of settings
    MinVRCoreSettings settings = MinVRCoreSettings(settingsFile);

  // Incorporate from other plugins if those settings are not in the
  // settings file. 
  settings.incorporateSettings(firstPluginSettingsFile);
  settings.incorporateSettings(secondPluginSettingsFile);

    // Adjust the settings here based upon argc and argv
  

    // Initialize MinVR with the desired settings
    MinVRCore::instance()->initialize(settings);
  /*****************************************/

