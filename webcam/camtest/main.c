#define WIDTH 320
#define HEIGHT 240

#include <gtk/gtk.h>

#include "camera.h"
#include "gui.h"
#include "manipulate.h"
#include "network.h"

int main(int argc, char **argv)
{
	g_set_application_name("Camera Test - By Feesh! (A lot of code borrowed from Sonicsnap-gui-1.7)");
	gtk_init(&argc, &argv);
	
	manip_decompress_init();
	network_init(argv[1]);	
	camera_open(argv[2],WIDTH, HEIGHT);
	camera_start_capture();
	gui_init();
	
	gtk_main();
	return 0;
}
