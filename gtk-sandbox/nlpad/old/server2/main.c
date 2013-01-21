/* Where all the magic really happens */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netdb.h>

#include <signal.h>

#include "log.h"
#include "network.h"

void signal_catcher(int signal) /* not quite a dream catcher */
{
	/* always close the log - flushes anything to the file that hasn't been flushed */
	log_writef(LOG_INFO,"Caught signal: %d, closing\n",signal);
	log_file_close();
}

int main(int argc, char ** argv)
{
	/* Begin logging */
	log_file_open();
	
	signal(SIGINT,signal_catcher);
	signal(SIGQUIT,signal_catcher);
	signal(SIGSEGV,signal_catcher);
	signal(SIGTERM,signal_catcher);
	
	network_init(10101);
	
	return 0;
}
