#ifndef ELEMENT_H
#define ELEMENT_H

#include <list>
#include "attribute.h"


class element
{
  public:
    element();
    element(element* element);
    ~element();
  private:
    std::list<attribute*> m_attribute_list;
    std::list<attribute*>::iterator it;                   //used for various functions
    std::list<attribute*>::iterator m_attribute_iterator; //global iterator for get next/prev attribute
    std::list<element*> m_element_list;
    std::list<element*>::iterator m_element_iterator;     //global iterator for get next/prev node
    std::list<element*>::iterator i;                      //used for various functions
    std::list<char*> m_comment_list;
    std::list<char*>::iterator m_comment_iterator;        //Global iterator for GetNext/prev comment
    std::list<char*>::iterator ic;                        //Global iterator for GetNext/prev comment
    std::list<attribute*> get_attribute_list();           //{return m_attribute_list;};
    std::list<element*> get_element_list();               //{return m_element_list;);
    
  public:
    // Adds an attribute to the attribute list
    bool add_attribute(char* attribute_name, char* value=NULL);
    bool add_attribute(attribute* m_attribute);
    bool set_value(const char* value);
    bool set_name(const char* the_name);
    bool set_parrent(element* the_element);
    bool is_root();
    bool set_comment(char* the_comment);
    bool set_as_pi();
    bool is_pi_element();
    bool is_comment_element();
    // Getter for the attribut by name
    attribute* get_attribute(char* the_name);
    // Gets the next attribute in the list, starting from head and increments the counter
    attribute* get_next_attribute();
    // Gets the previous attribute in the list and decrements the counter
    attribute* get_previous_attribute();
    // //adds a child node to this one
    element &operator = (element& the_element);
    element *operator = (element* the_element);
    element* add_child_element();
    element* get_next_child();
    element* get_parent();
    element* add_child_element(element* child_element);
    char* get_value();
    char* get_name();
private:
    bool is_empty_attribute_list;
    //flag set for the allocation of space for tha value
    bool value_is_allocated;
    bool name_is_allocated;
    bool is_comment;
    bool is_pi;
    char* m_name;
    char* m_value;
    element* m_parrent;
};
#endif // ELEMENT_H
