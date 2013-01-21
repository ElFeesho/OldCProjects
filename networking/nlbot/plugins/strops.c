#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "strops.h"

char *substr(char *string, char start, char finish)
{
	char *retString;
	retString = strdup(string);
	retString[finish]='\0';
	retString+=start;
	return retString;
}

char strpos(char *string, char chr, char num)
{
	char count = 0, pos = 0;
	while(string[0]!='\0')
	{
		if(string[0] == chr)
		{
			count++;
			if(count == num)
			{
				return pos;
			}
		}
		pos++;
		string++;
	}
	return -1;
}
