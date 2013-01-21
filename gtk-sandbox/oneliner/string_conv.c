#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

gchar *strip_lines_tabs(gchar *string)
{
	gchar *return_string = g_malloc0(strlen(string));
	int i;
	for(i = 0; i<strlen(string);i++)
	{
		if(string[i]!='\n' && string[i]!='\t')
			return_string[i] = string[i];
		else
			return_string[i] = ' ';
	}
	return return_string;
}
