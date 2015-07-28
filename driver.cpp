#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
#include "Cxml.h"
using namespace std;

// Based on ""Simple C++ XML Parser"" from the CodeProject article by "BratilaRazvan", 2010

void fill_xml_tree(element* xml_node)
{
    attribute *a = NULL;
    string szText= "";
    if(xml_node == NULL)
        return;
    szText.append(xml_node->get_name());
    szText.append( " - ");
    szText.append(xml_node->get_value());
    while((a=xml_node->get_next_attribute()) != NULL)
    {
        szText.append(" _ ");
        szText.append(a->get_name());
        szText.append(" = ");
        szText.append(a->get_value());
    }
    std::cout << szText << endl;
    while(true)
    {
        element *chld = xml_node->get_next_child();
        if(chld == NULL)
            return;
        fill_xml_tree(chld);
    }
}


int main(int argc,char** argv)
{
    std::string file_name;
    std::string line;
    std::cout << "A simple driver program for the MinVR XML config project." << std::endl;
    if(argc > 1)
    {
        std::string xml_string="";
        file_name = argv[1]; // input file name
        std::cout << "Reading from file = " << file_name << std::endl;
        ifstream file(file_name.c_str());
        if(file.is_open())
        {
            std::stringstream buffer;
            buffer << file.rdbuf();
            xml_string = buffer.rdbuf()->str();
            
            Cxml *xml = new  Cxml();
            xml->parse_string((char*)xml_string.c_str());
            element *xml_node = xml->get_root_element()->get_next_child();
            fill_xml_tree(xml_node);
            delete xml_node;
            std::cout << std::endl;
        }
        else
        {
            std::cout << "Error opening file " << file_name << std::endl;
        }
    }
    else
    {
        std::cout << "Usage: ./driver xml_file.xml" << std::endl;
    }
    return 0;
}
