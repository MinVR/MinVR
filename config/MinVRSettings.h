// -*-c++-*-
#ifndef MINVR_SETTINGS_H
#define MINVR_SETTINGS_H

#include <vector>
#include <cstring>
#include <map>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <deque>
#include "Cxml.h"

using namespace std;

const int SUCCESS = 0;
const int FAILURE = 1;

typedef enum 
{
  NONE       = 0,
  INT        = 1,
  FLOAT      = 2,
  STRING     = 3,
  VEC_INT    = 4,
  VEC_FLOAT  = 5,
  VEC_STRING = 6
} TYPE_ID;

class MinVRSettings 
{

  private:
    // Setting up a vector of name and value pairs, where the value can
    // be strings, ints, or floats.
    // map<string, string> settingNames;
    map<string, int>    settingFlags;
    map<string, string> settingValues;
    map<string, int>    settingIntValues;
    map<string, float>  settingFloatValues;

    map<string, vector<string> > settingValuesVector;
    map<string, vector<int> >    settingIntValuesVector;
    map<string, vector<float> >  settingFloatValuesVector;
    
    map<string, string> settingsToValues;

    map<string, TYPE_ID> keyword_to_type;
    deque<std::string> key_list;
    static const string id_str;
  protected:
    string context_id;
    vector<int>   string_int_vec(const string& value_column);
    vector<float> string_float_vec(const string& value_column);
    
    bool determine_if_xml(string);
    void fill_xml_tree(element* xml_node);
    int  readValuesNonXML(string settingFileName);
    int  readValuesXML(string settingFileName);

    string preppend(string& key);

  public:
    MinVRSettings(){ } // Constructor need not do much
    MinVRSettings(string settingFileName) { } // Populate settings

    string getValueString(string settingName);
    int    getValueStringVector(string settingName,vector<string>& stringValues);
    int    getValueInt(string settingName);
    int    getValueIntVector(string settingName,vector<int>& intValues);
    float  getValueFloat(string settingName);
    int    getValueFloatVector(string settingName,vector<float>& floatValues);
  
    int setValueString(string settingName, string settingValue);
    int setValueStringVector(string settingName, const vector<string>& settingValues);
    int setValueInt(string settingName, int settingValue);
    int setValueIntVector(string settingName, const vector<int>& settingValues);
    int setValueFloat(string settingName, float settingValue);
    int setValueFloatVector(string settingName, const vector<float>& settingValues);

    // Return value is some indicator of success/failure/type mismatch/etc

    int readValues(string settingFileName);

    int writeValues(string settingFileName);

    // Incorporate a collection of settings into this collection.
    int incorporateSettings(MinVRSettings settings);

    // define depth of scoping- use syntax e.g. deep/deeper/deepest/ 
    void set_context(string context);

};


// Inherits from MinVRSettings, provides names and default values.
class MinVRCoreSettings : public MinVRSettings {

 public:
  MinVRCoreSettings() {
    // set names and default values
  }
};
#if 0

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


#endif
#endif // MINVR_SETTINGS_H
