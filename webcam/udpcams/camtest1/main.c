#define WIDTH 320
#define HEIGHT 240

#include <gtk/gtk.h>
#include <signal.h>

#include "camera.h"
#include "gui.h"
#include "manipulate.h"
#include "network.h"

void handle_sig(int signum)
{
	printf("THE GOGGLES DO NOTHING!\n");
}

int main(int argc, char **argv)
{
	signal(SIGPIPE,handle_sig);
	g_set_application_name("Camera Test - By Feesh! (A lot of code borrowed from Sonicsnap-gui-1.7)");
	gtk_init(&argc, &argv);
	
	manip_decompress_init();
	if(argc>1)
		network_init(argv[1]);	
	else
		network_init("localhost");
	
	if(argc>2)
		camera_open(argv[2],WIDTH, HEIGHT);
	else
		camera_open("/dev/video0",WIDTH, HEIGHT);
	gui_init();
	
	gtk_main();
	return 0;
}
