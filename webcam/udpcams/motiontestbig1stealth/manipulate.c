#define WIDTH 320
#define HEIGHT 240
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CLAMP(x)	((x)<0?0:((x)>255)?255:(x))

void convolute(unsigned char *image)
{
	int mask[9] = 
	{ 
		0, -9, 9, 
		9, -9, 0, 
		0, -9, 9 
	};
	int scale = 1;
	unsigned char *new_img = malloc(WIDTH*HEIGHT*3);
	int i;
	int k;
	for(i = 0;i<WIDTH;i++)
	{
		for(k = 0; k<HEIGHT;k++)
		{
			if(i==0) /* No pixels to the left */
			{
				if(k==0) /* No pixels above */
				{
				
				}
				else if(k == 479) /* No pixels below */
				{
			
				}
				else /* Pixels all around :D */
				{
				
				}
			}
			else if(i==639) /* No pixels to the right */
			{
				if(k==0) /* No pixels above */
				{
				
				}
				else if(k == 479) /* No pixels below */
				{
			
				}
				else /* Pixels all around :D */
				{
				
				}
			}
			else /* There are pixels to the left and right :D */
			{
				if(k==0) /* No pixels above */
				{
				
				}
				else if(k == 479) /* No pixels below */
				{
			
				}
				else /* Pixels all around :D */
				{
					/* R */
					int offset_r = i*3+(k*WIDTH*3);

					int red_conv = 0;
					int gre_conv = 0;
					int blu_conv = 0;
					red_conv = (image[(i-1)*3+((k-1)*WIDTH*3)]*mask[0]);
					red_conv += (image[i*3+((k-1)*WIDTH*3)]*mask[1]);
					red_conv += (image[(i+1)*3+((k-1)*WIDTH*3)]*mask[2]);
					red_conv += (image[(i-1)*3+(k*WIDTH*3)]*mask[3]);
					red_conv += (image[i*3+(k*WIDTH*3)]*mask[4]);
					red_conv += (image[(i+1)*3+(k*WIDTH*3)]*mask[5]);
					red_conv += (image[(i-1)*3+((k+1)*WIDTH*3)]*mask[6]);
					red_conv += (image[i*3+((k+1)*WIDTH*3)]*mask[7]);
					red_conv += (image[(i+1)*3+((k+1)*WIDTH*3)]*mask[8]);
					/* G */
					gre_conv = (image[((i-1)*3+((k-1)*WIDTH*3))+1]*mask[0])+(image[(i*3+((k-1)*WIDTH*3))+1]*mask[1])+(image[((i+1)*3+((k-1)*WIDTH*3))+1]*mask[2])+(image[((i-1)*3+(k*WIDTH*3))+1]*mask[3])+(image[(i*3+(k*WIDTH*3))+1]*mask[4])+(image[((i+1)*3+(k*WIDTH*3))+1]*mask[5])+(image[((i-1)*3+((k+1)*WIDTH*3))+1]*mask[6])+(image[(i*3+((k+1)*WIDTH*3))+1]*mask[7])+(image[((i+1)*3+((k+1)*WIDTH*3))+1]*mask[8]);
					/* B */
					blu_conv = (image[((i-1)*3+((k-1)*WIDTH*3))+2]*mask[0])+(image[(i*3+((k-1)*WIDTH*3))+2]*mask[1])+(image[((i+1)*3+((k-1)*WIDTH*3))+2]*mask[2])+(image[((i-1)*3+(k*WIDTH*3))+2]*mask[3])+(image[(i*3+(k*WIDTH*3))+2]*mask[4])+(image[((i+1)*3+(k*WIDTH*3))+2]*mask[5])+(image[((i-1)*3+((k+1)*WIDTH*3))+2]*mask[6])+(image[(i*3+((k+1)*WIDTH*3))+2]*mask[7])+(image[((i+1)*3+((k+1)*WIDTH*3))+2]*mask[8]);
					red_conv/=scale;
					gre_conv/=scale;
					blu_conv/=scale;
					new_img[offset_r] = red_conv;
					new_img[offset_r+1] = gre_conv;
					new_img[offset_r+2] = blu_conv;
				}
			}
		}
	}
	memcpy(image, new_img, WIDTH*HEIGHT*3);
	free(new_img);
}

static void copy_image_scaled(unsigned char *dst, unsigned char *src)
{
	int y = 0;
	int x = 0;
	int ay = 0;
	int ax = 0;
	for(y = 0,ay=0;y<HEIGHT;y+=2,ay++)
	{
		for(x = 0,ax = 0;x<WIDTH*3;x+=6,ax+=3)
		{
			dst[ax+((WIDTH/2)*ay*3)] = src[(y*WIDTH*3)+x];
			dst[ax+((WIDTH/2)*ay*3)+1] = src[(y*WIDTH*3)+x+1];
			dst[ax+((WIDTH/2)*ay*3)+2] = src[(y*WIDTH*3)+x+2];
		}
	}
}

static char check_pixels(unsigned char *image1, unsigned char *image2, int x, int y, char thresh)
{
	if(image1[x+y*((WIDTH/2)*3)]<thresh)
	{
		if(image2[x+y*((WIDTH/2)*3)]>image1[x+y*((WIDTH/2)*3)]+thresh)
			return 1;
		else
			return 0;
	}
	if((image1[x+y*((WIDTH/2)*3)]+thresh)>255)
	{
		if(image2[x+y*((WIDTH/2)*3)]<image1[x+y*((WIDTH/2)*3)]-thresh)
			return 1;
		else
			return 0;
	}

	if(image1[x+y*((WIDTH/2)*3)]<image2[x+y*((WIDTH/2)*3)]-thresh || image1[x+y*((WIDTH/2)*3)]>image2[x+y*((WIDTH/2)*3)]+thresh)
		return 1;
	else
		return 0;
}

void manip_check_diffs(unsigned char *image, int *xp, int *yp, int *xp2, int *yp2)
{
	static unsigned char *last_image = NULL;
	if(last_image == NULL)
	{
		*xp = -1;
		*yp = -1;
		*xp2 = -1;
		*yp2 = -1;
		//The resolution of motion detection is half of what the actual image size is (I doubt highly we wanna check if a fly is in the room etc)
		last_image = malloc((WIDTH/2)*3*(HEIGHT/2)); 
		copy_image_scaled(last_image, image);
		
	}
	unsigned char *current_image = malloc((WIDTH/2)*(HEIGHT/2)*3);
	copy_image_scaled(current_image, image);

	*xp = -1;
	*yp = -1;
	*xp2 = -1;
	*yp2 = -1;
	
	int y = 0;
	int x = 0;
	for(y = 0; y<(HEIGHT/2);y++)
	{
		for(x = 0; x < (WIDTH/2); x++)
		{
			//Check the 3 pixels
			if(check_pixels(current_image, last_image, x*3,y,16))
			{
				if(check_pixels(current_image, last_image, x*3+1,y,16))
				{
					if(check_pixels(current_image, last_image, x*3+2,y,16))
					{
						if(*xp==-1) // not been set yet
						{
							*xp = x;
							*xp2 = x;
							*yp = y;
							*yp2 = y;
						}
						if(*xp>x)
							*xp = x;
						else if(*xp2<x)
							*xp2 = x;
						if(*yp>y)
							*yp = y;
						else if(*yp2<y)
							*yp2 = y;
					}		
				}
			}
		}
	}
	*xp*=2;
	*yp*=2;
	*xp2*=2;
	*yp2*=2;
	copy_image_scaled(last_image, image);
	free(current_image);
}
