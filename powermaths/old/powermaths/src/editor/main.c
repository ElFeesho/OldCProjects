/*
	PowerMaths Editor

	Creates SVGs that PowerMaths can use.
*/

#include <stdio.h>
#include <gtk/gtk.h>
#include "window.h"

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);
	g_set_application_name("PowerMaths Editor");
	window_show();
	
	gtk_main();
	return 0;
}
