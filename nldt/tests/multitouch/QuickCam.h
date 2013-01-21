#ifndef __QUICKCAM_H__
#define __QUICKCAM_H__

#include "Camera.h"
#include <errno.h>
#include <sys/mman.h>
#include <memory.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev.h>
#include <stdint.h>

class QuickCam : Camera
{
public:
  QuickCam();
  ~QuickCam();
  int get_width();
  int get_height();
  int *grab_image();
private:
  int fd, frame, pos, *data;
	struct video_capability cap;
  struct video_picture pic;
	struct video_window win;
	struct video_mbuf mbuf;
	struct video_mmap vid_map;
};

#endif

