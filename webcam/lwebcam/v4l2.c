/*
 *  V4L2 video capture example
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

#include <stdint.h>

int write_bitmap(const void *data, uint32_t width, uint32_t height)
{
   static int pic_num = 0;
   //Poopy bitmap header
   const int header[] =
   {
      (width*height*3)+36,    //File size - width by height by bitdepth / 8 + 36 bytes of header info
      0x00000000,          //Reserved for 'later' use - ooo laa laa
      0x00000026,          //Number of bytes till the bitmap data! (from beginning!) Should be 36
      0x00000028,          //Bit map header size or maybe which header to use
      width,               //Take a wild guess
      height,              //That's right! this one is the pallette!
      0x00180001,          //PLanes + bitdepth (flipped because of endianness)
      0x00000000,          //Compression (None!)
      width*height*3,         //How much bitmap data we have
      0x00000B13,          //H-Resolution - 19 for some reason...
      0x00000B13,          //V-Resolution - 19 for some funky munky reason
      0x00000000,          //Number of colours for palletted bitmaps (I believe...)
      0x00000000           //Number of heh, IMPORTANT colours :O...
   };

   char filename[255];
   sprintf(filename,"file%05d.bmp",pic_num++);
   FILE *bmp = fopen(filename,"wb");
   fprintf(bmp,"BM");
   fwrite(header,4,12,bmp);
   int i = 0;
   for(i = height;i>0;i--)
      fwrite(data+i*(width*3), 1, width*3, bmp);

   fclose(bmp);
   printf("Caught frame: %d\n",pic_num);
   return pic_num;
}


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
static io_method	io		= IO_METHOD_MMAP;
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

static int
xioctl                          (int                    fd,
                                 int                    request,
                                 void *                 arg)
{
        int r;

        do r = ioctl (fd, request, arg);
        while (-1 == r && EINTR == errno);

        return r;
}

static void
process_image                   (const void *           p)
{
		static int fnum = 0;
        fputc ('.', stdout);
			write_bitmap(p,640,480);
        fflush (stdout);
}

static int
read_frame			(void)
{
        struct v4l2_buffer buf;
	unsigned int i;

		CLEAR (buf);
            	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            	buf.memory = V4L2_MEMORY_MMAP;

    		if (-1 == xioctl (fd, VIDIOC_DQBUF, &buf)) {
            		switch (errno) {
            		case EAGAIN:
                    		return 0;

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit ("VIDIOC_DQBUF");
			}
		}

                assert (buf.index < n_buffers);

	        process_image (buffers[buf.index].start);

		if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
			errno_exit ("VIDIOC_QBUF");

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

	case IO_METHOD_MMAP:
	case IO_METHOD_USERPTR:
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl (fd, VIDIOC_STREAMOFF, &type))
			errno_exit ("VIDIOC_STREAMOFF");

		break;
	}
}

static void
start_capturing                 (void)
{
        unsigned int i;
        enum v4l2_buf_type type;

	switch (io) {
	case IO_METHOD_MMAP:
		for (i = 0; i < n_buffers; ++i) {
            		struct v4l2_buffer buf;

        		CLEAR (buf);

        		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        		buf.memory      = V4L2_MEMORY_MMAP;
        		buf.index       = i;

        		if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
                    		errno_exit ("VIDIOC_QBUF");
		}
		
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl (fd, VIDIOC_STREAMON, &type))
			errno_exit ("VIDIOC_STREAMON");

		break;
	}
}

static void
uninit_device                   (void)
{
        unsigned int i;

	switch (io) {
	case IO_METHOD_MMAP:
		for (i = 0; i < n_buffers; ++i)
			if (-1 == munmap (buffers[i].start, buffers[i].length))
				errno_exit ("munmap");
		break;
	}

	free (buffers);
}

static void
init_mmap			(void)
{
	struct v4l2_requestbuffers req;

        CLEAR (req);

        req.count               = 4;
        req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory              = V4L2_MEMORY_MMAP;

	if (-1 == xioctl (fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                        fprintf (stderr, "%s does not support "
                                 "memory mapping\n", dev_name);
                        exit (EXIT_FAILURE);
                } else {
                        errno_exit ("VIDIOC_REQBUFS");
                }
        }

        if (req.count < 2) {
                fprintf (stderr, "Insufficient buffer memory on %s\n",
                         dev_name);
                exit (EXIT_FAILURE);
        }

        buffers = calloc (req.count, sizeof (*buffers));

        if (!buffers) {
                fprintf (stderr, "Out of memory\n");
                exit (EXIT_FAILURE);
        }

        for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
                struct v4l2_buffer buf;

                CLEAR (buf);

                buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory      = V4L2_MEMORY_MMAP;
                buf.index       = n_buffers;

                if (-1 == xioctl (fd, VIDIOC_QUERYBUF, &buf))
                        errno_exit ("VIDIOC_QUERYBUF");

                buffers[n_buffers].length = buf.length;
                buffers[n_buffers].start =
                        mmap (NULL /* start anywhere */,
                              buf.length,
                              PROT_READ | PROT_WRITE /* required */,
                              MAP_SHARED /* recommended */,
                              fd, buf.m.offset);

                if (MAP_FAILED == buffers[n_buffers].start)
                        errno_exit ("mmap");
        }
}

static void
init_device                     (void)
{
        struct v4l2_capability cap;
        struct v4l2_cropcap cropcap;
        struct v4l2_crop crop;
        struct v4l2_format fmt;
			unsigned int min;

        if (-1 == xioctl (fd, VIDIOC_QUERYCAP, &cap)) {
                if (EINVAL == errno) {
                        fprintf (stderr, "%s is no V4L2 device\n",
                                 dev_name);
                        exit (EXIT_FAILURE);
                } else {
                        errno_exit ("VIDIOC_QUERYCAP");
                }
        }

        if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
                fprintf (stderr, "%s is no video capture device\n",
                         dev_name);
                exit (EXIT_FAILURE);
        }

	switch (io) {
	case IO_METHOD_MMAP:
	case IO_METHOD_USERPTR:
		if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
			fprintf (stderr, "%s does not support streaming i/o\n",
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
			printf("X : %d Y: %d WIDTH: %d HGEIGHT: %d\n",crop.c.left, crop.c.top, crop.c.width, crop.c.height);
        fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width       = 640; 
        fmt.fmt.pix.height      = 480;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

        if (-1 == xioctl (fd, VIDIOC_S_FMT, &fmt))
                errno_exit ("VIDIOC_S_FMT");

        /* Note VIDIOC_S_FMT may change width and height. */
	printf("Width is : %d\nHeight is: %d\n",fmt.fmt.pix.width, fmt.fmt.pix.height);
	/* Buggy driver paranoia. */
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;

	init_mmap ();
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

static void
usage                           (FILE *                 fp,
                                 int                    argc,
                                 char **                argv)
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

                case 'm':
                        io = IO_METHOD_MMAP;
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
