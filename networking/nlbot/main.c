#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "network.h"

int main(int argc, char ** argv)
{
	//Hook up signals - mainly SIGINT
	signal(SIGTERM,shutdownCon);	
	//Initialise the network system
	if(!initialiseNetwork())
	{
		perror("Couldn't initialise network");
		printf("Exitting\n");
		exit(1);
	}
	if(openConnection())
	{
		perror("Couldn't open a connection");
		printf("Exitting\n");
		exit(1);
	}
	sendAuth();
	loadPlugins("plugList.lst");
	for(;;)
	{
		char *data = recvData();
		handleData(data);
		free(data);
	}
	return 0;
}
