#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CLAMP(x)	((x)<0?0:((x)>255)?255:(x))

typedef struct 
{
	int is_abs;
	int len;
	int val;
	int unk;
} code_table_t;


/* local storage */
static code_table_t table[256];

/*
	sonix_decompress_init
	=====================
		pre-calculates a locally stored table for efficient huffman-decoding.

	Each entry at index x in the table represents the codeword
	present at the MSB of byte x.

*/
void manip_decompress_init()
{
	int i;
	int is_abs, val, len, unk;

	for (i = 0; i < 256; i++) 
	{
		is_abs = 0;
		val = 0;
		len = 0;
		unk = 0;
		if ((i & 0x80) == 0) 
		{
			/* code 0 */
			val = 0;
			len = 1;
		}
		else if ((i & 0xE0) == 0x80) 
		{
			/* code 100 */
			val = +4;
			len = 3;
		}
		else if ((i & 0xE0) == 0xA0) 
		{
			/* code 101 */
			val = -4;
			len = 3;
		}
		else if ((i & 0xF0) == 0xD0) 
		{
			/* code 1101 */
			val = +11;
			len = 4;
		}
		else if ((i & 0xF0) == 0xF0) 
		{
			/* code 1111 */
			val = -11;
			len = 4;
		}
		else if ((i & 0xF8) == 0xC8) 
		{
			/* code 11001 */
			val = +20;
			len = 5;
		}
		else if ((i & 0xFC) == 0xC0) 
		{
			/* code 110000 */
			val = -20;
			len = 6;
		}
		else if ((i & 0xFC) == 0xC4) 
		{
			/* code 110001xx: unknown */
			val = 0;
			len = 8;
			unk = 1;
		}
		else if ((i & 0xF0) == 0xE0) 
		{
			/* code 1110xxxx */
			is_abs = 1;
			val = (i & 0x0F) << 4;
			len = 8;
		}
		table[i].is_abs = is_abs;
		table[i].val = val;
		table[i].len = len;
		table[i].unk = unk;
	}
}


/*
	sonix_decompress
	================
		decompresses an image encoded by a SN9C101 camera controller chip.

	IN	width
		height
		inp		pointer to compressed frame (with header already stripped)
	OUT	outp	pointer to decompressed frame

	Returns 0 if the operation was successful.
	Returns <0 if operation failed.

*/

int manip_decompress(int width, int height, unsigned char *inp, unsigned char *outp)
{
	int row, col;
	int val;
	int bitpos;
	unsigned char code;
	unsigned char *addr;

	bitpos = 0;
	for (row = 0; row < height; row++) 
	{
		col = 0;

		/* first two pixels in first two rows are stored as raw 8-bit */
		if (row < 2) 
		{
			addr = inp + (bitpos >> 3);
			code = (addr[0] << (bitpos & 7)) | (addr[1] >> (8 - (bitpos & 7)));
			bitpos += 8;
			*outp++ = code;

			addr = inp + (bitpos >> 3);
			code = (addr[0] << (bitpos & 7)) | (addr[1] >> (8 - (bitpos & 7)));
			bitpos += 8;
			*outp++ = code;

			col += 2;
		}

		while (col < width) 
		{
			/* get bitcode from bitstream */
			addr = inp + (bitpos >> 3);
			code = (addr[0] << (bitpos & 7)) | (addr[1] >> (8 - (bitpos & 7)));

			/* update bit position */
			bitpos += table[code].len;

			/* calculate pixel value */
			val = table[code].val;
			if (!table[code].is_abs) 
			{
				/* value is relative to top and left pixel */
				if (col < 2) 
				{
					/* left column: relative to top pixel */
					val += outp[-2*width];
				}
				else if (row < 2) 
				{
					/* top row: relative to left pixel */
					val += outp[-2];
				}
				else 
				{
					/* main area: average of left pixel and top pixel */
					val += (outp[-2] + outp[-2*width]) / 2;
				}
			}

			/* store pixel */
			*outp++ = CLAMP(val);
			col++;
		}
	}
	return 0;
}

static void copy_image_scaled(unsigned char *dst, unsigned char *src)
{
	int y = 0;
	int x = 0;
	int ay = 0;
	int ax = 0;
	for(y = 0,ay=0;y<480;y+=2,ay++)
	{
		for(x = 0,ax = 0;x<640*3;x+=6,ax+=3)
		{
			dst[ax+(320*ay*3)] = src[(y*640*3)+x];
			dst[ax+(320*ay*3)+1] = src[(y*640*3)+x+1];
			dst[ax+(320*ay*3)+2] = src[(y*640*3)+x+2];
		}
	}
}

static char check_pixels(unsigned char *image1, unsigned char *image2, int x, int y, char thresh)
{
	if(image1[x+y*(320*3)]<thresh)
	{
		if(image2[x+y*(320*3)]>image1[x+y*(320*3)]+thresh)
			return 1;
		else
			return 0;
	}
	if((image1[x+y*(320*3)]+thresh)>255)
	{
		if(image2[x+y*(320*3)]<image1[x+y*(320*3)]-thresh)
			return 1;
		else
			return 0;
	}

	if(image1[x+y*(320*3)]<image2[x+y*(320*3)]-thresh || image1[x+y*(320*3)]>image2[x+y*(320*3)]+thresh)
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
		last_image = (unsigned char*)malloc(320*3*240); 
		copy_image_scaled(last_image, image);
		
	}
	unsigned char *current_image = (unsigned char*)malloc(320*240*3);
	copy_image_scaled(current_image, image);

	*xp = -1;
	*yp = -1;
	*xp2 = -1;
	*yp2 = -1;
	
	int y = 0;
	int x = 0;
	for(y = 0; y<240;y++)
	{
		for(x = 0; x < 320; x++)
		{
			//Check the 3 pixels
			if(check_pixels(current_image, last_image, x*3,y,32))
			{
				if(check_pixels(current_image, last_image, x*3+1,y,32))
				{
					if(check_pixels(current_image, last_image, x*3+2,y,32))
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
