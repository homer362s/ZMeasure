#include "str.h"


void deleteStringList(StringList list)
{
	// Free inner strings
	char* str;
	int i = 0;
	while((str = list[i])) {
		free(str);
		str = 0;
		i++;
	}
	
	// Free outer array
	free(list);
	list = 0;
}

void deleteString(String str)
{
	free(str);
}


StringList splitString(char* str, char delim)
{
	// Count the number of delimiter characters.
	// The number of strings is one more than this count
	size_t nStrs = 1;
	
	int j = 0;
	char c;
	while ((c = str[j])) {
		if (c == delim) {
			nStrs += 1;
		}
	
		j++;
	}
	
	// We need an array to hold one more element than nStrs (for the null terminator)
	StringList stringList = malloc((nStrs + 1) * sizeof(char*));
	stringList[nStrs] = 0;
	
	
	char* strstart = str;
	char* strend;
	size_t len;
	
	// Copy each substring into newly malloced strings
	for(size_t i = 0;i < nStrs;i++) {
		strend = strchr(strstart, delim);
		
		if (strend) {
			len = strend - strstart;
			stringList[i] = malloc((len + 1) * sizeof(char));
			stringList[i][len] = 0;
			memcpy(stringList[i], strstart, len);
		} else {
			len = strlen(strstart);
			stringList[i] = malloc((len+1) * sizeof(char));
			strcpy(stringList[i], strstart);
			break;
		}
		
		strstart = strend + 1;
	}
	
	return stringList;
}
