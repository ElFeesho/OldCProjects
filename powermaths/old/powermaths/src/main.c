#include <gtk/gtk.h>
#include "splash.h"
#include "browser.h"

int main(int argc, char ** argv)
{
	gtk_init(&argc, &argv);
	g_set_application_name("Powermaths");
	gtk_rc_parse("style.rc");
	splash_show();
	browser_show();
	gtk_main();	
	return 0;
}
