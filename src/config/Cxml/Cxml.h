#ifndef CXML_H
#define CXML_H

#include "utils.h"
#include <stdlib.h>

namespace MinVR {

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
    element* get_root_element();
};

} // ending namespace MinVR
 
#endif // CXML_H

// TODO list -- Needed improvements to the Cxml parser.
//
//   - Limited entity support.  At least the basics: &, <, >, etc.
//
//   - Make a "printXML() method like printStructure(), for debugging.
//
//   - Support comments (again).  That is, restore the comment-processing
//     calls, and persuade the data index code to ignore them.
//
//   - Real entity support, where they can be defined.
//
//   - CDATA support (maybe have this already?)
//
//   - Add hook for pre-processing with xsltproc, and validation to a schema.
//
//   - Tidying and better documentation.
//
//   - Better errors.
//
