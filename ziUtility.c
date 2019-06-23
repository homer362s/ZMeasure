#include <userint.h>
#include <ansi_c.h>

#include "fixedziAPI.h"

#include "ZMeasure.h"
#include "measurementSetup.h"
#include "ziUtility.h" 

char* buildZurichPath(char* str, char* device, char* path)
{
	char d[32];
	char p[256];
	int len;
	
	if(device[0] == '/') {
		strcpy(d, device+1);
	} else {
		strcpy(d, device);
	}
	len = strlen(d);
	if (len && d[len-1] == '/') {
		d[len-1] = 0;
	}
	
	if(path[0] == '/') {
		strcpy(p, path+1);
	} else {
		strcpy(p, path);
	}
	len = strlen(p);
	if (len && p[len-1] == '/') {
		p[len-1] = 0;
	}
	
	sprintf(str, "/%s/%s", d, p);
	return str;
}





