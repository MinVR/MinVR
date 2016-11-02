#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

namespace MinVR {
  
class attribute
{
  public:
    attribute();
    attribute(attribute* a);
    ~attribute();
  private:
    // The string value of the attribute
    char* m_value;
    // The name of the attribute
    char* m_name;
  public:
    // Gets The name of the attribute
    char* get_name();
    // gets the value of the attribute
    char* get_value();
    // Setter for the value of the attribute
    bool set_value(std::string m_value);
    bool set_value(char* m_value);
    // Setter for the name of the attribute
    bool set_name(std::string m_name);
    bool set_name(char* m_name);
    // Operator for sorting
    bool operator < (attribute& a);
    bool operator == (attribute& a);
    bool operator == (char* a);
    bool operator > (attribute& a);
    attribute &operator = (attribute& a);
    attribute *operator = (attribute* a);
    
  private:
    // falg var for knowing if the value has been previously allocated
    bool value_is_allocated;
    // falg var for knowing if the name has been previously allocated
    bool name_is_allocated;
};

} // ending namespace MinVR
 
#endif // ATTRIBUTE_H
