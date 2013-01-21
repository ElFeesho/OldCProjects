#include <gtk/gtk.h>
#include "window.h"

int main(int argc, char ** argv)
{
	g_set_application_name("NLComic");
	gtk_init(&argc, &argv);
	window_show();
	gtk_main();
	return 0;
}
