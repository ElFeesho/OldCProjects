#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <wiiuse.h>

SDL_mutex *wii_lock = NULL;

wiimote** wiimotes = NULL;

int wiimotes_connected = 0;

int event_thread(void *data);

int init_sdl(void)
{
	if(SDL_Init(SDL_INIT_AUDIO)<0)
	{
		fprintf(stderr, "Failed to initialise SDL for audio\n");
		return -1;
	}

	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) < 0)
	{
		fprintf(stderr, "Error initializing SDL_mixer: %s\n", Mix_GetError());
		return -2;
	}

	wii_lock = SDL_CreateMutex();

	SDL_CreateThread(event_thread, NULL);

	return 0;
}

void play_beep(void)
{
	static Mix_Chunk *beep = NULL;
	if(beep == NULL)
	{
		beep = Mix_LoadWAV("sounds/beep.wav");
		if(!beep)
		{
			fprintf(stderr, "Couldn't load sounds/beep.wav\n");
		}
	}
	Mix_PlayChannel(-1, beep, 0);
}

void play_ricochet(void)
{
	static Mix_Chunk *ricochets[4] = { NULL };
	if(ricochets[0] == NULL)
	{
		int i;
		for(i = 0;i<4;i++)
		{
			char *filename = malloc(1024);
			sprintf(filename, "sounds/ricochet%i.wav",i+1);
			ricochets[i] = Mix_LoadWAV(filename);
			if(!ricochets[i])
			{
				fprintf(stderr, "Couldn't load %s\n",filename);
			}
			free(filename);
		}
	}
	int randC = rand()%4;
	Mix_PlayChannel(-1, ricochets[randC], 0);
}

int find_wiimotes(void)
{
	if(wiimotes == NULL)
	{
		wiimotes = wiiuse_init(4);
	}

	int found = wiiuse_find(wiimotes+wiimotes_connected, 4-wiimotes_connected, 5);
	if(!found)
		return 0;

	SDL_mutexP(wii_lock);

	int connected = wiiuse_connect(wiimotes+wiimotes_connected, 4-wiimotes_connected);
	if(connected>0)
	{
		wiimotes_connected+=connected;
		int i = 0;
		for(;i<wiimotes_connected;i++)
		{
			wiiuse_set_leds(wiimotes[i], WIIMOTE_LED_1<<i);
		}
	}

	SDL_mutexV(wii_lock);
}

int event_thread(void *data)
{
	for(;;)
	{
		SDL_mutexP(wii_lock);

		if(wiiuse_poll(wiimotes, wiimotes_connected))
		{
			int i;
			for(i = 0;i<wiimotes_connected;i++)
			{
				if(wiimotes[i]->event == WIIUSE_EVENT)
				{
					if (IS_JUST_PRESSED(wiimotes[i], WIIMOTE_BUTTON_A))
					{
						play_beep();
					}
					if (IS_JUST_PRESSED(wiimotes[i], WIIMOTE_BUTTON_B))
					{
						play_ricochet();
					}
				}
				else if(wiimotes[i]->event == WIIUSE_DISCONNECT)
				{
					wiiuse_cleanup(wiimotes, 4);
					wiimotes_connected = 0;
					wiimotes = NULL;
					break;
				}
			}
		}

		SDL_mutexV(wii_lock);
	}
}
