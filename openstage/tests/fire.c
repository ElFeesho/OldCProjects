
/*
Variable Declarations
 FIXEDPOINT VLDx, VRDx, HDx
 FIXEDPOINT VLDy, VRDy, HDy
 FIXEDPOINT TX1, TY1, TX2, TY2, tx, ty
 INTEGER x, y

Code Begins
 VLDx = (Cx - Ax) / 16          'Rate of change of X down the left side of the wonky square 
 VRDx = (Dx - Bx) / 16          'Rate of change of X down the right side
 VLDy = (Cy - Ay) / 16          'Rate of change of Y down the left side
 VRDy = (Dy - By) / 16          'Rate of change of Y down the right side

 TX1  = Ax
 TY1  = Ay
 TX2  = Bx
 TY2  = By

 loop y from 0 to 15
        HDx  = (TX2-TX1) / 16   'Rate of change of X across the wonky polygon
        HDy  = (TY2-TY1) / 16   'Rate of change of Y across the wonky polygon
        tx = TX1
        ty = TY1

        loop x from 0 to 15
                Image2(x, y) = Image1( int(tx), int(ty) ) 
                tx = tx + HDx
                ty = ty + HDy
        end of x loop

        TX1 = TX1 + VLDx;
        TY1 = TY1 + VLDy;
        TX2 = TX2 + VRDx;
        TY2 = TY2 + VRDy;
 end of y loop
*/
 
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

SDL_Surface *screen = NULL, *test_case = NULL;

typedef struct
{
	unsigned int x, y, ox, oy;
} point;

point points[32*12];

void create_grid()
/*
	Expects test case to not be null!
*/
{	
	int i, j;
	for(i = 0;i<32;i++)
	{
		for(j = 0; j<24;j++)
		{
			points[i+j*32].x = i*20;
			points[i+j*32].y = j*20;
			points[i+j*32].ox = i*20;
			points[i+j*32].oy = j*20;
		}
	}
}

void draw_points()
{
	int i, j;
	for(i = 0;i<32;i++)
	{
		for(j = 0; j<24;j++)
		{
			SDL_Rect opos = { 0,0,1,1 }, pos = { 0,0,1,1 };
			opos.x = points[i+j*32].ox;
			opos.y = points[i+j*32].oy;
			pos.x = points[i+j*32].x;
			pos.y = points[i+j*32].y;
			SDL_FillRect(screen, &opos,SDL_MapRGBA(screen->format,0xff,0x00,0x00,0x00));
			SDL_FillRect(screen, &pos,SDL_MapRGBA(screen->format,0xff,0x00,0xff,0x00));
		}
	}
}

void mutate_grid()
{
	int i, j;
	for(i = 1;i<31;i++)
	{
		for(j = 1; j<23;j++)
		{
			points[i+j*32].x+=(rand()%10)-5;
			points[i+j*32].y+=(rand()%10)-5;
		}
	}
}

void transfer_pixels(SDL_Surface *surface, int x, int y, int fx, int fy)
{
	memcpy((void*)(surface->pixels+y*surface->w*3+x*3),(void*)(test_case->pixels+fy*test_case->w*3+fx*3),3);
}

void apply_transformation()
{
	SDL_Surface *ret_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,640,480,0,0,0,0,0);
	int i, j;
	for(i = 0;i<31;i++)
	{
		for(j = 0; j<23;j++)
		{
			double VLDx, VRDx, HDx;
			double VLDy, VRDy, HDy;
			double TX1, TY1, TX2, TY2, tx, ty;
			int x, y;
			VLDx = (points[i+j*32].x - points[i+j*32].ox) / 21;//          'Rate of change of X down the left side of the wonky square 
			VRDx = (points[(i+1)+j*32].x - points[(i+1)+j*32].ox) / 21;//          'Rate of change of X down the right side
			VLDy = (points[i+j*32].y - points[i+j*32].oy) / 21; //         'Rate of change of Y down the left side
			VRDy = (points[(i+1)+j*32].y - points[(i+1)+j*32].oy) / 21; //         'Rate of change of Y down the right side

			TX1  = points[i+j*32].ox;
			TY1  = points[i+j*32].oy;
			TX2  = points[(i+1)+j*32].ox;
			TY2  = points[(i+1)+j*32].oy;

			for(y = points[i+j*32].oy;y<points[i+j*32].oy+20;y++)
			{
				SDL_BlitSurface(ret_surface, NULL, screen, NULL);
				HDx  = (TX2-TX1) / 21;
				HDy  = (TY2-TY1) / 21;
				tx = TX1;
				ty = TY1;
				printf("HDx: %f TX2 %f TX1 %f\n",HDx, TX2, TX1);
				if(HDx>20.0)
					HDx = 20.0;
				if(HDy > 20.0)
					HDy = 20.0;
				for(x = points[i+j*32].ox; x<points[i+j*32].ox+20; x++)
				{
					printf("Tx: %d\n",tx);
					transfer_pixels(ret_surface,x,y, tx, ty);

					tx += HDx;
					ty += HDy;
				}

				TX1 += VLDx;
				TY1 += VLDy;
				TX2 += VRDx;
				TY2 += VRDy;
			}
		}
	}
	SDL_FreeSurface(test_case);
	test_case = ret_surface;
}

int main(int argc, char **argv)
{
	if(SDL_Init(SDL_INIT_VIDEO)>0)
	{
		fprintf(stderr, "Failed initialising SDL\n");
		return 1;
	}
	
	screen = SDL_SetVideoMode(640,480,24,SDL_SWSURFACE);
	
	if(!screen)
	{
		fprintf(stderr, "Failed to set screen mode\n");
		return 1;
	}
	
	test_case = IMG_Load("test_case.png");
	
	if(!test_case)
	{
		fprintf(stderr, "Failed to load test case\n");
		return 1;
	}
	
	create_grid();
	
	for(;;)
	{
		SDL_Event ev;
		while(SDL_PollEvent(&ev))
		{
			switch(ev.type)
			{
				case SDL_QUIT:
					SDL_Quit();
					return 0;
					break;
				case SDL_KEYDOWN:
					/* Goto next step */
					mutate_grid();
					apply_transformation();
					break;
			}
		}
		SDL_BlitSurface(test_case, NULL, screen, NULL);
		draw_points();
		SDL_Flip(screen);
	}
	return 0;
}
