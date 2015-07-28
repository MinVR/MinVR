#include "Cxml.h"
#include <string.h>
#include <malloc.h>
#include "utils.h"


Cxml::Cxml() : m_cursor(0),
               m_length(0)
{
    m_root_node = new element();
}

Cxml::~Cxml()
{
    delete(m_root_node);
}

// parse a string containg xml code
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
    const char CRET = 13; //carriage return
    char *szNodeNameBuff = (char *)calloc(256,sizeof(char));
    char *szNodeValBuff = (char *)calloc(256,sizeof(char));
    char *szAttrNameBuff = (char *)calloc(256,sizeof(char));
    char *szAttrValBuff = (char *)calloc(256,sizeof(char));
    if(k >= m_length)
        return false;
    m_root_node->set_name("XML_DOC");
    element* Current = m_root_node->add_child_node();
    while(k<m_length)
    {
        c = xml_string[k];
        if(c == CNEW || c == CTAB || c == CRET)
        {
            k++;
            continue;
        }
        if(c == COPEN)
        {
            if(xml_string[k+1] == CEXCLAMATION && xml_string[k+2] == CMINUS && xml_string[k+3] == CMINUS)//it is a comment
            {//Comment section
                clean_str(szAttrValBuff);
                k+=4;
                c = xml_string[k];
                while(!(xml_string[k] == CMINUS && xml_string[k+1] == CMINUS && xml_string[k+2] == CCLOSE))
                {
                    szAttrNameBuff = concat(szAttrNameBuff, c);
                    c = xml_string[++k];
                }
                k+=3;
                if(Current->get_name() != NULL)//we have seted this node, navigate to a child of it
                    Current = Current->add_child_node();
                Current->set_comment(szAttrNameBuff);//set it as a comment node
                Current = Current->get_parent();//return to the previous level
                continue;
            }
            while(k+10 < m_length && j < 9)
            {
                if(xml_string[k+1+j] != SZCDATA[j])
                    break;
                j++;
                if(j==8)
                {
                    //definetly a CDATA section follows
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
            {//closing tag for the last opend node
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
            // open tag. It means we have a node so we create it
            c = xml_string[++k];
            while(c != CSLASH && c != CSPACE && c != CCLOSE)
            {//loops until the node name has been entirely read
                if(c != CNEW && c != CTAB && c != CRET)
                    szNodeNameBuff = concat(szNodeNameBuff,c);
                c = xml_string[++k];
            }
            if(Current != NULL)//we have seted this node, navigate to a child of it
                if(Current->get_name() != NULL)//we have seted this node, navigate to a child of it
                    Current = Current->add_child_node();
            
            Current->set_name(szNodeNameBuff);
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
                attribute* pA = new attribute();
                while(c != CEQUAL)
                {//loops until the attribute name has been entirely read
                    if(c != CNEW && c != CTAB && c != CRET)
                        szAttrNameBuff = concat(szAttrNameBuff, c);
                    c = xml_string[++k];
                }
                c = xml_string[++k];
                if(c == CQUOTE || c == CDQUOTE)
                {
                    cDelim = c;
                    c = xml_string[++k];
                }
                while(c != cDelim && cDelim != 0)
                {//loops until the attribute value has been entirely read
                    if(c != CNEW && c != CTAB && c != CRET)
                        szAttrValBuff = concat(szAttrValBuff, c);
                    c = xml_string[++k];
                }
                cDelim = 0;
                c = xml_string[++k];
                pA->set_name(szAttrNameBuff);
                pA->set_value(szAttrValBuff);
                Current->add_attribute(pA);
            }
            if(c == CSLASH)
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
        if(c != COPEN && c != CCLOSE && c != CSLASH/* && c != CSPACE*/)
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


element* Cxml::get_root_node()
{
    return m_root_node;
}

