#include "MinVRSettings.h"

const string keyword_delimiter_str="/";
const char   keyword_delimiter_char = '/';

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
    return SUCCESS;
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
    return FAILURE;
}


int    
MinVRSettings::getValueIntVector(string settingName,vector<int>& intValues)
{
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
    return SUCCESS;
}


vector<int>    
MinVRSettings::string_int_vec(const string& value_column)
{
    vector<int> i_tmp;
    char *pch, *dup = strdup(value_column.c_str());
    pch = strtok(dup," \t,()");
    while(pch != NULL)
    {
        i_tmp.push_back(atoi(pch));
        pch = strtok(NULL," \t,()");
    }
    return i_tmp;
    
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
    return FAILURE;
}


int    
MinVRSettings::getValueFloatVector(string settingName,vector<float>& floatValues)
{
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
    return SUCCESS;
}


vector<float>    
MinVRSettings::string_float_vec(const string& value_column)
{
    vector<float> f_tmp;
    char *pch, *dup = strdup(value_column.c_str());
    pch = strtok(dup," \t,()");
    while(pch != NULL)
    {
        f_tmp.push_back(static_cast<float>(strtod(pch,NULL)));
        pch = strtok(NULL," \t,()");
    }
    return f_tmp;
}


int    
MinVRSettings::setValueString(string settingName, string settingValue)
{
    settingValues[settingName]=settingValue;
    keyword_to_type[settingName] = STRING;
    return SUCCESS;
}


int    
MinVRSettings::setValueStringVector(string settingName, const vector<string>& settingValues)
{
//    settingValues[settingName]=settingValue;
//    keyword_to_type[settingName] = STRING;
    return SUCCESS;
}


int    
MinVRSettings::setValueInt(string settingName, int settingValue)
{
    char str_tmp[128];
    sprintf(str_tmp,"%d",settingValue);
    settingsToValues[settingName]=str_tmp;
    settingIntValues[settingName]=settingValue;
    keyword_to_type[settingName] = INT;
    return SUCCESS;
}


int    
MinVRSettings::setValueIntVector(string settingName, const vector<int>& settingValues)
{
    string tmp;
    char str_tmp[128];
    for (int j=0; j<settingValues.size(); j++)
    {
        sprintf(str_tmp,"%d",settingValues[j]);
        tmp += str_tmp;
        tmp += " ";
    }
    settingsToValues[settingName]=tmp;
    
    settingIntValuesVector[settingName]=settingValues;
    keyword_to_type[settingName] = VEC_INT;
    return SUCCESS;
}


int    
MinVRSettings::setValueFloat(string settingName, float settingValue)
{
    char str_tmp[128];
    sprintf(str_tmp,"%f",settingValue);
    settingsToValues[settingName]=str_tmp;
    settingFloatValues[settingName]=settingValue;
    keyword_to_type[settingName] = FLOAT;
    return SUCCESS;
}


int    
MinVRSettings::setValueFloatVector(string settingName, const vector<float>& settingValues)
{
    string tmp;
    char str_tmp[128];
    for (int j=0; j<settingValues.size(); j++)
    {
        sprintf(str_tmp,"%f",settingValues[j]);
        tmp += str_tmp;
        tmp += " ";
    }
    settingsToValues[settingName]=tmp;
    
    settingFloatValuesVector[settingName]=settingValues;
    keyword_to_type[settingName] = VEC_FLOAT;
    return SUCCESS;
}

static void
remove_surrounding_spaces(string& s,
			  const string& sp = " \t[]\r")
{
    //string sp(" \t[]\r");
  int i,j;
  if (s.empty())
    return;
  i = s.find_first_not_of(sp);
  if (i < 0)
    {
      // all spaces in here so get out
      return;
    }
  s = s.substr(i);
  j = s.find_last_not_of(sp);
  if ( j>=0)
    s.erase(j+1, s.size());
}

string
parse_begin_section(const string& first_column,
			      const string& str)
{
    int s;

    s = first_column.find(" section");
    if (s >= 0)
    {
	string sp=str;
	remove_surrounding_spaces(sp);
	// assumes the name is well-formed; no error checking
	return sp;
    }
    return string("");
}

void 
parse_end_section(const string& first_column,
		  string& accumulated_key)
{
    int s;

    s = first_column.find("endsection");
    if (s >= 0)
    {
	int i = accumulated_key.find_last_of(keyword_delimiter_char);

	accumulated_key = accumulated_key.substr(0,i);
    }
    else
    {
	return;
    }
}
int    
MinVRSettings::readValues(string settingFileName)
{
    // Read config settings from a file
    ifstream file(settingFileName.c_str());
    int i;
    char *pch, *str;
    string line, the_key, the_val;
    const string empty_string="";
    string       new_section_name;
    string       accumulated_key="";

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
		    // find new section  scoping and process it
		    new_section_name = parse_begin_section(the_key, the_val);

		    // check to see if end def scoping, and do book-keeping
		    parse_end_section(the_key, accumulated_key);
		    if (new_section_name == empty_string)
		    {
			settingsToValues[accumulated_key+the_key] = the_val;
		    }
		    else
		    {
			accumulated_key+=keyword_delimiter_str;
			accumulated_key+=new_section_name;
		    }
                }
                else
                    settingsToValues[accumulated_key+the_key] = "";
            }
        }
    }
    else
    {
        cout << "File " << settingFileName << "cannot be openned" << endl;
        return FAILURE;
    }
    return SUCCESS;
}


int    
MinVRSettings::writeValues(string settingFileName)
{
    ofstream m_file;
    m_file.open(settingFileName.c_str());
    if (m_file.is_open())
    {
        map<string, string>::iterator i;
        for(i=settingsToValues.begin(); i!= settingsToValues.end(); i++)
        {
            string tmp = i->second;
            tmp.erase(tmp.begin(), std::find_if(tmp.begin(), tmp.end(), std::bind1st(std::not_equal_to<char>(), ' ')));
            m_file << setw(25) << left << i->first << " " << tmp << endl;
        }
        m_file.close();
    }
    else
    {
        cout << "File " << settingFileName << "cannot be openned for writing." << endl;
        return FAILURE;
    }
    return SUCCESS;
}

