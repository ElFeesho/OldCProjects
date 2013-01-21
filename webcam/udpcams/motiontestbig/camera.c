#define WIDTH 640
#define HEIGHT 480

#include <gtk/gtk.h>
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

#include <asm/types.h>          /* for videodev2.h */

#include <linux/videodev2.h>


#include "manipulate.h"
#include "controls.h"
#include "bayer.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))

// For people who have not yet patched their /usr/include/linux/videodev2.h

#ifndef V4L2_PIX_FMT_SN9C10X
#define V4L2_PIX_FMT_SN9C10X  v4l2_fourcc('S','9','1','0')
#endif

#ifndef V4L2_PIX_FMT_SBGGR8
#define V4L2_PIX_FMT_SBGGR8   v4l2_fourcc('B','A','8','1')
#endif

#include "camera.h"

/*
 *  V4L2 interface to the sn9c102 webcam
 *  by Bram Stolk
 */
 
struct buffer
{
  void * start;
  size_t length;
};
 
/* The camera's file descriptor */
static int fd = -1; 
static unsigned int w,h;
static gboolean capturing = FALSE;
static unsigned char *uncompressed_src;
static unsigned char *rgb;
static unsigned int n_buffers;
static struct buffer *buffers;

static gboolean camera_set_compression();

static void errno_exit(const char *s)
{
  fprintf (stderr, "%s error %d, %s\n", s, errno, strerror (errno));
  exit (EXIT_FAILURE);
}


static int xioctl(int fd, int request, void *arg)
{
	/*
		Simple convenience function to keep polling the camera for a service until 
		it replies kindly.
	*/

	int r;

	do
	{
		r = ioctl (fd, request, arg);
	} while (-1 == r && EINTR == errno);

	return r;
}

gboolean camera_process_image(unsigned char *p)
{
	/*
		Convert the raw image into something usable. Using the bayer2rgb24
		conversion function.
	*/
	unsigned char *src = p;
	unsigned char *reader = NULL;
  
	manip_decompress(WIDTH, HEIGHT, src, (unsigned char*)uncompressed_src);
	reader = (unsigned char*)uncompressed_src;

	bayer2rgb24(rgb, reader, WIDTH, HEIGHT);
	return TRUE;
}


unsigned char *camera_read_image()
{

	/*
		Poll the webcam for a new image.
	*/

	struct v4l2_buffer buf;

	/* Prepare the buffer that will hold the image */
	CLEAR (buf);

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	int rv = ioctl(fd, VIDIOC_DQBUF, &buf);
	if (rv==-1)
	{
		switch (errno) 
		{
			case EAGAIN:
				return FALSE;
			default:
				errno_exit ("VIDIOC_DQBUF");
		}
	}
	
	/* 
		If we've got a buffer index greater than the amount of buffers
		we tripped up somewhere.
	*/
	assert (buf.index < n_buffers);

	/*
		Convert raw image into RGB24
	*/

	camera_process_image(buffers[buf.index].start);

	if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
		errno_exit ("VIDIOC_QBUF");

  return rgb;
}

gboolean camera_stop_capturing(void)
{
	capturing = FALSE;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl (fd, VIDIOC_STREAMOFF, &type))
		errno_exit ("VIDIOC_STREAMOFF");

	return TRUE;
}


gboolean camera_start_capture()
{
	/* Tell the webcam to start streaming data */
	capturing = TRUE;
	unsigned int i;
	for (i = 0; i < n_buffers; ++i) 
	{
		struct v4l2_buffer buf;

		CLEAR (buf);

		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = i;

		if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
			errno_exit ("VIDIOC_QBUF");
	}
		
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl (fd, VIDIOC_STREAMON, &type))
		errno_exit ("VIDIOC_STREAMON");

	return TRUE;
}

void camera_shutdown()
{
	unsigned int i;

	/* Free all the memory we are using right now for program exit! */

	for (i = 0; i < n_buffers; ++i)
		if (-1 == munmap (buffers[i].start, buffers[i].length))
			errno_exit ("munmap");

	free (buffers);

	free(rgb);
	free(uncompressed_src);

	// close device
	if (-1 == close (fd))
		errno_exit ("close");

	fd = -1;
}


void camera_open(const char *dev_name, int width, int height)
{
	// open device
	w = width;
	h = height;

	/* We stat the device filename to make sure it does exist and is a device */	
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


	struct v4l2_capability cap;
	if (-1 == xioctl (fd, VIDIOC_QUERYCAP, &cap)) 
	{
		if (EINVAL == errno) 
		{
			fprintf (stderr, "%s is no V4L2 device\n", dev_name);
			exit (EXIT_FAILURE);
		} 
		else 
		{
			errno_exit ("VIDIOC_QUERYCAP");
		}
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) 
	{
		fprintf (stderr, "%s is no video capture device\n", dev_name);
		exit (EXIT_FAILURE);
	}

	if (!(cap.capabilities & V4L2_CAP_STREAMING)) 
	{
		fprintf (stderr, "%s does not support streaming i/o\n", dev_name);
		exit(EXIT_FAILURE);
	}

	/* Select video input, video standard and tune here. */

	struct v4l2_cropcap cropcap;
	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	xioctl (fd, VIDIOC_CROPCAP, &cropcap);
	
	struct v4l2_crop crop;
	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	crop.c = cropcap.defrect; /* reset to default */

	xioctl(fd, VIDIOC_S_CROP, &crop);

	camera_set_compression();

	struct v4l2_requestbuffers req;

	CLEAR (req);

	req.count               = 4;
	req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory              = V4L2_MEMORY_MMAP;

	if (-1 == xioctl (fd, VIDIOC_REQBUFS, &req)) 
	{
		if (EINVAL == errno) 
		{
			fprintf (stderr, "%s does not support " "memory mapping\n", dev_name);
			exit (EXIT_FAILURE);
		}
		else 
		{
			errno_exit ("VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2)
	{
		fprintf (stderr, "Insufficient buffer memory on %s\n", dev_name);
		exit (EXIT_FAILURE);
	}

	buffers = (struct buffer*)calloc (req.count, sizeof (*buffers));

	if (!buffers) 
	{
		fprintf (stderr, "Out of memory\n");
		exit (EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) 
	{
		struct v4l2_buffer buf;

		CLEAR (buf);

		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = n_buffers;

		if (-1 == xioctl (fd, VIDIOC_QUERYBUF, &buf))
			errno_exit ("VIDIOC_QUERYBUF");

		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start = mmap(NULL, buf.length,PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

		if (MAP_FAILED == buffers[n_buffers].start)
			errno_exit ("mmap");
	}

	rgb = (char*)g_malloc0(w*h*3);
	uncompressed_src = (char*)g_malloc0(w*h*3);
}


void camera_set_gain(int v)
{
	static int max = 63;
	if(v>max) v = max;
	unsigned char val = v;
	controls_set_gain(fd, val);
}

void camera_set_red_balance(int v)
{
	static int max = 127;
	if(v>max) v = max;
	unsigned char val = v;
	controls_set_red_balance(fd, val);
}

void camera_set_blue_balance(int v)
{
	static int max = 127;
	if(v>max) v = max;
	unsigned char val = v;
	controls_set_blue_balance(fd, val);
}

void camera_set_white_balance_blue(int v)
{
	static int max = 63;
	if(v>max) v = max;
	unsigned char val = v;
	controls_set_white_balance_blue(fd, val);
}

void camera_set_white_balance_red(int v)
{
	static int max = 63;
	if(v>max) v = max;
	unsigned char val = v;
	controls_set_white_balance_red(fd, val);
}

void camera_set_exposure(int v)
{
	static int max = 255;
	if(v>max) v = max;
	unsigned char val = v;
	controls_set_exposure(fd, val);
}

void camera_set_auto_adjust(int v)
{
	static int max = 1;
	if(v>max) v = max;
	unsigned char val = v;
	controls_set_auto_adjust(fd, val);
}

void camera_set_vflip(int v)
{
	static int max = 1;
	if(v>max) v = max;
	unsigned char val = v;
	controls_set_white_balance_blue(fd, val);
}


static gboolean camera_set_compression()
{
	struct v4l2_format fmt;
	CLEAR (fmt);
	fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width       = WIDTH; 
	fmt.fmt.pix.height      = HEIGHT;
	fmt.fmt.pix.field       = V4L2_FIELD_NONE;

	fmt.fmt.pix.pixelformat = 0;

	if (-1 == xioctl (fd, VIDIOC_S_FMT, &fmt))
	{
		perror("VIDIOC_S_FMT");
		return FALSE;
	}

	if (fmt.fmt.pix.width != WIDTH || fmt.fmt.pix.height != HEIGHT)
	{
		fprintf(stderr,"After compression setting, wxh is %dx%d\n",fmt.fmt.pix.width,fmt.fmt.pix.height);
		fprintf(stderr,"Try upgrading your linux kernel headers.\n");
		fprintf(stderr,"Old versions of videodev2.h are bugged.]n");
	}
	//assert(fmt.fmt.pix.width == WIDTH);
	// Sometimes, pix.height is mangled
	// Geting a new linux/videodev2.h solved this for me:
	// the RW bits of CROPCAP ioctl were faulty in old headers.
	//assert(fmt.fmt.pix.height == 480);

	return TRUE;
}


