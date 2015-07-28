#ifndef CXML_H
#define CXML_H

#include "utils.h"


class Cxml
{
  public:
    Cxml();
    ~Cxml();
    // parse a string containg xml code
    bool parse_string(char* xml_string);
  private:
    int m_cursor;
    int m_length;
    element* m_root_node;
    bool get_node(char* xml_string);
  public:
    element* get_root_node();
};
#endif // CXML_H
