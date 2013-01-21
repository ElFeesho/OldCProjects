#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "errors.h"

static sqlite3 *database_handle	 = NULL;

/* Prototypes */

int database_open(char *filename)
{
	if(sqlite3_open(filename,&database_handle)<0)
		return -1;
	return 0;
}

sqlite3_stmt *database_query(char *query)
{
	sqlite3_stmt *return_value;
	sqlite3_prepare(database_handle,query,strlen(query),&return_value,NULL);
	return return_value;
}
