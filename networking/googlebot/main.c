#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "con.h" 			/* Connection abstraction */
#include "parse.h"		/* Parse input to form a query string for the goog */
#include "interp.h"		/* Interpret results */
#include "http.h"			/* For actually making the http request to send */
int main(int argc, char **argv)
{
	/* 
		Normally I would write this in C++ for std::string 
		But I'm like so totally renegade.
	*/

	if(argc==1)
	{
		fprintf(stderr, "Usage: %s \"QUERY STRING\"\n",argv[0]);
		return 1;
	}
	
	int google_con = con_new("www.google.com");
	if(google_con<0) 
	{
		fprintf(stderr, "Quitting due to previous errors\n");
		return 1;
	}

	char *query = parse_query(argv[1]);
	
	char *http_header = http_create_request("www.google.com", query);

	free(query);
	
	con_send_data(google_con,http_header,strlen(http_header));

	free(http_header);

	char *html_data = con_recv_data(google_con);

	char *result = interp_google_count(html_data);
	
	free(html_data);

	printf("Results: %s\n",result);

	free(result);

	return 0;
}


