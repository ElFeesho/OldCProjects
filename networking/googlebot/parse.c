#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

static int parse_check_char(char letter)
{
	if(letter<'A' || letter >'z')
	{
		return 3;
	}
	return 1;
}

char *parse_query(char *query)
{
	char *result_str;

	/* 
		First off, find out what we are dealing with 
		Go through the query, identifying chars that will be converted
		to a 3 char html hex code.
	*/

	int result_len = 10; /* /search?q=  TEN CHARS for that! */
	int i = 0;
	for(;i<strlen(query);i++)
	{
		result_len += parse_check_char(query[i]);
	}

	result_str = malloc(result_len+1); /* +1 for \0 */
	memset(result_str, '\0', result_len);
	strcpy(result_str, "/search?q=");	
	for(i=0;i<strlen(query);i++)
	{
		if(parse_check_char(query[i])==3)
			sprintf(result_str, "%s%%%X",result_str, query[i]);
		else
		sprintf(result_str, "%s%c",result_str, query[i]);			
	}
	
	return result_str;
}
