#include <gtk/gtk.h>
#include "gui.h"

int main(int argc, char **argv)
{
	/*
		asim8086 is a simple 8086 assembler 
	*/

	g_set_application_name("ASIM8086");

	gtk_init(&argc, &argv);
	
	gui_init();
	
	gtk_main();

	return 0;
}
