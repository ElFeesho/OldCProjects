#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *sub_str(char *string, char delim, int num, size_t size)
{
	char *ret_string = malloc(size);
	memset(ret_string,'\0',size);
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
				return ret_string;
		}
		if(dc == num)
		{
			ret_string[k++] = string[i];
		}
	}
	if(ret_string[0] == '\0')
	{
		free(ret_string); /* Still allocated some memory for it! so lets delete it and be a good fellow */
		return NULL;
	}
	else
		return ret_string;
}
