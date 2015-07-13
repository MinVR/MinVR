#include "MinVRSettings.h"
#include <stdlib.h>
#include <cstdlib>

string 
MinVRSettings::getValueString(string settingName)
{
    map<string, TYPE_ID>::iterator i;
    i = keyword_to_type.find(settingName);
    if (i == keyword_to_type.end())
    {
	// translate input into a string
	string value_column = settingsToValues[settingName];
	
	// save it in the keyword_to_type lookup
	keyword_to_type[settingName] = STRING;

	// save it in the settingValues map
	settingValues[settingName] = value_column;
	return value_column;
    }
    else
    {
	// return saved value
	TYPE_ID t = i->second;
	if (t == STRING)
	{
	    return settingValues[settingName]; 
	}
	else
	{
	    cout << " getValueString (" << settingName << ") is not a string."
		 << endl;
	    //exit(1);
	}

    }
    return string();
}

int    
MinVRSettings::getValueStringVector(string settingName,vector<string>& stringValues)
{
    //
	return 0;
}

int    
MinVRSettings::getValueInt(string settingName)
{
    map<string, TYPE_ID>::iterator i;
    i = keyword_to_type.find(settingName);
    if (i == keyword_to_type.end())
    {
	// translate input into a string
	string value_column = settingsToValues[settingName];
	
	// save it in the keyword_to_type lookup
	keyword_to_type[settingName] = INT;

	int tmp_int = atoi(value_column.c_str());
	// save it in the settingValues map
	settingIntValues[settingName] = tmp_int;
	return tmp_int;
    }
    else
    {
	// return saved value
	TYPE_ID t = i->second;
	if (t == INT)
	{
	    return settingIntValues[settingName]; 
	}
	else
	{
	    cout << " getValueInt (" << settingName << ") is not an Int."
		 << endl;
	    //exit(1);
	}

    }
    return 1;
}

int    
MinVRSettings::getValueIntVector(string settingName,vector<int>& intValues)
{
    //
    map<string, TYPE_ID>::iterator i;
    i = keyword_to_type.find(settingName);
    if (i == keyword_to_type.end())
    {
        // translate input into a string
        string value_column = settingsToValues[settingName];
        
        // save it in the keyword_to_type lookup
        keyword_to_type[settingName] = VEC_INT;
        
        intValues = string_int_vec(value_column);
        // save it in the settingValues map
        settingIntValuesVector[settingName] = intValues;
        return intValues.size();
    }
    else
    {
        // return saved value
        TYPE_ID t = i->second;
        if (t == VEC_INT)
        {
            intValues = settingIntValuesVector[settingName]; 
            return intValues.size();
        }
        else
        {
            cout << " getValueIntVector (" << settingName << ",intValues) is not a vector of integers."
                 << endl;
            //exit(1);
        }
    }
    return 0;
}

vector<int> MinVRSettings::string_int_vec(string value_column)
{
    //
    char * pch;
    vector<int> tmp;
    char* str=(char*)value_column.c_str();
    pch = strtok (str," \t,()");
    while (pch != NULL)
    {
        //printf ("%s\n",pch);
        tmp.push_back(atoi(pch));
        pch = strtok (NULL, " \t,()");
    }
    return tmp;
}

float 
MinVRSettings::getValueFloat(string settingName)
{
    map<string, TYPE_ID>::iterator i;
    i = keyword_to_type.find(settingName);
    if (i == keyword_to_type.end())
    {
	// translate input into a string
	string value_column = settingsToValues[settingName];
	
	// save it in the keyword_to_type lookup
	keyword_to_type[settingName] = FLOAT;

	float tmp_float = (float)strtod(value_column.c_str(),NULL);
	// save it in the settingValues map
	settingFloatValues[settingName] = tmp_float;
	return tmp_float;
    }
    else
    {
	// return saved value
	TYPE_ID t = i->second;
	if (t == FLOAT)
	{
	    return settingFloatValues[settingName]; 
	}
	else
	{
	    cout << " getValueFloat (" << settingName << ") is not an Float."
		 << endl;
	    //exit(1);
	}

    }
    return 1;
}

int    
MinVRSettings::getValueFloatVector(string settingName,vector<float>& floatValues)
{
    //
    map<string, TYPE_ID>::iterator i;
    i = keyword_to_type.find(settingName);
    if (i == keyword_to_type.end())
    {
        // translate input into a string
        string value_column = settingsToValues[settingName];
        
        // save it in the keyword_to_type lookup
        keyword_to_type[settingName] = VEC_FLOAT;
        
        floatValues = string_float_vec(value_column);
        // save it in the settingValues map
        settingFloatValuesVector[settingName] = floatValues;
        return floatValues.size();
    }
    else
    {
        // return saved value
        TYPE_ID t = i->second;
        if (t == VEC_FLOAT)
        {
            floatValues = settingFloatValuesVector[settingName]; 
            return floatValues.size();
        }
        else
        {
            cout << " getValueFloatVector (" << settingName << ",floatValues) is not a vector of floats."
                 << endl;
            //exit(1);
        }
    }
    return 0;
}

vector<float> MinVRSettings::string_float_vec(string value_column)
{
    //
    char * pch;
    vector<float> tmp;
    char* str=(char*)value_column.c_str();
    pch = strtok (str," \t,()");
    while (pch != NULL)
    {
        //printf ("%s\n",pch);
        tmp.push_back(strtod(pch,NULL));
        pch = strtok (NULL, " \t,()");
    }
    return tmp;
}

int 
MinVRSettings::setValueString(string settingName, string settingValue)
{
    settingValues[settingName]=settingValue;
    return 0;
}

int 
MinVRSettings::setValueInt(string settingName, int settingValue)
{
    settingIntValues[settingName]=settingValue;
    return 0;
}

int 
MinVRSettings::setValueFloat(string settingName, float settingValue)
{
    settingFloatValues[settingName]=settingValue;
    return 0;
}

vector<std::string> MinVRSettings::string_float_vec_test(const std::string m_string)
{
    //
    char * pch;
    vector<string> tmp;
    char* str=(char*)m_string.c_str();
//    printf ("Splitting string \"%s\" into tokens:\n",str);
    pch = strtok (str," \t,()");
    while (pch != NULL)
    {
        //printf ("%s\n",pch);
        tmp.push_back(string(pch));
        pch = strtok (NULL, " \t,()");
    }
    return tmp;
}


int MinVRSettings::readValues(string settingFileName)
{
    // Read config settings from a file
    ifstream file(settingFileName.c_str());
    int i;
    char *pch, *str;
    string line, the_key, the_val;
    
    if (file.is_open())
    {
        while (!file.eof())
        {
            getline(file, line);
            i = line.find("#");
            if (i>=0)
                continue;
            
            str=(char*)line.c_str();
            pch = strtok (str," \t,()");
            if(pch)
            {
                the_key = string(pch);
                pch = strtok (NULL, "");
                if(pch)
                {
                    the_val = string(pch);
                    cout << "the_key = " << the_key << ", the_val = " << the_val << endl;
                    settingsToValues[the_key] = the_val;
                }
                else
                    settingsToValues[the_key] = "";
            }
        }
    }
    else
    {
        cout << "File " << settingFileName << "cannot be openned" << endl;
        return 1;
    }
    return 0;
}
