#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

char *http_create_request(char *address, char *file)
{
	/*
		Our header will look like: 

		GET file HTTP/1.1\r\n
		Host: address\r\n
		User-Agent: Mozilla/5.0 (Linux: X11:UTF-8)\r\n\r\n

		68 chars + file and address strings +1 for \0 terminator
	*/
	char *http_header = malloc(strlen(address)+strlen(file)+69); 
	memset(http_header, '\0', strlen(address)+strlen(file)+69);
	
	sprintf(http_header,"GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/5.0 (Linux: X11:UTF-8)\r\n\r\n",file, address);

	return http_header;
}
