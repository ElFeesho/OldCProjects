#include <gtk/gtk.h>
#include "desktop.h"

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);
	gtk_rc_parse("style.rc");
	g_set_application_name("Nonlogic Project");

	desktop_show();	
	
	gtk_main();
	return 0;
}
