/*
	Nothing more than the program entry point.

	Program starts and fires the main window, at
	this point, GTK takes over the program flow.
*/

#include <gtk/gtk.h>
#include <string.h>
#include "main_window.h"
#include "resources.h"

int main(int argc, char ** argv)
{
	printf("Prog: %s\n", argv[0]);
	working_dir = g_strdup(argv[0]);
	char *last_slash = strrchr(working_dir,'/');
	last_slash[1] = '\0';
	printf("Working dir: %s\n",working_dir);

	g_set_application_name("Nonlogic Pad - SVN $Revision: 19 $");
	gtk_init(&argc, &argv);
	gtk_rc_parse(resources_file("style.rc"));
	main_window_show();	
	gtk_main();
	return 0;
}
