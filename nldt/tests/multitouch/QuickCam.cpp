#include "QuickCam.h"
#include <iostream>
using namespace std;

QuickCam::QuickCam()
{
  /* Open camera */
	fd = open("/dev/video0", O_RDWR);
	if(fd < 0)
	{
    push_error("Could not connect to camera.");
		return;
	}
	
  /* Get capabilities */
	if(ioctl(fd, VIDIOCGCAP, &cap) < 0)
	{
    push_error("Could not read capabilities.");
		return;
	}
			
	/* TODO SET BUFFER HERE? KTHNXBAI - feesh*/

  /* Get window properties */
	ioctl(fd, VIDIOCGWIN, &win);
	
  /* Get picture properties */
	ioctl(fd, VIDIOCGPICT, &pic);
	
  /* Set picture properties */
	pic.contrast = 0x1FFF;
	pic.brightness = 0x7FFF;
	if(ioctl(fd, VIDIOCSPICT, &pic)<0)
	{
    push_error("Could not set picture mode.");
		return;
	}
	
  /* Create mbuf */
	if(ioctl(fd, VIDIOCGMBUF, &mbuf) < 0)
	{
    push_error("Failed to make mbuf.");
		return;
	}

  /* Memory map the device data */
	data = (int *) mmap(0, mbuf.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  /* Setup vid_mmap */
  frame = 0;
	vid_map.width = win.width;
	vid_map.height = win.height;
	vid_map.format = 4;
	vid_map.frame = 0;
	if(ioctl(fd, VIDIOCMCAPTURE, &vid_map) < 0)
	{
    push_error("Failed to capture!");
		return;
	}
}

QuickCam::~QuickCam()
{
  /* Close camera */
	close(fd);	
}

int QuickCam::get_width()
{
  return win.width;
}

int QuickCam::get_height()
{
  return win.height;
}

int *QuickCam::grab_image()
{
  /* Set vid_map to back buffer */
  vid_map.frame = 1 - frame;

  /* Start capturing back buffer */
  if(ioctl(fd, VIDIOCMCAPTURE, &vid_map) < 0)
  {
    push_error("Failed to capture.");
    return NULL;
  }

  /* Wait for current buffer to sync */
  while(ioctl(fd, VIDIOCSYNC, &frame) < 0 && errno == EINTR);

  /* Flip buffers */
  frame = vid_map.frame;

  /* Return image data */
  return (data + mbuf.offsets[1 - frame]);
}

