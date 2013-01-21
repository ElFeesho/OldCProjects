#include <SDL/SDL.h>
#include <SDL/SDL_rotozoom.h>
#include <wiiuse.h>

struct wiimote_t **wiimotes;

int cursorpos[2];
double lastrot[2];

int main(int argc, char **argv)
{
	SDL_Surface *screen = NULL;
	SDL_Surface *backbuf = NULL;
	if(SDL_Init(SDL_INIT_VIDEO)<0)
	{
		fprintf(stderr, "Failed initialising SDL\n");
		return 0;
	}
	SDL_WM_SetCaption("Etcha-Sketch - Idea by D. Palmer", NULL);
	screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
	cursorpos[0] = 320;
	cursorpos[1] = 240;
	if(!screen)
	{
		fprintf(stderr, "Failed setting video mode\n");
		return 0;
	}

	backbuf = SDL_CreateRGBSurface(SDL_SWSURFACE,640,480, 24,0,0,0,0);
	SDL_FillRect(backbuf, NULL, SDL_MapRGB(backbuf->format, 192,192,192));
	SDL_BlitSurface(backbuf,NULL,screen,NULL);
	SDL_Flip(screen);


	wiimotes = wiiuse_init(2);
	int found = wiiuse_find(wiimotes, 2, 5);
	if(found<2)
	{
		fprintf(stderr, "Etcha-Sketch needs 2 nobs - er wiimotes!\n");
		return 0;
	}

	int connected = wiiuse_connect(wiimotes, 2);

	if(connected<2)
	{
		fprintf(stderr, "Failed to connect to 2 wiimotes\n");
		return 0;
	}

	wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_1);
	wiiuse_set_leds(wiimotes[1], WIIMOTE_LED_1);
	wiiuse_motion_sensing(wiimotes[0], 1);
	wiiuse_motion_sensing(wiimotes[1], 1);
	lastrot[0] = wiimotes[0]->orient.roll;
	lastrot[1] = wiimotes[1]->orient.roll;

	SDL_Event ev;

	int quit = 0;
	while(!quit)
	{
		while(SDL_PollEvent(&ev))
		{
			switch(ev.type)
			{
				case SDL_QUIT:
					quit = 1;
					break;
			}
		}
		if(wiiuse_poll(wiimotes,2))
		{
			if(wiimotes[0]->event == WIIUSE_EVENT && !IS_HELD(wiimotes[0],WIIMOTE_BUTTON_B))
			{
				if(wiimotes[0]->orient.roll<lastrot[0]-10)
				{
					lastrot[0] = wiimotes[0]->orient.roll;
					cursorpos[0]-=1;
				}
				else if(wiimotes[0]->orient.roll>lastrot[0]+10)
				{
					lastrot[0] = wiimotes[0]->orient.roll;
					cursorpos[0]+=1;
				}
			}
			if(wiimotes[1]->event == WIIUSE_EVENT && !IS_HELD(wiimotes[1], WIIMOTE_BUTTON_B))
			{
				if(wiimotes[1]->orient.roll<lastrot[1]-10)
				{
					lastrot[1] = wiimotes[1]->orient.roll;
					cursorpos[1]-=1;
				}
				else if(wiimotes[1]->orient.roll>lastrot[1]+10)
				{
					lastrot[1] = wiimotes[1]->orient.roll;
					cursorpos[1]+=1;
				}
			}
		}
		if(!IS_HELD(wiimotes[0], WIIMOTE_BUTTON_A) && !IS_HELD(wiimotes[1], WIIMOTE_BUTTON_A))
		{
			aalineRGBA(backbuf, cursorpos[0], cursorpos[1], cursorpos[0], cursorpos[1], 0,0,0,255);
		}
		SDL_BlitSurface(backbuf,NULL,screen,NULL);
		boxRGBA(screen, cursorpos[0]-1, cursorpos[1]-2,cursorpos[0]+1,cursorpos[1]+1, 128,0,0,255);
		SDL_Flip(screen);
	}
	
	SDL_Quit();

	return 0;
}
