#include <gtk/gtk.h>
#include <gnet.h>

int main(int argc, char **argv)
{
	/*
		First of all initialise gtk and gnet
	*/
	
	gtk_init(&argc, &argv);
	
	gnet_init();
	
	/*
		Now set the application name
		
		This gives a title to all of the windows created by this program!
	*/
	g_set_application_name("FMSN : Feesh's MSN testbed");
	
	/*
		Create the first gui window
	*/
	
	gui_init();
	
	/* Hand over program flow to gtk_main() */
	gtk_main();
	
	return 0;
}
