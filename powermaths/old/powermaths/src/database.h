#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <sqlite3.h>
#include <gtk/gtk.h>
#include <string.h>

extern int database_open(char *filename);
extern sqlite3_stmt *database_query(char *query);

#endif
