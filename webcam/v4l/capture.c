/*
 *  V4L video capture example
 *
 *  This program can be used and distributed without restrictions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

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

#define CLEAR(x) memset (&(x), 0, sizeof (x))

typedef enum {
	IO_METHOD_READ,
	IO_METHOD_MMAP,
	IO_METHOD_USERPTR,
} io_method;

struct buffer {
        void *                  start;
        size_t                  length;
};

static char *           dev_name        = NULL;
static io_method	io		= IO_METHOD_READ;
static int              fd              = -1;
struct buffer *         buffers         = NULL;
static unsigned int     n_buffers       = 0;

static void
errno_exit                      (const char *           s)
{
        fprintf (stderr, "%s error %d, %s\n",
                 s, errno, strerror (errno));
        exit (EXIT_FAILURE);
}

static int xioctl(int fd, int request, void *arg)
{
        int r;

        do r = ioctl (fd, request, arg);
        while (-1 == r && EINTR == errno);

        return r;
}

static void process_image (const void *p)
{
	FILE *image = fopen("test.bmp","a");
	fwrite((char*)p,1,sizeof(p),image);
	fclose(image);
    fputc ('.', stdout);
    fflush (stdout);
}

static int read_frame(void)
{
   struct v4l2_buffer buf;
	unsigned int i;

	switch (io) {
	case IO_METHOD_READ:
    		if (-1 == read (fd, buffers[0].start, buffers[0].length)) {
            		switch (errno) {
            		case EAGAIN:
                    		return 0;

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit ("read");
			}
		}

    		process_image (buffers[0].start);

		break;
	}

	return 1;
}

static void
mainloop                        (void)
{
	unsigned int count;

        count = 100;

        while (count-- > 0) {
                for (;;) {
                        fd_set fds;
                        struct timeval tv;
                        int r;

                        FD_ZERO (&fds);
                        FD_SET (fd, &fds);

                        /* Timeout. */
                        tv.tv_sec = 2;
                        tv.tv_usec = 0;

                        r = select (fd + 1, &fds, NULL, NULL, &tv);

                        if (-1 == r) {
                                if (EINTR == errno)
                                        continue;

                                errno_exit ("select");
                        }

                        if (0 == r) {
                                fprintf (stderr, "select timeout\n");
                                exit (EXIT_FAILURE);
                        }

			if (read_frame ())
                    		break;
	
			/* EAGAIN - continue select loop. */
                }
        }
}

static void
stop_capturing                  (void)
{
        enum v4l2_buf_type type;

	switch (io) {
	case IO_METHOD_READ:
		/* Nothing to do. */
		break;

	}
}

static void
start_capturing                 (void)
{
}

static void
uninit_device                   (void)
{
	switch (io) {
	case IO_METHOD_READ:
		free (buffers[0].start);
		break;
	}
}

static void
init_read(unsigned int buffer_size)
{
        buffers = calloc (1, sizeof (*buffers));

        if (!buffers) {
                fprintf (stderr, "Out of memory\n");
                exit (EXIT_FAILURE);
        }

	buffers[0].length = buffer_size;
	buffers[0].start = malloc (buffer_size);

	if (!buffers[0].start) {
    		fprintf (stderr, "Out of memory\n");
            	exit (EXIT_FAILURE);
	}
}

static void init_device()
{
        struct v4l2_capability cap;
        struct v4l2_cropcap cropcap;
        struct v4l2_crop crop;
        struct v4l2_format fmt;
	unsigned int min;


	switch (io) {
	case IO_METHOD_READ:
		if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
			fprintf (stderr, "%s does not support read i/o\n",
				 dev_name);
			exit (EXIT_FAILURE);
		}

		break;

	}


        /* Select video input, video standard and tune here. */


	CLEAR (cropcap);

        cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (0 == xioctl (fd, VIDIOC_CROPCAP, &cropcap)) {
                crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                crop.c = cropcap.defrect; /* reset to default */

                if (-1 == xioctl (fd, VIDIOC_S_CROP, &crop)) {
                        switch (errno) {
                        case EINVAL:
                                /* Cropping not supported. */
                                break;
                        default:
                                /* Errors ignored. */
                                break;
                        }
                }
        } else {	
                /* Errors ignored. */
        }


        CLEAR (fmt);

        fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width       = 640; 
        fmt.fmt.pix.height      = 480;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;


        /* Note VIDIOC_S_FMT may change width and height. */

	/* Buggy driver paranoia. */
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;

	switch (io) {
	case IO_METHOD_READ:
		init_read (fmt.fmt.pix.sizeimage);
		break;
	}
}

static void
close_device                    (void)
{
        if (-1 == close (fd))
	        errno_exit ("close");

        fd = -1;
}

static void
open_device                     (void)
{
        struct stat st; 

        if (-1 == stat (dev_name, &st)) {
                fprintf (stderr, "Cannot identify '%s': %d, %s\n",
                         dev_name, errno, strerror (errno));
                exit (EXIT_FAILURE);
        }

        if (!S_ISCHR (st.st_mode)) {
                fprintf (stderr, "%s is no device\n", dev_name);
                exit (EXIT_FAILURE);
        }

        fd = open (dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

        if (-1 == fd) {
                fprintf (stderr, "Cannot open '%s': %d, %s\n",
                         dev_name, errno, strerror (errno));
                exit (EXIT_FAILURE);
        }
}

static void usage(FILE *fp, int argc, char **argv)
{
        fprintf (fp,
                 "Usage: %s [options]\n\n"
                 "Options:\n"
                 "-d | --device name   Video device name [/dev/video]\n"
                 "-h | --help          Print this message\n"
                 "-m | --mmap          Use memory mapped buffers\n"
                 "-r | --read          Use read() calls\n"
                 "-u | --userp         Use application allocated buffers\n"
                 "",
		 argv[0]);
}

static const char short_options [] = "d:hmru";

static const struct option
long_options [] = {
        { "device",     required_argument,      NULL,           'd' },
        { "help",       no_argument,            NULL,           'h' },
        { "mmap",       no_argument,            NULL,           'm' },
        { "read",       no_argument,            NULL,           'r' },
        { "userp",      no_argument,            NULL,           'u' },
        { 0, 0, 0, 0 }
};

int
main                            (int                    argc,
                                 char **                argv)
{
        dev_name = "/dev/video0";

        for (;;) {
                int index;
                int c;
                
                c = getopt_long (argc, argv,
                                 short_options, long_options,
                                 &index);

                if (-1 == c)
                        break;

                switch (c) {
                case 0: /* getopt_long() flag */
                        break;

                case 'd':
                        dev_name = optarg;
                        break;

                case 'h':
                        usage (stdout, argc, argv);
                        exit (EXIT_SUCCESS);

			break;

                case 'r':
                        io = IO_METHOD_READ;
			break;

                case 'u':
                        io = IO_METHOD_USERPTR;
			break;

                default:
                        usage (stderr, argc, argv);
                        exit (EXIT_FAILURE);
                }
        }

        open_device ();

        init_device ();

        start_capturing ();

        mainloop ();

        stop_capturing ();

        uninit_device ();

        close_device ();

        exit (EXIT_SUCCESS);

        return 0;
}
