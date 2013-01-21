#include <gtk/gtk.h>
#include "codeedit.h"

int main(int argc, char **argv)
{
	g_set_application_name("CodeEdit"); //Creative aren't I
	gtk_init(&argc, &argv);
	ce_show(NULL);
	gtk_main();
	return 0;
}
