#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <wiiuse.h>

#include "res.h"

static int found = 0;

static wiimote **wiimotes;

void handle_event(wiimote *wm, int num);

int check_wiimotes(void *data)
{
	for(;;)
	{
		if(wiiuse_poll(wiimotes,2))
		{
			int i;
			for(i = 0;i<2;i++)
			{
				switch(wiimotes[i]->event)
				{
					case WIIUSE_EVENT:
						handle_event(wiimotes[i],i);
						break;
				}
			}
		}
	}
}

int connect_wiimotes(void *data)
{
	wiimotes = wiiuse_init(2);
	int fnd = 0;
	while(found!=2)
	{
		fnd = wiiuse_find(wiimotes+found,2-found,5);
		if(fnd>=1)
		{
			if(wiiuse_connect(wiimotes+found,2-found))
				found+=fnd;
		}
	}

	int i = 0;
	for(i = 0; i<2;i++)
	{
		wiiuse_set_leds(wiimotes[i],WIIMOTE_LED_1<<i);
		wiiuse_motion_sensing(wiimotes[i],1);
	}
	return 0;
}

void draw_intro_gfx(SDL_Surface *screen, int alphat, int alphac, int alphaw, int alphaw2)
{
	static SDL_Surface *pnp, *cwn, *wiimote;
	if(!pnp)
	{
		pnp = IMG_Load("pnp.png");
		cwn = IMG_Load("cwn.png");
		wiimote = IMG_Load("wii.png");
	}

	SDL_Rect wii_pos[2];

	wii_pos[0].x = screen->w/3-8;
	wii_pos[0].y = 320;
	wii_pos[0].w = wiimote->w;
	wii_pos[0].h = wiimote->h;

	wii_pos[1].x = screen->w/3*2-8;
	wii_pos[1].y = 320;
	wii_pos[1].w = wiimote->w;
	wii_pos[1].h = wiimote->h;

	SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 255,255,255));

	SDL_Rect pnp_pos;
	pnp_pos.x = 320-pnp->w/2;
	pnp_pos.y = 240-pnp->h/2-cwn->h;
	pnp_pos.w = pnp->w;
	pnp_pos.h = pnp->h;

	SDL_Rect cwn_pos;
	cwn_pos.x = 320-cwn->w/2;
	cwn_pos.y = 240-pnp->h/2+cwn->h+15;
	cwn_pos.w = cwn->w;
	cwn_pos.h = cwn->h;

	SDL_SetAlpha(pnp,SDL_SRCALPHA,alphat);
	SDL_BlitSurface(pnp,NULL,screen,&pnp_pos);

	SDL_SetAlpha(cwn,SDL_SRCALPHA,alphac);
	SDL_BlitSurface(cwn,NULL,screen,&cwn_pos);

	SDL_SetAlpha(wiimote,SDL_SRCALPHA,alphaw);
	SDL_BlitSurface(wiimote, NULL, screen, &wii_pos[0]);
	SDL_SetAlpha(wiimote,SDL_SRCALPHA,alphaw2);
	SDL_BlitSurface(wiimote, NULL, screen, &wii_pos[1]);

	SDL_Flip(screen);
}

void intro(SDL_Surface *screen)
{
	int alpha[2] = { 0 };
	draw_intro_gfx(screen,0,0,0,0);
	//SDL_Delay(1500);

	while(alpha[0]<255)
	{
		draw_intro_gfx(screen,alpha[0],0,0,0);
		alpha[0]+=2;
		SDL_Delay(10);
	}
	alpha[0] = 255;
	//SDL_Delay(1000);

	while(alpha[1]<255)
	{
		draw_intro_gfx(screen,alpha[0],alpha[1],0,0);
		alpha[1]+=2;
		SDL_Delay(10);
	}

	//SDL_Delay(1000);
}

int display_connectivity(SDL_Surface *screen)
{
	int alpha[2] = { 0 };

	while(alpha[0]<128)
	{
		alpha[0]+=2;
		alpha[1]+=2;
		draw_intro_gfx(screen,255,255,alpha[0],alpha[1]);
		SDL_Delay(20);
	}

	while(alpha[1]<255)
	{
		if(found>=1)
		{
			if(alpha[0]<255)
				alpha[0]++;
		}
		if(found>=2)
		{
			if(alpha[1]<255)
				alpha[1]++;
		}
		draw_intro_gfx(screen,255,255,alpha[0],alpha[1]);
		SDL_Delay(20);
		SDL_Event ev;
		while(SDL_PollEvent(&ev))
		{
			if(ev.type == SDL_QUIT)
			{
				return -1;
			}
		}
	}
	SDL_Delay(500);
	for(alpha[0]=255;alpha[0]>0;alpha[0]-=5)
	{
		draw_intro_gfx(screen,alpha[0],alpha[0],alpha[0],alpha[0]);
		SDL_Delay(20);
	}
	return 0;
}


void handle_event(struct wiimote_t *wm, int num)
{
	static int down[2] = { 0 };
	static int play[2] = { 0 };
	if(wm->gforce.y>1.0 && !down[num])
	{
		down[num] = 1;
		play[num] = 1;
	}
	else if(down[num] && wm->gforce.y<0.5)
		down[num] = 0;
	if(down[num] && play[num])
	{
		play[num] = 0;
		if(IS_PRESSED(wm, WIIMOTE_BUTTON_A))
		{
			res_play(SNARE,num);
		}

		if(IS_PRESSED(wm, WIIMOTE_BUTTON_B))
		{
			res_play(HIHAT,num);
		}

		if(IS_PRESSED(wm, WIIMOTE_BUTTON_LEFT))
		{
			res_play(CRASH,num);
		}

		if(IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN))
		{
			res_play(BASS,num);
		}

		if(IS_PRESSED(wm, WIIMOTE_BUTTON_RIGHT))
		{
			res_play(CYM,num);
		}

		if(IS_PRESSED(wm, WIIMOTE_BUTTON_PLUS))
		{
			res_play(HITOM,num);
		}

		if(IS_PRESSED(wm, WIIMOTE_BUTTON_HOME))
		{
			res_play(MIDTOM,num);
		}

		if(IS_PRESSED(wm, WIIMOTE_BUTTON_MINUS))
		{
			res_play(FLOORTOM,num);
		}

		if(IS_PRESSED(wm, WIIMOTE_BUTTON_ONE))
		{
			res_play(STICK,num);
		}
	}
}

int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);

	if(Mix_OpenAudio(44100, AUDIO_S16SYS, 4, 1024)<0)
   {
		printf("Couldn't open audio\n");
		return 0;
   }

	SDL_CreateThread(res_load_resources,NULL);

	SDL_Surface *screen = SDL_SetVideoMode(640,480,32,SDL_SWSURFACE);

	intro(screen);

	SDL_CreateThread(connect_wiimotes,NULL);

	if(display_connectivity(screen))
	{
		SDL_Quit();
		return 0;
	}

	SDL_CreateThread(check_wiimotes,NULL);


	int quit = 0;

	// Wait for resources to be loaded at this point
	res_loaded();

	while(!quit)
	{
		SDL_Event ev;
		while(SDL_PollEvent(&ev))
		{
			if(ev.type == SDL_QUIT)
				quit = 1;
		}

		res_draw_kit(screen);
		SDL_Delay(10);
		SDL_Flip(screen);
	}
	SDL_Quit();
	return 0;
}
