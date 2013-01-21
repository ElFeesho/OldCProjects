#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <memory.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev.h>

#include <stdint.h>

int write_bitmap(char *data, uint32_t width, uint32_t height)
{
	static int pic_num = 0;
	//Poopy bitmap header
	const int header[] = 
	{
		(width*height*3)+36, 	//File size - width by height by bitdepth / 8 + 36 bytes of header info
		0x00000000,				//Reserved for 'later' use - ooo laa laa
		0x00000026,				//Number of bytes till the bitmap data! (from beginning!) Should be 36
		0x00000028,				//Bit map header size or maybe which header to use
		width,					//Take a wild guess
		height,					//That's right! this one is the pallette!
		0x00180001,				//PLanes + bitdepth (flipped because of endianness)
		0x00000000,				//Compression (None!)
		width*height*3,			//How much bitmap data we have
		0x00000B13,				//H-Resolution - 19 for some reason...
		0x00000B13,				//V-Resolution - 19 for some funky munky reason
		0x00000000,				//Number of colours for palletted bitmaps (I believe...)
		0x00000000				//Number of heh, IMPORTANT colours :O...
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


void print_caps(struct video_capability *caps)
{
	printf("Wecam Caps\n");
	printf("~~~~~~~~~~~~~\n");
	printf("Name: %s\n",caps->name);
	printf("Max W: %d\n",caps->maxwidth);
	printf("Max H: %d\n",caps->maxheight);
	printf("Min W: %d\n",caps->minwidth);
	printf("Min H: %d\n",caps->minheight);
	
	switch(caps->type)
	{
		case VID_TYPE_CAPTURE:
			printf("Type: Memory Capturing\n");
			break;
		default:
			printf("Type: Other\n");
			break;
	}
	
	printf("~~~~~~~~~~~~~\n");
}

void print_capwin(struct video_window *cap_win)
{
	printf("Capture Window Info\n");
	printf("~~~~~~~~~~~~~\n");
	printf("Cap X: %d\n",cap_win->x);
	printf("Cap Y: %d\n",cap_win->y);
	printf("Cap H: %d\n",cap_win->height);
	printf("Cap W: %d\n",cap_win->width);
	printf("Flags: %d\n",cap_win->flags);
	printf("~~~~~~~~~~~~~\n");
}

int main(int argc, char **argv)
{
	int webcam = open("/dev/video0",O_RDWR);
	if(webcam<0)
	{
		fprintf(stderr,"Couldn't connect to camera\n");
		return 1;
	}
	
	struct video_capability caps;
	
	if(ioctl(webcam,VIDIOCGCAP,&caps)<0)
	{
		fprintf(stderr,"Failed reading capabilities\n");
		return 1;
	}
	printf("VIDIOCGCAP\n");	
	struct video_window cap_win;
	
	ioctl(webcam,VIDIOCGWIN,&cap_win);
	printf("VIDIOCGWIN\n");
	struct video_picture pict_info;
	ioctl(webcam,VIDIOCGPICT,&pict_info);
	printf("VIDIOCGPICT\n");
	pict_info.contrast = 10485;
	pict_info.brightness = 51772;
	
	if(ioctl(webcam,VIDIOCSPICT,&pict_info)<0)
	{
		fprintf(stderr,"Couldn't set picture mode!\n");
		return 1;
	}
	printf("VIDIOCSPICT\n");
	int frame = 0;
	
	struct video_mbuf mbuf;
	
	if(ioctl(webcam, VIDIOCGMBUF,&mbuf)<0)
	{
		fprintf(stderr,"Failed to make mbuf\n");
		return 1;
	}
	printf("VIDIOCGMBUF\n");
	void *data= mmap(0, mbuf.size, PROT_READ | PROT_WRITE, MAP_SHARED, webcam,0);
	
	int i = -1;
	struct video_mmap vid_map;
	vid_map.width = cap_win.width;
	vid_map.height = cap_win.height;
	vid_map.format = 4;
	vid_map.frame = 0;
	if(ioctl(webcam,VIDIOCMCAPTURE,&vid_map)<0)
	{
		fprintf(stderr, "Failed to capture! here\n");
		return 1;
	}
	printf("VIDIOCMCAPTURE\n");
	for(;;)
	{
		vid_map.frame = 1;
		if(ioctl(webcam,VIDIOCMCAPTURE,&vid_map)<0)
		{
			fprintf(stderr, "Failed to capture!\n");
			return 1;
		}
		printf("VIDIOCMCAPTURE\n");
		while(ioctl(webcam,VIDIOCSYNC,&frame)<0 && errno==EINTR);
		printf("VIDIOCSYNC\n");
		frame++;
		write_bitmap(data+mbuf.offsets[0],cap_win.width,cap_win.height);
		vid_map.frame = 0;
		if(ioctl(webcam,VIDIOCMCAPTURE,&vid_map)<0)
		{
			fprintf(stderr, "Failed to capture!\n");
			return 1;
		}
		printf("VIDIOCMCAPTURE\n");
		while(ioctl(webcam,VIDIOCSYNC,&frame)<0 && errno==EINTR);
		printf("VIDIOCSYNC\n");
		frame=0;
		write_bitmap(data+mbuf.offsets[1],cap_win.width,cap_win.height);
	}
	close(webcam);	
	return 0;
}
