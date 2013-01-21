#define WIDTH 160
#define HEIGHT 128
#include <gtk/gtk.h> /* Things just gotta spicy! */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>          /* for videodev2.h */

#include <linux/videodev2.h>

#include "sonix_compress.h"
#include "bayer.h"

GTimer *fps_timer = NULL;

static void save_ppm(char *fname, unsigned char *data, unsigned int width, unsigned int height)
{
  int sz=width*height*3;
  unsigned char *img = g_malloc0(sz);
  memcpy(img, data, sz);
  FILE *f=fopen(fname,"wb");
  if (!f)
  {
    fprintf(stderr, "Cannot open file %s for writing", fname);
    g_free(img);
    return;
  }
  fprintf(f,"P6 %d %d %d\n", width, height, 255);
  int rv=fwrite(img, sz, 1, f);
  fclose(f);
  g_free(img);
}

static int xioctl(int fd, int32_t request, void *arg)
{
	int r;
	do
	{
		r = ioctl (fd, request, arg);
	} while (-1 == r && EINTR == errno);
	return r;
}


struct buffer {
	void * start;
	size_t length;
};

int n_buffers;

struct buffer *buffers = NULL;

int actual_w = 0;
int actual_h = 0;

/* Return a File descriptor to the webcam */
int vid_open_device(char *device)
{
	int handle = open("/dev/video0",O_RDWR);
	return handle;
}

/* Return caps */

struct v4l2_capability *vid_get_caps(int handle)
{
	struct v4l2_capability *cap = malloc(sizeof(struct v4l2_capability));
	memset(cap,'\0',sizeof(struct v4l2_capability));
	if(xioctl (handle, VIDIOC_QUERYCAP, cap)<0)
	{
		fprintf(stderr,"Failed to get Device Caps\n");
		return NULL;
	}
	if (!(cap->capabilities & V4L2_CAP_STREAMING)) 
	{
		fprintf (stderr, "Device does not support streaming\n");
		return NULL;
	}
	return cap;
}

void vid_set_crop_window(int handle, int x, int y, int width, int height)
{
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;	
	struct v4l2_format fmt;
	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (xioctl (handle, VIDIOC_CROPCAP, &cropcap)==0)
	{
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c.left = 0; /* reset to default */
		crop.c.top = 0;
		crop.c.width = width;
		crop.c.height = height;

		if (xioctl(handle, VIDIOC_S_CROP, &crop)<0) 
		{
			fprintf(stderr,"Failed to set cropping rectangle!\n");
		}

		fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		fmt.fmt.pix.width       = WIDTH; 
		fmt.fmt.pix.height      = HEIGHT;
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SN9C10X;
		fmt.fmt.pix.field       = V4L2_FIELD_NONE;

		if (xioctl(handle, VIDIOC_S_FMT, &fmt) < 0)
		{
			fprintf(stderr,"Failed to set the video format!\n");
			return;
		}
		printf("Width: %d\nHeight: %d\n",fmt.fmt.pix.width, fmt.fmt.pix.height);
		actual_w = fmt.fmt.pix.width;
		actual_h = fmt.fmt.pix.height;
	}
}

void vid_get_standards(int handle)
{
	struct v4l2_input input;
	struct v4l2_standard standard;

	memset (&input, 0, sizeof (input));

	if (-1 == xioctl (handle, VIDIOC_G_INPUT, &input.index)) 
	{
		perror ("VIDIOC_G_INPUT");
		exit (EXIT_FAILURE);
	}

	if (-1 == xioctl (handle, VIDIOC_ENUMINPUT, &input)) 
	{
		perror ("VIDIOC_ENUM_INPUT");
		exit (EXIT_FAILURE);
	}

	printf ("Current input %s supports:\n", input.name);

	memset (&standard, 0, sizeof (standard));
	standard.index = 0;

	while (0 == xioctl (handle, VIDIOC_ENUMSTD, &standard)) 
	{
		if (standard.id & input.std)
			printf ("%s\n", standard.name);

		standard.index++;
	}

	/* EINVAL indicates the end of the enumeration, which cannot be
	empty unless this device falls under the USB exception. */

	if (errno != EINVAL || standard.index == 0) 
	{
		perror ("VIDIOC_ENUMSTD");
		exit (EXIT_FAILURE);
	}
}

void init_mmap(int handle)
{
	struct v4l2_requestbuffers req;
	req.count               = 4;
	req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory              = V4L2_MEMORY_MMAP;

	if (xioctl (handle, VIDIOC_REQBUFS, &req)<0) 
	{
		if (EINVAL == errno) 
		{
			fprintf (stderr, "Device does not support memory mapping\n");
			exit (EXIT_FAILURE);
		} 
		else 
		{
			fprintf(stderr,"Device suffered a miscellaneous error\n");
		}
	}
	if (req.count < 2) 
	{
		fprintf (stderr, "Insufficient buffer memory on device\n");
		exit (EXIT_FAILURE);
	}
	
	buffers = calloc (req.count, sizeof (*buffers));

	if (!buffers) 
	{
		fprintf (stderr, "Out of memory\n");
		exit (EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) 
	{
		struct v4l2_buffer buf;
		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = n_buffers;

		if (xioctl (handle, VIDIOC_QUERYBUF, &buf) < 0)
		{
			fprintf(stderr, "Couldn't get buffer information\n");
			exit(EXIT_FAILURE);
		}
		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start =
		mmap (NULL, buf.length, PROT_READ | PROT_WRITE,MAP_SHARED,handle, buf.m.offset);
		if (MAP_FAILED == buffers[n_buffers].start)
		{
			fprintf(stderr,"Failed to mmap device\n");
		}
	}
}

void vid_start_capture(int handle)
{
	unsigned int i;
	enum v4l2_buf_type type;
	for (i = 0; i < n_buffers; ++i) 
	{
		struct v4l2_buffer buf;
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (xioctl (handle, VIDIOC_QBUF, &buf)<0)
		{
			fprintf(stderr,"Couldn't query buffers\n");
			exit(EXIT_FAILURE);
		}
		
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (xioctl (handle, VIDIOC_STREAMON, &type)<0)
		{
			fprintf(stderr,"Failed to start capturing\n");
			exit(EXIT_FAILURE);
		}
	}
}

int wc_handle = -1;

int vid_get_frame(GtkWidget *webcam)
{
	static double ltime = 0;
	static imgnum = 0;
	static int num = 0;
	struct v4l2_buffer buf;
	unsigned int i;

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (xioctl (wc_handle, VIDIOC_DQBUF, &buf)<0) 
	{
		fprintf(stderr,"Failed to read in device frame buffer\n");
		return 0;
	}
	
	//Handle image drawing here?
	unsigned char *out_put = malloc(actual_w*actual_h*3);
	unsigned char *outPut = malloc(actual_w*actual_h*3);
	
	sonix_decompress(actual_w, actual_h,buffers[buf.index].start,out_put);
	
	bayer2rgb24(outPut, out_put, WIDTH, HEIGHT);
	//save_ppm(g_strdup_printf("TEST%d.ppm",imgnum++),outPut,WIDTH,HEIGHT);
	gdk_draw_rgb_image(webcam->window,gdk_gc_new(webcam->window),0,0,WIDTH,HEIGHT,GDK_RGB_DITHER_NONE,outPut,WIDTH*3);
	num+=5;
	printf("New Num: %d\n", num);
	if (xioctl (wc_handle, VIDIOC_QBUF, &buf)<0)
	{
		fprintf(stderr,"Failed queueing next buffer\n");
		return 0;
	}

	if(ltime==0)
   {
      ltime = g_timer_elapsed(fps_timer,NULL);
   }
   else
   {
      double ctime = g_timer_elapsed(fps_timer,NULL);
      printf("Last update: %.2f FPS: %.2f\n",ctime-ltime, 1.0/(ctime-ltime));
      ltime = ctime;
   }

	return 1;
}

void vid_change_control(int fd)
{
	struct v4l2_queryctrl queryctrl;
	struct v4l2_control control;

	memset (&queryctrl, 0, sizeof (queryctrl));
	queryctrl.id = V4L2_CID_BRIGHTNESS;

	if (-1 == xioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) 
	{
		if (errno != EINVAL) 
		{
			perror ("VIDIOC_QUERYCTRL");
			exit (EXIT_FAILURE);
		}
		else
		{
			printf ("V4L2_CID_BRIGHTNESS is not supported\n");
		}
	}
	else if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) 
	{
		printf ("V4L2_CID_BRIGHTNESS is not supported\n");
	} 
	else
	{
		memset (&control, 0, sizeof (control));
		control.id = V4L2_CID_BRIGHTNESS;
		control.value = queryctrl.default_value;
		if (-1 == xioctl (fd, VIDIOC_S_CTRL, &control)) 
		{
			perror ("VIDIOC_S_CTRL");
			exit (EXIT_FAILURE);
		}
	}

	memset (&control, 0, sizeof (control));
	control.id = V4L2_CID_CONTRAST;

	if (0 == xioctl (fd, VIDIOC_G_CTRL, &control)) {
		printf("Contrast : %d\n",control.value);
		control.value += 10000;

		/* The driver may clamp the value or return ERANGE, ignored here */

		if (-1 == xioctl (fd, VIDIOC_G_CTRL, &control) && errno != ERANGE) 
		{
			perror ("VIDIOC_S_CTRL");
			exit (EXIT_FAILURE);
		}
	
	
	
		if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) 
		{
			printf ("V4L2_CID_CONTRAST is not supported\n");
		} 
		/* Ignore if V4L2_CID_CONTRAST is unsupported */
	}
	else if (errno != EINVAL) 
	{
		perror ("VIDIOC_G_CTRL");
		exit (EXIT_FAILURE);
	}
	else
	{
		printf("Setting contrast failed\n");
	}

}

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);
	GtkWidget *window, *webcam, *pause, *table, *contrast, *brightness;
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	webcam = gtk_drawing_area_new();
	
	pause = gtk_button_new_with_label("Pause");
	
	contrast = gtk_hscale_new_with_range(0,65535,1);
	brightness = gtk_hscale_new_with_range(0,65535,1);
	gtk_widget_set_size_request(webcam,WIDTH,HEIGHT);
	table = gtk_table_new(5,2,0);
	gtk_table_attach(GTK_TABLE(table),gtk_label_new("Believe me, it's part of something huge"),0,2,0,1,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(table),webcam,0,1,1,4,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(table),pause,0,1,4,5,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table),gtk_label_new("Contrast"),1,2,1,2,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(table),contrast,1,2,2,3,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(table),gtk_label_new("Brightness"),1,2,3,4,GTK_FILL,GTK_FILL,0,0);	
	gtk_table_attach(GTK_TABLE(table),brightness,1,2,4,5,GTK_FILL,GTK_FILL,0,0);
	
	gtk_container_add(GTK_CONTAINER(window),table);
	
	gtk_widget_show_all(window);
	
	wc_handle = vid_open_device("/dev/video0");
	
	struct v4l2_capability *caps = vid_get_caps(wc_handle);
	
	vid_set_crop_window(wc_handle, 0,0,WIDTH,HEIGHT);	
	sonix_decompress_init();
	init_mmap(wc_handle);
	vid_start_capture(wc_handle);
	sonix_decompress_init();
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(gtk_exit),NULL);
	g_idle_add((GSourceFunc)vid_get_frame,webcam);
	fps_timer = g_timer_new();
	g_timer_start(fps_timer);
	gtk_main();
	return 0;		
}
