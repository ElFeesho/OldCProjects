#define WIDTH 320
#define HEIGHT 240
#include <stdlib.h>

#include "bitmap.h"
#include "camera.h"
#include "network.h"

void take_photo(void *data)
{
	static int ltime = 0;
	static int img_num = 0;
	if(ltime == 0)
	{
		ltime = time(NULL);
	}
	else if(time(NULL)<ltime+3)
	{
		return;
	}
	char *filename = malloc(128);
	sprintf(filename,"caught%03d.bmp",img_num++);
	bitmap_write(data, WIDTH, HEIGHT);
}

char update_camera()
{
	char *data = camera_read_image();
 
   int x, y,x2,y2;
   manip_check_diffs(data, &x, &y, &x2, &y2);
	if(x!=-2)
	{
		take_photo(data);
	}
	if(data==NULL)
		return 1;
	unsigned char *send_data = NULL;
	int bytes = network_encode_data(data, &send_data);
	free(send_data);
	return 1;
}
