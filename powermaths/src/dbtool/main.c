#include <gtk/gtk.h>
#include "window.h"

int main(int argc, char ** argv)
{
	gtk_init(&argc, &argv);
	g_set_application_name("PowerMaths Database Tool");
#ifndef WIN32
	gtk_window_set_default_icon(gdk_pixbuf_new_from_file("pixmaps/pmlogo-ico.png",NULL));
#endif
	dbtool_window_create();
	gtk_main();
	return 0;
}
