/*
	Copyright (c) 2008 - Christopher Sawczuk

	NOTE: MUSIC BUNDLED WITH OPEN STAGE IS ONLY FOR USE
	WITH OPEN STAGE.
	
	If you like the featured music please consider purchasing
	the sound tracks (plus additional none-featured tracks)
	from Core//Mute.

	<GPL>

	</GPL>
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef WIN32
	#include <unistd.h>
	#define WIIUSE_PATH		"./wiiuse.so"
#else
	#define WIIUSE_PATH		"wiiuse.dll"
#endif

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

#include "wiiuse.h"
#include "song_eng.h"

SDL_Surface *screen = NULL;
TTF_Font **fonts;
Mix_Chunk *beep = NULL;

static long int speed = 0;
static unsigned int start_ticks = 0;

void draw_loading()
{
	SDL_Color text_col = { 0xff, 0xff, 0xff };
	SDL_FillRect(screen,NULL,0x00000000);
	SDL_Surface *text_surface = TTF_RenderText_Solid(fonts[0], "Loading...",text_col);
	if(!text_surface)
	{
		fprintf(stderr, "Failed creating text\n");
		return;
	}
	
	SDL_Rect target = { screen->w/2-text_surface->w/2,screen->h/2-text_surface->h/2,text_surface->w, text_surface->h };
	SDL_BlitSurface(text_surface, NULL, screen, &target);
	
	SDL_Flip(screen);
}

void draw_finding_wiimotes()
{
	SDL_Color text_col = { 0xff, 0xff, 0xff };
	SDL_FillRect(screen,NULL,0x00000000);
	SDL_Surface *text_surface = TTF_RenderText_Solid(fonts[0], "Finding Wiimotes",text_col);
	SDL_Surface *description  = TTF_RenderText_Solid(fonts[1], "Place the Wiimotes into discovery mode, by pushing the '1' and '2' buttons simultaneously.", text_col);
	if(!text_surface || !description)
	{
		fprintf(stderr, "Failed creating text\n");
		return;
	}
	
	SDL_Rect target = { screen->w/2-text_surface->w/2,10,text_surface->w, text_surface->h };
	SDL_BlitSurface(text_surface, NULL, screen, &target);
	
	target.x = screen->w/2-description->w/2;
	target.y = 48;
	target.w = description->w;
	target.h = description->h;
	SDL_BlitSurface(description, NULL, screen, &target);
	
	SDL_Flip(screen);
}

void handle_event(struct wiimote_t* wm) {
	//printf("\n\n--- EVENT [wiimote id %i] ---\n", wm->unid);
	
	/* if a button is pressed, report it */
	if (WIIUSE_USING_ACC(wm)) 
	{
		if (wm->btns) 
		{
	
			//printf("wiimote pitch = %f\n", wm->orient.pitch);
			if(wm->accel_calib.st_pitch<-150.0f)
			{
				if (IS_PRESSED(wm, WIIMOTE_BUTTON_A))
				{
				}
				if (IS_PRESSED(wm, WIIMOTE_BUTTON_B))
				{
				}
				if (IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN))
				{
				}
				if (IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN))
				{
				}
				if (IS_PRESSED(wm, WIIMOTE_BUTTON_RIGHT))
				{
				}
				if (IS_PRESSED(wm, WIIMOTE_BUTTON_LEFT))
				{
				}
			}
		}
		/*
		 *	Pressing minus will tell the wiimote we are no longer interested in movement.
		 *	This is useful because it saves battery power.
		 */
		if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_MINUS))
			wiiuse_motion_sensing(wm, 0);
		/*
		 *	Pressing plus will tell the wiimote we are interested in movement.
		 */
		if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_PLUS))
			wiiuse_motion_sensing(wm, 1);
	}

	/* if the accelerometer is turned on then print angles
	if (WIIUSE_USING_ACC(wm)) {
//		printf("wiimote roll  = %f\n", wm->orient.roll);
//		printf("wiimote pitch = %f\n", wm->orient.pitch);
//		printf("wiimote yaw   = %f\n", wm->orient.yaw);
	}
	 */
	/*
	 *	If IR tracking is enabled then print the coordinates
	 *	on the virtual screen that the wiimote is pointing to.
	 *
	 *	Also make sure that we see at least 1 dot.
	 */
/*	if (WIIUSE_USING_IR(wm)) 
	{
		int i = 0;

		// go through each of the 4 possible IR sources //
		for (; i < 4; ++i) {
			// check if the source is visible
			if (wm->ir.dot[i].visible)
				printf("IR source %i: (%u, %u)\n", i, wm->ir.dot[i].x, wm->ir.dot[i].y);
		}

		printf("IR cursor: (%u, %u)\n", wm->ir.x, wm->ir.y);
		printf("IR z distance: %f\n", wm->ir.z);
	}
*/
	/* show events specific to supported expansions 
	if (wm->exp.type == EXP_NUNCHUK) {
		// nunchuk
		struct nunchuk_t* nc = (nunchuk_t*)&wm->exp.nunchuk;

		if (IS_PRESSED(nc, NUNCHUK_BUTTON_C))		printf("Nunchuk: C pressed\n");
		if (IS_PRESSED(nc, NUNCHUK_BUTTON_Z))		printf("Nunchuk: Z pressed\n");

		printf("nunchuk roll  = %f\n", nc->orient.roll);
		printf("nunchuk pitch = %f\n", nc->orient.pitch);
		printf("nunchuk yaw   = %f\n", nc->orient.yaw);

		printf("nunchuk joystick angle:     %f\n", nc->js.ang);
		printf("nunchuk joystick magnitude: %f\n", nc->js.mag);
	}
	*/
}

void handle_read(struct wiimote_t* wm, byte* data, unsigned short len) {
	int i = 0;

	printf("\n\n--- DATA READ [wiimote id %i] ---\n", wm->unid);
	printf("finished read of size %i\n", len);
	for (; i < len; ++i) {
		if (!(i%16))
			printf("\n");
		printf("%x ", data[i]);
	}
	printf("\n\n");
}

void handle_ctrl_status(struct wiimote_t* wm, int attachment, int speaker, int ir, int led[4], float battery_level) {
	printf("\n\n--- CONTROLLER STATUS [wiimote id %i] ---\n", wm->unid);

	printf("attachment:      %i\n", attachment);
	printf("speaker:         %i\n", speaker);
	printf("ir:              %i\n", ir);
	printf("leds:            %i %i %i %i\n", led[0], led[1], led[2], led[3]);
	printf("battery:         %f %%\n", battery_level);
}

void handle_disconnect(wiimote* wm) {
	printf("\n\n--- DISCONNECTED [wiimote id %i] ---\n", wm->unid);
}

void draw_scene(void)
{
	static SDL_Surface *backg = NULL;
	if(!backg)
	{
		backg = SDL_CreateRGBSurface(SDL_HWSURFACE, screen->w, screen->h, 24, 0,0,0,0);
		SDL_FillRect(backg,NULL,0xffffffff);
	}
	SDL_BlitSurface(backg, NULL, screen, NULL);
}

static void draw_count_in(int num)
{
	SDL_Color text_col = { 0x0, 0x0, 0x0 };
	char text[1];
	sprintf(text,"%d",num);
	SDL_Surface *text_surface = TTF_RenderText_Solid(fonts[0], text, text_col);
	SDL_Rect pos;
	pos.x = screen->w/2-text_surface->w/2;
	pos.y = screen->h/2-text_surface->h/2;
	pos.w = text_surface->w;
	pos.h = text_surface->h;
	SDL_BlitSurface(text_surface,NULL,screen, &pos);
}

static void draw_notes(void)
{
	if(start_ticks == 0)
		start_ticks = SDL_GetTicks();
	static SDL_Surface *notes[5] = { NULL };
	if(!notes[0])
	{
		notes[0] = IMG_Load("g.png");
		notes[1] = IMG_Load("r.png");
		notes[2] = IMG_Load("y.png");
		notes[3] = IMG_Load("b.png");
		notes[4] = IMG_Load("p.png");
	}
	song_note *list = song_eng_get_notes();
	unsigned int ticks = SDL_GetTicks()-start_ticks;
	SDL_Rect pos;
	pos.w = notes[0]->w;
	pos.h = notes[0]->h;
	SDL_Rect end_pos;
	end_pos.x = 15;
	end_pos.y = screen->h-notes[0]->h;
	end_pos.w = notes[0]->w;
	end_pos.h = notes[1]->h;
	SDL_BlitSurface(notes[0],NULL,screen,&end_pos);
	end_pos.x = 15+notes[0]->w;
	SDL_BlitSurface(notes[1],NULL,screen,&end_pos);
	end_pos.x = 15+notes[0]->w*2;
	SDL_BlitSurface(notes[2],NULL,screen,&end_pos);
	end_pos.x = 15+notes[0]->w*3;
	SDL_BlitSurface(notes[3],NULL,screen,&end_pos);
	end_pos.x = 15+notes[0]->w*4;
	SDL_BlitSurface(notes[4],NULL,screen,&end_pos);
	while(list!=NULL)
	{
		/*
			Update notes that are only between a certain threshold to make sure we don't start blitting
			several thousand for no reason.
		*/
		if(list->t_off-speed<ticks)
		{
			pos.x = list->colour*notes[0]->w+15;
			pos.y = ((((ticks-list->t_off+speed)/(double)speed)*100*(screen->h/100.0))-notes[0]->h);
			SDL_BlitSurface(notes[list->colour],NULL,screen,&pos);
		}
		if(list->t_off>= ticks-5 && list->t_off<ticks+5 && list->played!=1)
		{
			list->played = 1;
			Mix_PlayChannel(-1,beep,0);
		}
		list = list->next;
	}
}

static int check_note_col(char instrument, char colour)
{
	/* 
		Since there could only be 5 notes at any given 
		time we wanna be checking, I only check the first
		5 notes :D. Also it should be impossible to whack
		2 same coloured notes in one swoop because the notes
		at the beginning of the list should be before the
		notes at the end of the list.
	*/
	
	int i;
	
	long int c_time = SDL_GetTicks()-start_ticks;
	
	song_note *tmp = song_eng_get_notes();
	
	/* Go past all the notes that younger than the current time */
	while(tmp->t_off<c_time+10)
	{
		tmp = tmp->next;
		if(tmp == NULL)
			return 0;
	}
	
	for(i = 0; i<5;i++)
	{
		if(tmp==NULL)
			return 0;
		if(tmp->colour == colour)
		{
			if(tmp->t_off>=c_time-100 && tmp->t_off<=c_time+100)
			{
				tmp->played = 1;
				printf("\n\n\nHit note at %d (Note time: %d)\n\n\n",c_time, tmp->t_off);
				return 1;
			}
		}
	}
	return 0;
}

#define WIIMOTE_ID_1		0
#define WIIMOTE_ID_2		1

int main(int argc, char** argv) 
{
	song_eng_load("test.gtar");
	
	speed = song_eng_get_speed();
	
	/* Initialise SDL */
	
	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)<0)
	{
		fprintf(stderr, "Failed intialising SDL\n");
		return 0;
	}
	
	atexit(SDL_Quit);
	
	screen = SDL_SetVideoMode(640,480,24,SDL_HWSURFACE|SDL_DOUBLEBUF);
	
	if(!screen)
	{
		fprintf(stderr, "Failed initialising screen\n");
		return 0;
	}
	
	SDL_WM_SetCaption("Open Stage", NULL);
	
	if(Mix_OpenAudio(44100, AUDIO_S16, 2, 1024)<0)
	{
		fprintf(stderr, "Failed initialising audio subsystem\n");
	}
	
	if(TTF_Init()==-1) 
	{
		fprintf(stderr,"TTF_Init: %s\n", TTF_GetError());
		return 0;
	}
	
	fonts = malloc(sizeof(TTF_Font*)*2);
	fonts[0] = TTF_OpenFont("arial.ttf",24);
	fonts[1] = TTF_OpenFont("arial.ttf",12);
	
	draw_loading();

	Mix_Chunk *song = Mix_LoadWAV(song_eng_get_song());
	beep = Mix_LoadWAV("beep.ogg");
	
	if(!song)
	{
		fprintf(stderr, "Failed loading %s song\n",song_eng_get_song());
		return 1;
	}

/*
	int ids[] = { WIIMOTE_ID_1, WIIMOTE_ID_2 };

	const char* version;
	wiimote** wiimotes;
	int found, connected;

	version = wiiuse_startup(WIIUSE_PATH);
	printf("Wiiuse Version = %s\n", version);
	if (!version) {
		fprintf(stderr, "Failed to load wiiuse library.\n");
		return 0;
	}

	wiimotes =  wiiuse_init(16, ids, handle_event, handle_ctrl_status, handle_disconnect);
	
	draw_finding_wiimotes();
	
	found = wiiuse_find(wiimotes, 2, 5);
	if (!found)
		return 0;
	connected = wiiuse_connect(wiimotes, 2);
	if (connected)
		printf("Connected to %i wiimotes (of %i found).\n", connected, found);
	else {
		printf("Failed to connect to any wiimote.\n");
		return 0;
	}

	wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_1);
	wiiuse_set_leds(wiimotes[1], WIIMOTE_LED_2);
	wiiuse_motion_sensing(wiimotes[0], 1);
	wiiuse_motion_sensing(wiimotes[1], 1);
	wiiuse_rumble(wiimotes[0], 1);
	wiiuse_rumble(wiimotes[1], 1);

	#ifndef WIN32
		usleep(200000);
	#else
		Sleep(200);
	#endif


	wiiuse_rumble(wiimotes[0], 0);
	wiiuse_rumble(wiimotes[1], 0);

	wiiuse_status(wiimotes[0]);
*/
	/*
	 *	This is the main loop
	 *
	 *	wiiuse_poll() needs to be called with the wiimote array
	 *	and the number of wiimote structures in that array
	 *	(it doesn't matter if some of those wiimotes are not used
	 *	or are not connected).
	 *
	 *	This function will invoke the callbacks set in wiiuse_init()
	 *	when the wiimote has things to report.
	 */
	int quit = 0;
	
	int i = 5;
	char song_started = 0;
	unsigned int last_tick = SDL_GetTicks();
	while (!quit) 
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					quit = 1;
					break;
				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_a)
					{
						check_note_col(0,0);
					}
					if(event.key.keysym.sym == SDLK_s)
					{
						check_note_col(0,1);
					}
					if(event.key.keysym.sym == SDLK_d)
					{
						check_note_col(0,2);
					}
					if(event.key.keysym.sym == SDLK_f)
					{
						check_note_col(0,3);
					}
					if(event.key.keysym.sym == SDLK_g)
					{
						check_note_col(0,4);
					}
					break;
			}
		}
		SDL_FillRect(screen, NULL, 0x00000000);
		/*wiiuse_poll(wiimotes, 2);*/
		draw_scene();
		if(i>0)
		{
			draw_count_in(i);
			if(SDL_GetTicks()>last_tick+1000)
			{
				i--;
				last_tick = SDL_GetTicks();
			}
		}
		else if(!song_started)
		{
			song_started = 1;
			Mix_PlayChannel(-1, song, 0);
		}
		draw_notes();
		SDL_Flip(screen);
	}

	/*
	 *	Disconnect the wiimotes
	wiiuse_disconnect(wiimotes[0]);
	wiiuse_disconnect(wiimotes[1]);

	 */
	/*
	 *	Unload the wiiuse library
	
	wiiuse_shutdown();
	*/
	TTF_CloseFont(fonts[0]);
	TTF_CloseFont(fonts[1]);
	Mix_CloseAudio();
	SDL_Quit();
	return 0;
}
