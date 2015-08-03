#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
#include <deque>

#include "Cxml.h"
using namespace std;

// Based on ""Simple C++ XML Parser"" from the CodeProject article by "BratilaRazvan", 2010

deque<std::string> key_list;
std::string id_str = "id";


void fill_xml_tree(element* xml_node)
{
    attribute *a = NULL;
    if(xml_node == NULL)
        return;
    
    while((a=xml_node->get_next_attribute()) != NULL)
        key_list.push_back(a->get_value());
    
    while(true)
    {
        element *chld = xml_node->get_next_child();
        if(chld == NULL)
        {
            if(xml_node->get_value()!=NULL && 
               xml_node->get_attribute((char*)id_str.c_str())==NULL && 
               xml_node->get_value()!="")
            {
                std::stringstream key_buffer;
                key_buffer << key_list.back() << "/" << xml_node->get_name();
                std::string str_value = xml_node->get_value();
                std::cout << "  key = " << key_buffer.str() << std::endl 
                          << "value = " << str_value        << std::endl;
                // populate the settings map here...
            }
            return;
        }
        fill_xml_tree(chld);
    }
    key_list.pop_back();
}


int main(int argc,char** argv)
{
    std::string file_name;
    std::string line;
    std::cout << "A simple driver program for the MinVR XML config project." << std::endl;
    if(argc > 1)
    {
        std::string xml_string="";
        file_name = argv[1]; // the input file name
        std::cout << "Reading from file = " << file_name << std::endl;
        ifstream file(file_name.c_str());
        if(file.is_open())
        {
            std::stringstream buffer;
            buffer << file.rdbuf();
            xml_string = buffer.rdbuf()->str();
            
            Cxml *xml = new  Cxml();
            xml->parse_string((char*)xml_string.c_str());
            element *xml_node = xml->get_root_element();
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
