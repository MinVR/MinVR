#include "utils.h"

namespace MinVR {

char* concat(char *str,char c)
{
    char buf[2] ={0};
    //char *ret = NULL;
    buf[0] = c;
    if((strlen(str) + (2*sizeof(char))) < 256)
    {
        strcat(str,buf);
        return str;
    }
    str = (char*)realloc(str,(strlen(str) + 2)*sizeof(char));
    strcat(str,buf);
    return str;
}

void copyx(char* dst,char* src,int start,int stop)
{
    memcpy(dst, &src[start], (stop - start + 1) *sizeof(char));
}

void clean_str(char* str)
{
    memset(str,0,strlen(str) * sizeof(char));
}

} // ending namespace MinVR
