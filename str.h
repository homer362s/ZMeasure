#ifndef __STRING_H__
#define __STRING_H__

#include <ansi_c.h> 

// A StringList is a null terminated array of strings
typedef char** StringList;

// A String is a null terminated character array
typedef char* String;

void deleteStringList(StringList list);
void deleteString(String str);

// Split string into a list of strings, splitting at delim
StringList splitString(char* str, char delim);

#endif  /* ndef __string_H__ */
