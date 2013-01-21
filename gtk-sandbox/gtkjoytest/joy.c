#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/joystick.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

static int joy_axis[6] = { 0 }; 
static int joy_button[128] = { 0 };

fd_set joystick;

int fd;

void joy_open(char *device)
{
	fd = open(device,O_RDONLY);
	if(fd<0)
	{
		fprintf(stderr, "Failed openning joystick device\n");
		exit(0);
	}

	int opts = fcntl(fd,F_GETFL,0);
	opts |= O_NONBLOCK;
	if(fcntl(fd,F_SETFL,opts)!=0)
	{
		fprintf(stderr, "Failed to set nonblocking\n");
		exit(0);
	}
}

gpointer joy_poll(gpointer data)
{
	for(;;)
	{
		FD_ZERO(&joystick);
		FD_SET(fd, &joystick);
		struct timeval time_out;
		time_out.tv_sec = 0;
		time_out.tv_usec = 100;	
		int am = select(fd+1,&joystick,NULL,NULL,&time_out);
		if(am > 0)
		{
			if(FD_ISSET(fd, &joystick))
			{
				struct js_event e;
				if(read(fd,&e,sizeof(struct js_event)) > 0)
				{
					if(e.type == JS_EVENT_AXIS)
					{
						joy_axis[e.number] = e.value;
					}
					else if(e.type == JS_EVENT_BUTTON)
					{
						joy_button[e.number] = e.value;
					}
				}
			}
		}
	}
}

int *joy_get_axis()
{
	return joy_axis;
}

int *joy_get_button()
{
	return joy_button;
}
