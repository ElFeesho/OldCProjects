#include <sys/ioctl.h>		// for ioctl
#include <sys/types.h>		// req'd for videodev2.h
#include <asm/types.h>		// req'd for videodev2.h
#include <errno.h>		// for errno
#include <linux/videodev2.h>
#include <stdio.h>		// for fprintf
#include <string.h>		// for memset
#include <assert.h>

#include "controls.h"

static void enumerate_menu (int fd, struct v4l2_queryctrl *queryctrl)
{
  struct v4l2_querymenu querymenu;
  fprintf(stderr,"  Menu items:\n");

  memset (&querymenu, 0, sizeof (querymenu));
  querymenu.id = queryctrl->id;

  for (querymenu.index = queryctrl->minimum; querymenu.index <= (unsigned int)queryctrl->maximum; querymenu.index++) 
  {
    if (0 == ioctl (fd, VIDIOC_QUERYMENU, &querymenu)) 
    {
      fprintf (stderr,"    %s\n", querymenu.name);
    } 
    else 
    {
      perror ("VIDIOC_QUERYMENU");
      return;
    }
  }
}


char controls_enumerate(int fd)
{
  struct v4l2_queryctrl queryctrl;
  memset (&queryctrl, 0, sizeof (queryctrl));

  for (queryctrl.id = V4L2_CID_BASE; queryctrl.id < V4L2_CID_LASTP1; queryctrl.id++) 
  {
    if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl))
    {
      if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
        continue;

      fprintf(stderr,"Control %s (0x%x)\n", queryctrl.name, queryctrl.id);
      fprintf(stderr,"min=%d, max=%d, default=%d\n", queryctrl.minimum, queryctrl.maximum, queryctrl.default_value);

      if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
        enumerate_menu(fd, &queryctrl);
    }
    else 
    {
      if (errno == EINVAL)
        continue;

      perror ("VIDIOC_QUERYCTRL");
      return 0;
    }
  }

  for (queryctrl.id = V4L2_CID_PRIVATE_BASE;; queryctrl.id++) 
  {
    if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) 
    {
      if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
        continue;
      fprintf(stderr,"Custom control %s (0x%x)\n", queryctrl.name, queryctrl.id);
      fprintf(stderr,"min=%d, max=%d, default=%d\n", queryctrl.minimum, queryctrl.maximum, queryctrl.default_value);
      if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
        enumerate_menu(fd, &queryctrl);
    }
    else 
    {
      if (errno == EINVAL)
        break;
      perror ("VIDIOC_QUERYCTRL");
      return 0;
    }
  }
  return 1;
}


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

