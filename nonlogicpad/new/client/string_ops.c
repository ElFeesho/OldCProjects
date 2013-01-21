#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *sub_str(char *string, char delim, int num, size_t size)
{
	char *retString = malloc(size);
	memset(retString,'\0',size);
	int i;
	int dc = 0;
	int k = 0;
	for(i = 0;i<strlen(string);i++)
	{
		if(string[i]==delim)
		{
			dc++;
			if(k==0)
				continue;
			else
				return retString;
		}
		if(dc == num)
		{
			retString[k++] = string[i];
		}
	}
	if(retString[0] == '\0')
		return NULL;
	else
		return retString;
}
