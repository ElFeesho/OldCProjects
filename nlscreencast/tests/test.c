/*
 *	Whole screen capture test! 
 */

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "bitmap.h"

void get_pointer_pos(Display *disp, int *x, int *y)
{
	Window root_win, child_win;
	int dummy;   

	root_win = DefaultRootWindow (disp);

	if (!XQueryPointer (disp, root_win, &root_win, &child_win,x, y, &dummy, &dummy, (unsigned int *) &dummy)) 
	{            
		*x = -1;
		*y = -1;
	}
}


int main(int argc, char **argv)
{
	printf("Screen capture test\nUsing X11 XGetImage\n");
	Display *disp = XOpenDisplay(NULL);
	Window root_win = XDefaultRootWindow(disp);
	unsigned int disp_width, disp_height;
	disp_width = XDisplayWidth(disp,0);
	disp_height = XDisplayHeight(disp,0);
	XSelectInput(disp,root_win,PointerMotionHintMask);
	XImage *screen_grab = XGetImage(disp, root_win,0,0,disp_width,disp_height,XAllPlanes(),ZPixmap);
	int i = 0;
	XImage *screen_grab2 = XGetImage(disp, root_win,0,0,disp_width,disp_height,XAllPlanes(),ZPixmap);
	XFlush(disp);
	GC gc = XCreateGC(disp, root_win, 0, 0);
	XSetForeground(disp, gc, 0xFF0000);
	int last_count = 0;
	int cur_count = 0;
	for(;;) 
	{
		int x, y;
		get_pointer_pos(disp,&x, &y);
		printf("Mouse pos: %d %d\n",x,y);
		XDrawLine(disp, root_win, gc, 10, 60, 180, 20);
		XFlush(disp);
		if(cur_count>last_count+20000)
		{
			last_count = cur_count;
			screen_grab2 = XGetImage(disp, root_win,0,0,disp_width,disp_height,XAllPlanes(),ZPixmap);
			if(memcmp(screen_grab2->data,screen_grab->data,disp_width*disp_height*4)!=0)
			{
				free(screen_grab);
				screen_grab = XGetImage(disp, root_win,0,0,disp_width,disp_height,XAllPlanes(),ZPixmap);
				write_bitmap(screen_grab->data,screen_grab->width,screen_grab->height);
			}
		}
		cur_count++;
	}
	XCloseDisplay(disp);
	return 0;
}
