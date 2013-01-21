/*
	Nothing more than the program entry point.

	Program starts and fires the main window, at
	this point, GTK takes over the program flow.
*/

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <gnet.h>
#include "main_window.h"
#include "resources.h"
#include "preferences.h"

int main(int argc, char ** argv)
{
	working_dir = g_strdup(argv[0]);
#ifndef __WIN32__
	char *last_slash = strrchr(working_dir,'/');
#else
	char *last_slash = strrchr(working_dir,'\\');
#endif
	last_slash[1] = '\0';

	/* Get the config for NLPad - doi! Config contains nickname, last channel */
//	config_load();

	gnet_init();
	g_set_application_name("Tnl Pad"); // Witness the rebirth
	gtk_init(&argc, &argv);
	main_window_show();
	gtk_main();
	return 0;
}
