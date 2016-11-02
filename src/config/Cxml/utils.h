#ifndef UTILS_H
#define UTILS_H

#include <locale>
#include <iostream>
#include <string>
#include <sstream>

#include "element.h"
using namespace std;

namespace MinVR {
  
char* concat(char *str, char c);
void copyx(char* dst ,char* src, int start, int stop);
void clean_str(char* str);
 
} // ending namespace MinVR
 
#endif // UTILS_H
