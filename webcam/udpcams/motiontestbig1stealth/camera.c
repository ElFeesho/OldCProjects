#define WIDTH 320
#define HEIGHT 240
#define FALSE 0
#define TRUE 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>

#include <linux/videodev.h>

#define CLEAR(x) memset (&(x), 0, sizeof (x))

#include "camera.h"

static struct video_mmap vid_map;
static struct video_mbuf vid_buf;
static struct video_picture vid_pinfo;
static unsigned char *wc_data;

 /* The camera's file descriptor */
static int fd = -1; 
static unsigned int w,h;

static char capturing = FALSE;

static void simple_switch(unsigned char *data)
{
	if(!data)
		return;
	int i = 0;
	for(i = 0;i<(vid_map.width*vid_map.height*3)-3;i+=3)
	{
		unsigned char tmp = data[i];
		data[i] = data[i+2];
		data[i+2] = tmp;
	}
}

static void errno_exit(const char *s)
{
  fprintf (stderr, "%s error %d, %s\n", s, errno, strerror (errno));
  exit (EXIT_FAILURE);
}

static int xioctl(int fd, int request, void *arg)
{
	int r;

	do
	{
		r = ioctl (fd, request, arg);
	} while (-1 == r && EINTR == errno);

	return r;
}

unsigned char *camera_read_image()
{
	static int frame = 0;
	if(ioctl(fd,VIDIOCMCAPTURE,&vid_map)==-1)
	{
		fprintf(stderr,"Dropped a frame!\n");
		return NULL;
	}

	while(ioctl(fd,VIDIOCSYNC,&frame)<0 && errno==EINTR);

	simple_switch(wc_data+vid_buf.offsets[frame]);
	frame=1-frame;
	vid_map.frame = 1-vid_map.frame;
	
	return (wc_data+vid_buf.offsets[1-frame]);
}

void camera_shutdown()
{
	close(fd);
	fd = -1;
}


int camera_open(const char *dev_name, int width, int height)
{
	w = width;
	h = height;
	struct stat st; 

	if (-1 == stat (dev_name, &st)) 
	{
		fprintf (stderr, "Cannot identify '%s': %d, %s\n", dev_name, errno, strerror (errno));
		exit (EXIT_FAILURE);
	}

	if (!S_ISCHR (st.st_mode)) 
	{
		fprintf (stderr, "%s is no device\n", dev_name);
		exit (EXIT_FAILURE);
	}

	fd = open (dev_name, O_RDWR | O_NONBLOCK, 0);

	if (-1 == fd) 
	{
		fprintf (stderr, "Cannot open '%s': %d, %s\n", dev_name, errno, strerror (errno));
		exit (EXIT_FAILURE);
	}
	
	struct video_capability vid_caps;
	if(ioctl(fd,VIDIOCGCAP,&vid_caps)==-1)
	{
		fprintf(stderr,"Failed to get the webcamera's capabiltiies\n");
		exit(EXIT_FAILURE);
	}

	/* Select video input, video standard and tune here. */

	struct video_window vid_cwin;
	if(ioctl(fd,VIDIOCGWIN,&vid_cwin)<0)
	{
		fprintf(stderr,"Failed to read video window capture jive\n");
		return 1;
	}

	vid_cwin.width = WIDTH;
	vid_cwin.height = HEIGHT;

	if(ioctl(fd,VIDIOCSWIN,&vid_cwin)<0)
	{
		fprintf(stderr,"Failed to read video window capture jive\n");
		return 1;
	}

	if(ioctl(fd,VIDIOCGPICT,&vid_pinfo)<0)
	{
		fprintf(stderr,"Failed to get picture mode\n");
	}

	vid_pinfo.contrast = 10485;
	vid_pinfo.brightness = 51772;
	vid_pinfo.palette = VIDEO_PALETTE_RGB24;
	
	if(ioctl(fd,VIDIOCSPICT,&vid_pinfo)<0)
	{
		fprintf(stderr,"Couldn't set picture mode!\n");
		return 1;
	}

	if(ioctl(fd, VIDIOCGMBUF,&vid_buf)<0)
	{
		fprintf(stderr,"Failed to get mbuf\n");
		return 1;
	}
	
	wc_data = mmap(0, vid_buf.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd,0);
	if((unsigned char*)wc_data == (unsigned char*)-1)
		return 1;

	vid_map.width = vid_cwin.width;
	vid_map.height = vid_cwin.height;
	vid_map.format = VIDEO_PALETTE_RGB24;
	vid_map.frame = 0;
	
	if(ioctl(fd,VIDIOCMCAPTURE,&vid_map)==-1)
	{
		fprintf(stderr,"Failed to get first capture\n");
		return 1;
	}
	vid_map.frame = 1;
}

int camera_set_contrast(int value)
{
	vid_pinfo.contrast = value;
		
	if(ioctl(fd,VIDIOCSPICT,&vid_pinfo)<0)
	{
		fprintf(stderr,"Couldn't set contrast!\n");
		return 1;
	}
}

int camera_set_brightness(int value)
{
	vid_pinfo.brightness = value;
		
	if(ioctl(fd,VIDIOCSPICT,&vid_pinfo)<0)
	{
		fprintf(stderr,"Couldn't set brightness!\n");
		return 1;
	}
}


