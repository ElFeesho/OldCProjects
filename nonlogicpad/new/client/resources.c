#include <gtk/gtk.h>

char *working_dir;

char *resources_file(char *image)
{
	char *return_string = g_strdup_printf("%s%s",working_dir,image);
	return return_string;
}
