#include "element.h"

namespace MinVR {

element::element() : is_empty_attribute_list(true),
                   value_is_allocated(false),
                   name_is_allocated(false),
                   is_comment(false),
		   is_pi(false),
                   m_name(NULL),
                   m_value(NULL),
                   m_parent(NULL)
{
    m_element_iterator = m_element_list.begin();
    m_attribute_iterator = m_attribute_list.begin();
}

element::element(element* node) : is_empty_attribute_list(true),
                               value_is_allocated(false),
                               name_is_allocated(false),
                               is_comment(false),
                               m_name(NULL),
                               m_value(NULL),
                               m_parent(NULL)
{
    if(node != NULL)
    {
        set_value(node->get_value());
        set_name(node->get_name());
        m_attribute_list = node->get_attribute_list();
        m_element_list = node->get_element_list();
    }
    m_element_iterator = m_element_list.begin();
    m_attribute_iterator = m_attribute_list.begin();
}

element::~element()
{
    if(value_is_allocated)
        free(m_value);
    if(name_is_allocated)
        free(m_name);
    if(is_empty_attribute_list == false)
    {
        it = m_attribute_list.begin();
        while(it != m_attribute_list.end())
        {
            delete(*it);
            it++;
        }
    }
    i = m_element_list.begin();
    while(i != m_element_list.end())
    {
        delete(*i);
        i++;
    }
}

// Added by ts to get the whole attribute list back at once.
std::map<std::string, std::string> element::get_attribute_map() {

  std::map<std::string, std::string> out;
  
  for (std::list<attribute*>::iterator it = m_attribute_list.begin();
       it != m_attribute_list.end(); it++) {
    out[std::string((*it)->get_name())] = std::string((*it)->get_value());
  }

  return out;
}

bool element::add_attribute(std::string the_attribute_name, std::string the_value) {
  return add_attribute((char *)the_attribute_name.c_str(), (char *)the_value.c_str());
}

// Adds an attribute to the attribute list
bool element::add_attribute(char* the_attribute_name, char* the_value)
{
    if(the_attribute_name == NULL)
        return false;
    if(strlen(the_attribute_name) == 0)
        return false;

    attribute* m_attribute = new attribute();
    m_attribute->set_name(the_attribute_name);
    if(the_value!=NULL)
        if(strlen(the_value)!= 0)
            m_attribute->set_value(the_value);
    m_attribute_list.push_back(m_attribute);
    m_attribute_iterator = m_attribute_list.begin();
    is_empty_attribute_list = false;
    return true;
}

bool element::add_attribute(attribute* ptr_attribute)
{
    if(ptr_attribute == NULL)
        return false;
    m_attribute_list.push_back(ptr_attribute);
    m_attribute_iterator = m_attribute_list.begin();
    is_empty_attribute_list = false;
    return true;
}

// Getter for the attribut by name
attribute* element::get_attribute(const char* the_name)
{
    it = m_attribute_list.begin();
    while(it != m_attribute_list.end())
    {
        if(strcmp((*it)->get_name(), the_name) == 0)
        {
            return ((attribute*) *it);
            break;
        }
        it++;
    }
    return NULL;
}

// Gets the next attribute in the list, starting from head and increments the counter
attribute* element::get_next_attribute()
{
    if(m_attribute_iterator != m_attribute_list.end())
    {
        attribute* res =  (attribute*) *m_attribute_iterator;
        m_attribute_iterator++;
        return res;
    }
    return NULL;
}

// Gets the previous attribute in the list and decrements the counter
attribute* element::get_previous_attribute()
{
    m_attribute_iterator--;
    if(m_attribute_iterator != m_attribute_list.begin())
    {
        attribute* res =   (attribute*)*m_attribute_iterator;
        return res;
    }
    return NULL;
}

char* element::get_value()
{
    return m_value;
}

bool element::set_value(const std::string the_value) {
  return set_value((char *)the_value.c_str());
}

bool element::set_value(const char* the_value)
{
    if(the_value != NULL)
    {
        int l;
        l=strlen(the_value)*sizeof(char);
        if(l == 0)
            return false;
        if(value_is_allocated)
            free(m_value);
        m_value = (char*)malloc(l+sizeof(char));
        memset(m_value,'\0',l+sizeof(char));
        strcpy(m_value,the_value);
        value_is_allocated = true;
        return true;
    }
    return false;
}

// //adds a child node to this one
element* element::add_child_element(element* child)
{
    if(child == NULL)
        return NULL;
    m_element_list.push_back(child);
    i = m_element_list.end();
    return (*(--i));
}

bool element::set_name(const std::string the_name) {
  return set_name((char *)the_name.c_str());
}

bool element::set_name(const char* the_name)
{
    if(the_name != NULL)
    {
        int l;
        l=strlen(the_name)*sizeof(char);
        if(l == 0)
            return false;
        if(name_is_allocated)
            free(m_name);
        m_name = (char*)malloc(l+sizeof(char));
        memset(m_name,'\0',l+sizeof(char));
        strcpy(m_name,the_name);
        name_is_allocated = true;
        return true;
    }
    return false;
}

element* element::get_parent()
{
    return m_parent;
}

bool element::set_parent(element* pnode)
{
    if(pnode == NULL)
        return false;
    m_parent = pnode;
    return true;
}

bool element::is_root()
{
    return (m_parent == NULL);
}

char* element::get_name()
{
    return m_name;
}

element* element::add_child_element()
{
    element* child = new element();
    child->set_parent(this);
    m_element_list.push_back(child);
    i = m_element_list.end();
    m_element_iterator = m_element_list.begin();
    return (*(--i));
}

element* element::get_next_child()
{
    if(m_element_iterator != m_element_list.end())
    {
        element* res =  (element*)*m_element_iterator;
        m_element_iterator++;
        return res;
    }
    return NULL;
}

element* element::get_first_child()
{
  m_element_iterator = m_element_list.begin();
  return (element*)*m_element_iterator;
}

bool element::has_child() {
  return !m_element_list.empty();
}

void element::reset_iterators()
{
  m_element_iterator = m_element_list.begin();
  m_attribute_iterator = m_attribute_list.begin();
}

element& element::operator=( element& the_element)
{
    set_name(the_element.get_name());
    set_value(the_element.get_value());
    m_element_list = the_element.get_element_list();
    m_attribute_list = the_element.get_attribute_list();
    return *this;
}

element* element::operator = ( element* the_element)
{
    set_name(the_element->get_name());
    set_value(the_element->get_value());
    m_element_list = the_element->get_element_list();
    m_attribute_list = the_element->get_attribute_list();
    return this;
}

std::list<attribute*> element::get_attribute_list()
{
    return m_attribute_list;
}

std::list<element*> element::get_element_list()
{
    return m_element_list;
}

bool element::set_comment(char* the_comment)
{
    set_name("Comment");
    set_value(the_comment);
    is_comment = true;
    return true;
}

bool element::is_comment_element()
{
    return is_comment;
}

bool element::set_as_pi()
{
    is_pi = true;
    return is_pi;
}

bool element::is_pi_element()
{
    return false;
}

} // ending namespace MinVR
