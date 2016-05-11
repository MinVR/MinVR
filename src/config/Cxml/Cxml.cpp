#include <iostream>
#include <stdlib.h>
#include "utils.h"
#include "Cxml.h"

Cxml::Cxml() : m_cursor(0),
               m_length(0)
{
    m_root_node = new element();
}

Cxml::~Cxml()
{
    delete(m_root_node);
}

// parse a string with the xml code
bool Cxml::parse_string(char* xml_string)
{
    if(xml_string == NULL)
        return false;
    m_length = strlen(xml_string);
    get_node(xml_string);
    return true;
}

bool Cxml::get_node(char* xml_string)
{
    int k = m_cursor;
    int j = 0;               //second level cursor;
    bool bCDATA = false;
    bool bIsPI = false;      //Set to true if the cursor is curently inside a processing instruction
    char cDelim = 0;
    char c = xml_string[k];
    const char COPEN = '<';
    const char CCLOSE = '>';
    const char CSLASH = '/';
    const char CSPACE = ' ';
    const char CQUOTE = '\'';
    const char CDQUOTE = '\"';
    const char CEQUAL = '=';
    const char CNEW = '\n';
    const char CTAB = '\t';
    const char CEXCLAMATION = '!';
    const char CMINUS = '-';
    const char CSQRPO = '[';
    const char CSQRPC = ']';
    const char SZCDATA[9] = "![CDATA[";
    const char CQM = '?';
    const char CRET = 13; // carriage return
    char *szNodeNameBuff = (char *)calloc(256,sizeof(char));
    char *szNodeValBuff  = (char *)calloc(256,sizeof(char));
    char *szAttrNameBuff = (char *)calloc(256,sizeof(char));
    char *szAttrValBuff  = (char *)calloc(256,sizeof(char));
    if(k >= m_length)
        return false;
    m_root_node->set_name("XML_DOC");
    m_root_node->set_value(""); // just in case we want to access it later...
    element* Current = m_root_node->add_child_element();

    // We are going to march through the file, one character at a time
    // (k counts the file position, and sort the characters into XML
    // elements as they appear.    
    while(k<m_length)
    {
        c = xml_string[k];
        if(c == CNEW || c == CTAB || c == CRET) 
        {
            k++;  // This is white space.  Eat it.
            continue;
        }
        if(c == COPEN) // Found an "open" character (<)
        {
            if(xml_string[k+1] == CEXCLAMATION && xml_string[k+2] == CMINUS && xml_string[k+3] == CMINUS) // this is a comment
            { //the comment section
                clean_str(szAttrValBuff);
                k+=4;
                c = xml_string[k];
                while(!(xml_string[k] == CMINUS && xml_string[k+1] == CMINUS && xml_string[k+2] == CCLOSE)) // Find the end of the comment.
                {
                    szAttrNameBuff = concat(szAttrNameBuff, c);
                    c = xml_string[++k];
                }
                k+=3;
                // MODIFIED BY TS
                // We don't really want to process comments at all, so just
                // discard them.
                //if(Current->get_name() != NULL)       //we have set this node, navigate to a child of it
                //    Current = Current->add_child_element();
                //Current->set_comment(szAttrNameBuff); //set it as a comment node
                //Current = Current->get_parent();      //return to the previous level
                continue;
            }
            while(k+10 < m_length && j < 9)
            {
                if(xml_string[k+1+j] != SZCDATA[j])
                    break;
                j++;
                if(j==8)
                {
                    // This is definitely a CDATA section
                    k = k + j;
                    int start = k;
                    while((k + 3) < m_length && xml_string[k+1] != CSQRPC && xml_string[k+2] != CSQRPC && xml_string[k+3] != CCLOSE)
                    {
                        k++;
                    }
                    int stop = k;
                    k+=5;
                    char* buffer = (char*)calloc(stop - start + 2, sizeof(char));
                    copyx(buffer, xml_string, start, stop);
                    Current->set_value(buffer);
                    free(buffer);
                    j = 0;
                    bCDATA = true;
                    break;
                }
            }
            if(bCDATA)
            {
                bCDATA = false;
                continue;
            }
            clean_str(szNodeNameBuff);
            if(xml_string[k+1] == CSLASH)
            {// This is a close tag, hopefully for the last opened node.
                Current = Current->get_parent();
                k++;
                while(xml_string[k] != CCLOSE)
                {
                    k++;
                }
                k++;
                continue;
            }
            if(xml_string[k+1] == CQM)
            {
                c = xml_string[++k];
                bIsPI = true;
            }
            // If we are here, this is an open tag. So create a new node.
            c = xml_string[++k];
            while(c != CSLASH && c != CSPACE && c != CCLOSE)
            {//Loop until the node name has been entirely read.
                if(c != CNEW && c != CTAB && c != CRET)
                    szNodeNameBuff = concat(szNodeNameBuff,c);
                c = xml_string[++k];
            }
            if(Current != NULL)   // this node is set, navigate to a child of it
                if(Current->get_name() != NULL) 
                    Current = Current->add_child_element();

            Current->set_name(szNodeNameBuff);
            // We are inside the element tag, though at the end of the
            // element name.  Therefore, if there's a space here,
            // there must be an attribute coming.
            while(c == CSPACE)
            {
                c = xml_string[++k];
                if(c == CSLASH)
                {
                    break;
                }
                if(c == CQM && bIsPI)
                {
                    break;
                }
                clean_str(szAttrNameBuff);
                clean_str(szAttrValBuff);

                // Get ready for a new attribute.
                attribute* pA = new attribute();

                // Accumulate characters until there is an equal sign, when
                // we'll know the attribute name has been read.
                while(c != CEQUAL)
                {
                    if (c != CNEW && c != CTAB && c != CRET)
                        szAttrNameBuff = concat(szAttrNameBuff, c);
                    c = xml_string[++k];
                }

                // Is the attribute value in single or double quotes?
                c = xml_string[++k];
                if (c == CQUOTE || c == CDQUOTE)
                {
                    cDelim = c;
                    c = xml_string[++k];
                }

                // Accumulate characters until the next delimiter, when
                // we'll know the attribute value has been read.
                while(c != cDelim && cDelim != 0)
                {
                    if(c != CNEW && c != CTAB && c != CRET)
                        szAttrValBuff = concat(szAttrValBuff, c);
                    c = xml_string[++k];
                }

                // Reset the delimiter indicator, and advance one character.
                cDelim = 0;
                c = xml_string[++k];

                // Set the name and value of our new attribute.
                pA->set_name(szAttrNameBuff);
                pA->set_value(szAttrValBuff);
                Current->add_attribute(pA);
            } // Repeat if the next character is a space.

            if(c == CSLASH) // A slash here indicates a singleton element.
            {
                Current = Current->get_parent();
                c=xml_string[++k];
                while(c != CCLOSE)
                {
                    c = xml_string[++k];
                }
            }
            if(c == CQM && bIsPI)
            {
                Current->set_as_pi();
                Current = Current->get_parent();
                c=xml_string[++k];
                bIsPI = false;
                while(c != CCLOSE)
                {
                    c = xml_string[++k];
                }
            }
            if(c == CCLOSE)
            {
                ;
            }
        }
        // If there isn't an open character (<) here, this is the
        // contents of an element.  Record the characters.  Originally
        // (pre-TS), this would not allow leading spaces or leading
        // slashes (/) in an element. Don't know why, but these have
        // been tentatively commented out by TS 6/11/16
        if(c != COPEN && c != CCLOSE /*&& c != CSLASH*//* && c != CSPACE*/)
        {
            clean_str(szNodeValBuff);
            while(c != COPEN)
            {
                if(c != CNEW && c != CTAB && c != CRET/* && c != CSPACE*/)
                    szNodeValBuff =concat(szNodeValBuff,c);
                c = xml_string[++k];
            }
            Current->set_value(szNodeValBuff);
            continue;
        }
        k++;
    }
    free(szNodeNameBuff);
    free(szNodeValBuff);
    free(szAttrNameBuff);
    free(szAttrValBuff);
    return true;
}


element* Cxml::get_root_element()
{
    return m_root_node;
}

