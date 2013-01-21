#include <SDL/SDL_opengl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main_desktop.hpp"

#include <SDL/SDL.h>

MainDesktop::MainDesktop() : cx(0-400),cy(0-300),ltick(0), screenw(800),screenh(600), dragging(false)
{
	SDL_SetVideoMode(screenw, screenh, 32, SDL_OPENGL|SDL_HWSURFACE);
	SDL_WM_SetCaption("Beach",NULL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_DEPTH_TEST);
	glEnable (GL_COLOR_MATERIAL);
		
	glViewport(0,0,screenw,screenh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Calculate The Aspect Ratio Of The Window
	gluOrtho2D(0,screenw,screenh,0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

MainDesktop::~MainDesktop()
{
}

void MainDesktop::flush(){
	SDL_Event ev;
	while(SDL_PollEvent(&ev))
	{
		if(ev.type == SDL_QUIT)
		{
			SDL_Quit();
			exit(0);
		}
	}
	SDL_GL_SwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT);

	SDL_PumpEvents(); // Update events

	// Update window position based on mouse coordinates
	Uint8 *keys = SDL_GetKeyState(NULL);
	if(dragging)
	{
		int mx, my;
		SDL_GetMouseState(&mx, &my);
		if(mx<96)
		{
			cx-=(int)(8.0f*(1.0f-(96.0f/mx)));
		}
	
		if(mx>screenw-96)
		{
			cx+=(int)(8.0f*(1.0f-(96.0f/(screenw-mx))));
		}
	
		if(my<96)
		{
			cy-=(int)(8.0f*(1.0f-(96.0f/my)));
		}
	
		if(my>screenh-96)
		{
			cy+=(int)(8.0f*(1.0f-(96.0f/(screenh-my))));
		}
		if(keys[SDLK_LCTRL])
		{
			dragging = false;
			keys[SDLK_LCTRL]=0;
		}
	}
	else
	{
		if(keys[SDLK_LCTRL])
		{
			dragging = true;
			keys[SDLK_LCTRL]=0;
		}
	}

	// Draw dots

	glLoadIdentity();
	for(int x=cx%64;x<=screenw;x+=64) 
	{
		for(int y=cy%64;y<=screenh;y+=64) 
		{
			glBegin(GL_QUADS);
				//glColor3f(152.0f/255.0f,208.0f/255.0f, 1.0f);
				glColor4f(102.0f/255.0f,158.0f/255.0f, 1.0f, 1.0f);
				glVertex2i(x-2,y-2);
				glVertex2i(x+2,y-2);
				glVertex2i(x+2,y+2);
				glVertex2i(x-2,y+2);
			glEnd();
		}
	}

	long ldiff = 1000/60-(SDL_GetTicks()-ltick);
	if(ldiff>0)
		SDL_Delay(ldiff);
	ltick = SDL_GetTicks();
}
