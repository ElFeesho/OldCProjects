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

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

#include "song_eng.h"

SDL_Surface *screen = NULL;
TTF_Font **fonts;
Mix_Chunk *beep = NULL;

static long int speed = 0;
static unsigned int start_ticks = 0;

static int keys_down = 0;

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

void draw_scene(void)
{
	static SDL_Surface *backg = NULL, *trough = NULL;
	if(!backg)
	{
		backg = SDL_CreateRGBSurface(SDL_HWSURFACE, screen->w, screen->h, 24, 0,0,0,0);
		SDL_FillRect(backg,NULL,0xffffffff);
		trough = IMG_Load("trough.png");
		SDL_Rect end_pos;
		end_pos.x = 15;
		end_pos.y = screen->h-trough->h;
		end_pos.w = trough->w;
		end_pos.h = trough->h;
		SDL_BlitSurface(trough,NULL,backg,&end_pos);
		end_pos.x = 15+trough->w;
		SDL_BlitSurface(trough,NULL,backg,&end_pos);
		end_pos.x = 15+trough->w*2;
		SDL_BlitSurface(trough,NULL,backg,&end_pos);
		end_pos.x = 15+trough->w*3;
		SDL_BlitSurface(trough,NULL,backg,&end_pos);
		end_pos.x = 15+trough->w*4;
		SDL_BlitSurface(trough,NULL,backg,&end_pos);
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
	SDL_Rect pos;
	pos.w = notes[0]->w;
	pos.h = notes[0]->h;
	song_note *list = song_eng_get_notes();
	unsigned long int ticks = SDL_GetTicks()-start_ticks;
	
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
		if(pos.y>screen->h-notes[0]->h+20 && list->played!=1)
		{
			printf("Beeping at %ld NOTE TIME: %ld\n",ticks,list->t_off);
			list->played = 1;
			Mix_PlayChannel(-1,beep,0);
		}
		list = list->next;
	}
}

static int check_note_col()
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

	unsigned long int c_time = SDL_GetTicks()-start_ticks;

	song_note *tmp = song_eng_get_notes();

	printf("CTIME: %ld\n",c_time);

	/*
		Round up an array of notes that _could_ be played

		Can only ever be 5 notes being possible to play
	*/
	song_note *notes[5]; 
	int notes_found = 0;
	/*
		To make sure that no notes that shouldn't be played
		get played (woah), gonna store the note time, and
		if another note appears and it has a HIGHER note time
		i.e. it needs to be played later, it can be disregarded.

		Also, any 'chords' will have to have the same time in order
		for them to be played. 
	*/
	long note_time = 0;
	/* Go past all the notes that younger than the current time */
	while(tmp->t_off<c_time+10)
	{
		tmp = tmp->next;
		if(tmp == NULL)
		{
			printf("Didn't find any potential notes\n");
			return 0;
		}
	}

	for(i = 0; i<5;i++)
	{
		if(tmp==NULL)
		{
			printf("Found some potential notes- didn't hit em on time CTIME: %ld\n", c_time);
			return 0;
		}
		if(tmp->t_off>=c_time-100 && tmp->t_off<=c_time+100)
		{
			if(notes_found == 0)
			{
				notes[notes_found++] = tmp;
				note_time = tmp->t_off;
			}
			else
			{
				if(notes[notes_found-1]->t_off==note_time)
				{
					/* We have a chord barnaby! */
					notes[notes_found++] = tmp;
				}
			}
		}
		tmp = tmp->next;
	}

	/* Now that we've rounded up potential notes - it's time to neutralise them! */

	if(notes_found == 1)
	{
		printf("Got a signle note\n");
		/* 
			One note is a special case because:
			Following other games like this, if your playing a guitar
			and for the hell of it a drumkit or synth, when you play
			a fret higher than the fret your holding, the highest
			fret takes precedence, so, the trick here is,
			We know that if your holding Green Red and Yellow
			the keys_down var will have 1 + 2 + 4, which = 7.
			So if the colour was yellow, we know we can say "yeah it's played"
			because the combination of keys_down is note > 4*2, you dig?

			Calling the power function a lot, so just set it as a var to save calls.
		*/
		int c_pow = pow(2,notes[0]->colour);
		printf("C_POW: %X\n",c_pow);
		if((keys_down&c_pow) == c_pow && keys_down<(c_pow*2))
		{
			printf("\n\n\nNote hit @ %ld (Note time: %ld)\n\n\n",c_time, notes[0]->t_off);
			notes[0]->played = 1;
		}
	}
	else if(notes_found>1)
	{
		/*
			Sadly chords aren't as clean as single notes- but it's not that bad :D

			The trick here is, make a nice huge binary mask of all the notes we've 
			got down, then if keys_down ^ bmask != 0, then all the right keys aren't down!

			You dig?
		*/

		printf("Got a chord!\n");

		int bmask = 0, i = 0;

		for(i=notes_found-1;i>=0;i--)
		{
			printf("BMASKING Colour: %d\n", notes[i]->colour);
			bmask |= (int)pow(2,notes[i]->colour);
		}
		printf("B MASK: %X\n", bmask);
		if((keys_down^bmask)==0)
		{
			printf("\n\n\nNote hit @ %ld (Note time: %ld)\n\n\n",c_time, notes[0]->t_off);
			for(i=0;i<notes_found;i++)
				notes[i]->played = 1;
		}
	}

	return 0;
}

int main(int argc, char** argv) 
{
	printf("ARGC: %d\n", argc);
	song_eng_load("test.gtar");

	speed = song_eng_get_speed();

	/* Initialise SDL */

	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)<0)
	{
		fprintf(stderr, "Failed intialising SDL\n");
		return 0;
	}

	screen = SDL_SetVideoMode(640,480,24,SDL_SWSURFACE);

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

	int quit = 0;

	int i = 5;
	char song_started = 0;
	unsigned int last_tick = SDL_GetTicks();
	printf("TICKS: %d\n",last_tick);
	while (!quit)
	{
		SDL_FillRect(screen, NULL, 0x00000000);

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

		SDL_Event event;

		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					quit = 1;
					break;
				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_F1)
					{
						keys_down |= 0x1;
					}
					if(event.key.keysym.sym == SDLK_F2)
					{
						keys_down |= 0x02;
					}
					if(event.key.keysym.sym == SDLK_F3)
					{
						keys_down |= 0x04;
					}
					if(event.key.keysym.sym == SDLK_F4)
					{
						keys_down |= 0x08;
					}
					if(event.key.keysym.sym == SDLK_F5)
					{
						keys_down |= 0x10;
					}
					if(event.key.keysym.sym == SDLK_RETURN)
					{
						check_note_col();
					}
					printf("Keys down: %X\n",keys_down);
					break;
				case SDL_KEYUP:
					if(event.key.keysym.sym == SDLK_F1)
					{
						keys_down ^= 0x1;
					}
					if(event.key.keysym.sym == SDLK_F2)
					{
						keys_down ^= 0x02;
					}
					if(event.key.keysym.sym == SDLK_F3)
					{
						keys_down ^= 0x04;
					}
					if(event.key.keysym.sym == SDLK_F4)
					{
						keys_down ^= 0x08;
					}
					if(event.key.keysym.sym == SDLK_F5)
					{
						keys_down ^= 0x10;
					}
					printf("Keys down: %X\n",keys_down);
					break;
			}
		}
	}

	TTF_CloseFont(fonts[0]);
	TTF_CloseFont(fonts[1]);
	Mix_CloseAudio();
	SDL_Quit();
	return 0;
}
