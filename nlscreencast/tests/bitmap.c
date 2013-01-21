#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int write_bitmap(char *data, uint32_t width, uint32_t height)
{
	static int pic_num = 0;
	//Poopy bitmap header
	const int header[] = 
	{
		(width*height*4)+36, 	//File size - width by height by bitdepth / 8 + 36 bytes of header info
		0x00000000,				//Reserved for 'later' use - ooo laa laa
		0x00000026,				//Number of bytes till the bitmap data! (from beginning!) Should be 36
		0x00000028,				//Bit map header size or maybe which header to use
		width,					//Take a wild guess
		height,					//That's right! this one is the pallette!
		0x00200001,				//PLanes + bitdepth (flipped because of endianness)
		0x00000000,				//Compression (None!)
		width*height*4,			//How much bitmap data we have
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
		fwrite(data+i*(width*4), 1, width*4, bmp);
	
	fclose(bmp);
	return pic_num;
}
