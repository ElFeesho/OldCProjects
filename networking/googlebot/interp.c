#include <stdio.h>
#include <string.h>
#include <memory.h>

char *interp_google_count(char *html_data)
{
	/* Here's to hoping that there isn't something so popular that it will generate > 999,999,999,999,999,999,999,999 results */
	char *result = NULL;
	
	/* Find the results in the big HTML char array NO PCRE HERE BITCHES! */
	html_data = strstr(html_data,"</b> of about <b>");
	if(html_data == NULL)
	{
		result = strdup("None found");
		return result;
	}
	html_data+=17;
	char *end_point = strstr(html_data, "</b>");
	end_point[0] = '\0';
	
	result = strdup(html_data);
	return result;
}
