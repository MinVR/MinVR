#include "attribute.h"

namespace MinVR {
  
attribute::attribute() : m_value(NULL),
                             m_name(NULL),
                             value_is_allocated(false),
                             name_is_allocated(false)
{
    //
}


attribute::attribute(attribute *a) : m_value(NULL),
                                           m_name(NULL),
                                           value_is_allocated(false),
                                           name_is_allocated(false)
{
    if(a != NULL)
    {
        set_value(a->get_value());
        set_name(a->get_name());
    }
}


attribute::~attribute()
{
    if(value_is_allocated)
        free(m_value);
    if(name_is_allocated)
        free(m_name);
}


// Gets The name of the attribute
char* attribute::get_name()
{
    return m_name;
}


// Gets the value of the attribute
char* attribute::get_value()
{
    return m_value;
}

bool attribute::set_value(std::string value) {
  return set_value((char *)value.c_str());
}

// Setter for the value of the attribute
bool attribute::set_value(char* value)
{
    if(value != NULL)
    {
        int l;
        l = strlen(value)*sizeof(char);
        if(l == 0)
            return false;
        if(value_is_allocated)
            free(m_value);
        m_value = (char*)malloc(l+sizeof(char));
        memset(m_value,'\0',l+sizeof(char));
        strcpy(m_value,value);
        value_is_allocated = true;
        return true;
    }
    return false;
}

bool attribute::set_name(std::string name) {
  return set_name((char *)name.c_str());
}

bool attribute::set_name(char* name)
{
    if(name != NULL)
    {
        int l;
        l = strlen(name)*sizeof(char);
        if(l == 0)
            return false;
        if(name_is_allocated)
            free(m_name);
        m_name = (char*)malloc(l+sizeof(char));
        memset(m_name,'\0',l+sizeof(char));
        strcpy(m_name,name);
        name_is_allocated = true;
        return true;
    }
    return false;
}


bool attribute::operator < (attribute &a)
{
    if(strcmp(m_name,a.get_name()) < 0)
        return true;
    return false;
}


bool attribute::operator == (attribute &a)
{
    if(strcmp(m_name,a.get_name()) == 0)
        return true;
    return false;
}


bool attribute::operator > (attribute &a)
{
    if(strcmp(m_name,a.get_name()) > 0)
        return true;
    return false;
}


bool attribute::operator == (char* a)
{
    if(strcmp(m_name,a) == 0)
        return true;
    return false;
}


attribute& attribute::operator=(attribute& a)
{
    set_name(a.get_name());
    set_value(a.get_value());
    return *this;
}


attribute* attribute::operator = (attribute* a)
{
    set_name(a->get_name());
    set_value(a->get_value());
    return this;
}
  
} // ending namespace MinVR
