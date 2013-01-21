#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "net.h"

int main(int argc, char **argv)
{
	if(argc!=3)
	{
		fprintf(stderr, "Usage: %s USERNAME PASSWORD\n", argv[0]);
		return -1;
	}
	if(net_connect(argv[1], argv[2])<0)
		return -1;

	atexit(net_shutdown);

	for(;;)
	{
		const char *data = net_update();
		printf("%s\n",data);
		free((void*)data);
		sleep(1);
	}
	net_shutdown();
	return 0;
}
