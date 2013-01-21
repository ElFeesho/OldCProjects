#include <sys/ioctl.h>		// for ioctl
#include <sys/types.h>		// req'd for videodev2.h
#include <asm/types.h>		// req'd for videodev2.h
#include <errno.h>		// for errno
#include <linux/videodev2.h>
#include <stdio.h>		// for fprintf
#include <string.h>		// for memset
#include <assert.h>

#include "controls.h"

int controls_get_max_gain(int fd)
{
	struct v4l2_queryctrl queryctrl;
	memset (&queryctrl, 0, sizeof (queryctrl));
	queryctrl.id = V4L2_CID_GAIN;
	int rv=ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl);
	if (rv)
		perror("ioctl VIDIOC_QUERYCTRL failed for V4L2_CID_GAIN");
	else
		assert(!(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED));
	return queryctrl.maximum;
}


int controls_get_gain(int fd)
{
	struct v4l2_control control;
	memset (&control, 0, sizeof (control));
	control.id = V4L2_CID_GAIN;

	int rv = ioctl(fd, VIDIOC_G_CTRL, &control);
	if (rv)
		perror("VIDIOC_G_CTRL");

	return control.value;
}

void controls_set_gain(int fd, int v)
{
	struct v4l2_control control;
	memset (&control, 0, sizeof (control));
	control.id = V4L2_CID_GAIN;
	control.value = v;

	int rv=ioctl(fd, VIDIOC_S_CTRL, &control);
	if (rv==-1)
	{
		perror("VIDEOC_S_CTRL");
		fprintf(stderr,"control.id=%x\n", control.id);
		fprintf(stderr,"control.value=%d\n", control.value);
	}
}

void controls_set_blue_balance(int fd, int v)
{
	struct v4l2_control control;
	memset (&control, 0, sizeof (control));
	control.id = V4L2_CID_BLUE_BALANCE;
	control.value = v;

	int rv=ioctl(fd, VIDIOC_S_CTRL, &control);
	if (rv==-1)
	{
		perror("VIDEOC_S_CTRL");
		fprintf(stderr,"control.id=%x\n", control.id);
		fprintf(stderr,"control.value=%d\n", control.value);
	}
}

int controls_get_blue_balance(int fd)
{
	struct v4l2_control control;
	memset (&control, 0, sizeof (control));
	control.id = V4L2_CID_BLUE_BALANCE;

	int rv = ioctl(fd, VIDIOC_G_CTRL, &control);
	if (rv)
		perror("VIDIOC_G_CTRL");

	return control.value;
}

void controls_set_white_balance_blue(int fd, int v)
{
	struct v4l2_control control;
	memset (&control, 0, sizeof (control));
	control.id = 0x98090d;
	control.value = v;

	int rv=ioctl(fd, VIDIOC_S_CTRL, &control);
	if (rv==-1)
	{
		perror("VIDEOC_S_CTRL");
		fprintf(stderr,"control.id=%x\n", control.id);
		fprintf(stderr,"control.value=%d\n", control.value);
	}
}

int controls_get_white_balance_blue(int fd)
{
	struct v4l2_control control;
	memset (&control, 0, sizeof (control));
	control.id = 0x98090d;

	int rv = ioctl(fd, VIDIOC_G_CTRL, &control);
	if (rv)
		perror("VIDIOC_G_CTRL");

	return control.value;
}

void controls_set_white_balance_red(int fd, int v)
{
	struct v4l2_control control;
	memset (&control, 0, sizeof (control));
	control.id = 0x980910;
	control.value = v;

	int rv=ioctl(fd, VIDIOC_S_CTRL, &control);
	if (rv==-1)
	{
		perror("VIDEOC_S_CTRL");
		fprintf(stderr,"control.id=%x\n", control.id);
		fprintf(stderr,"control.value=%d\n", control.value);
	}
}

int controls_get_white_balance_red(int fd)
{
	struct v4l2_control control;
	memset (&control, 0, sizeof (control));
	control.id = 0x980910;

	int rv = ioctl(fd, VIDIOC_G_CTRL, &control);
	if (rv)
		perror("VIDIOC_G_CTRL");

	return control.value;
}

void controls_set_auto_adjust(int fd, int v)
{
	struct v4l2_control control;
	memset (&control, 0, sizeof (control));
	control.id = 0x980912;
	control.value = v;

	int rv=ioctl(fd, VIDIOC_S_CTRL, &control);
	if (rv==-1)
	{
		perror("VIDEOC_S_CTRL");
		fprintf(stderr,"control.id=%x\n", control.id);
		fprintf(stderr,"control.value=%d\n", control.value);
	}
}



int controls_get_auto_adjust(int fd)
{
	struct v4l2_control control;
	memset (&control, 0, sizeof (control));
	control.id = 0x980912;

	int rv = ioctl(fd, VIDIOC_G_CTRL, &control);
	if (rv)
		perror("VIDIOC_G_CTRL");

	return control.value;
}

void controls_set_red_balance(int fd, int v)
{
	struct v4l2_control control;
	memset (&control, 0, sizeof (control));
	control.id = V4L2_CID_RED_BALANCE;
	control.value = v;

	int rv=ioctl(fd, VIDIOC_S_CTRL, &control);
	if (rv==-1)
	{
		perror("VIDEOC_S_CTRL");
		fprintf(stderr,"control.id=%x\n", control.id);
		fprintf(stderr,"control.value=%d\n", control.value);
	}
}

int controls_get_red_balance(int fd)
{
	struct v4l2_control control;
	memset (&control, 0, sizeof (control));
	control.id = V4L2_CID_RED_BALANCE;

	int rv = ioctl(fd, VIDIOC_G_CTRL, &control);
	if (rv)
		perror("VIDIOC_G_CTRL");

	return control.value;
}

void controls_set_vflip(int fd, char on)
{
	struct v4l2_control control;
	memset (&control, 0, sizeof (control));
	control.id = V4L2_CID_VFLIP;
	control.value = on;

	int rv=ioctl(fd, VIDIOC_S_CTRL, &control);
	if (rv==-1)
	{
		perror("VIDEOC_S_CTRL");
		fprintf(stderr,"control.id=%x\n", control.id);
		fprintf(stderr,"control.value=%d\n", control.value);
	}
}

int controls_get_vflip(int fd)
{
	struct v4l2_control control;
	memset (&control, 0, sizeof (control));
	control.id = V4L2_CID_VFLIP;

	int rv = ioctl(fd, VIDIOC_G_CTRL, &control);
	if (rv)
		perror("VIDIOC_G_CTRL");

	return control.value;
}

void controls_set_exposure(int fd, int v)
{
  struct v4l2_control control;
  memset (&control, 0, sizeof (control));
  control.id = V4L2_CID_EXPOSURE;
  control.value = v;

  int rv=ioctl(fd, VIDIOC_S_CTRL, &control);
  if (rv==-1)
  {
    perror("VIDEOC_S_CTRL");
    fprintf(stderr,"control.id=%x\n", control.id);
    fprintf(stderr,"control.value=%d\n", control.value);
  }
}

int controls_get_exposure(int fd)
{
	struct v4l2_control control;
	memset (&control, 0, sizeof (control));
	control.id = V4L2_CID_EXPOSURE;

	int rv = ioctl(fd, VIDIOC_G_CTRL, &control);
	if (rv)
		perror("VIDIOC_G_CTRL");

	return control.value;
}

int controls_get_jpegcompr(int fd)
{
  struct v4l2_jpegcompression compr;
  int rv=ioctl(fd, VIDIOC_G_JPEGCOMP, &compr);
  if (rv==-1)
  {
    perror("VIDIOC_G_JPEGCOMP");
    return -1;
  }
//  fprintf(stderr,"quality=%d\n", compr.quality);
  return compr.quality;
}


void controls_set_jpegcompr(int fd, int v)
{
  assert(v==0 || v==1);
  struct v4l2_jpegcompression compr;
  int rv=ioctl(fd, VIDIOC_G_JPEGCOMP, &compr);
  if (rv==-1)
  {
    perror("VIDIOC_G_JPEGCOMP");
    return;
  }
  compr.quality = v;
  rv=ioctl(fd, VIDIOC_S_JPEGCOMP, &compr);
  if (rv==-1)
  {
    perror("VIDIOC_S_JPEGCOMP");
  }
//  fprintf(stderr,"compression is now: %d\n", controls_get_jpegcompr(fd));
}

