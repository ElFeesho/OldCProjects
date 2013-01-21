#include <gtk/gtk.h>

#include "network.h"
#include "gui.h"

int main(int argc, char **argv)
{
	/* 
		Initialise GTK+ - this just takes any GTK+ specific
		command line arguements and gobbles em up, before 
		the user's program deals with it's own command lines.
	*/
	gtk_init(&argc, &argv);

	/* Nice to do: it sets the title of all windows to this by default */
	g_set_application_name("UDsPlat");

	/* Initialise Network and Gui */
	network_init();
	gui_init();

	/* Hand the flow of the program to GTK+ */
	gtk_main();
	return 0;
}
