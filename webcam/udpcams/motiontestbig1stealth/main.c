#define WIDTH 320
#define HEIGHT 240

#include <stdio.h>
#include <signal.h>

#include "camera.h"
#include "gui.h"
#include "network.h"

void handle_sig(int signum)
{
	printf("THE GOGGLES DO NOTHING!\n");
}

int main(int argc, char **argv)
{
	signal(SIGPIPE,handle_sig);
	
	if(argc>1)
		network_init(argv[1]);	
	else
		network_init("localhost");
	
	if(argc>2)
		camera_open(argv[2],WIDTH, HEIGHT);
	else
		camera_open("/dev/video0",WIDTH, HEIGHT);



	while(update_camera());
	return 0;
}
