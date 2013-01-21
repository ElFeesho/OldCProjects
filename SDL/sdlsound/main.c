#include <stdio.h>
#include "SDL.h"
#include "SDL_audio.h"


int main(int argc, char **argv)
{

extern void mixaudio(void *unused, Uint8 *stream, int len)
{
	printf("Mixing\n");
	Uint32 amount;
	int i = 0;
	for(i = 0;i < 255; i++)
		SDL_MixAudio(stream, 878, 8008, SDL_MIX_MAXVOLUME);
}

	SDL_Init(SDL_INIT_AUDIO);
	SDL_AudioSpec fmt;

	/* Set 16-bit stereo audio at 22Khz */
	fmt.freq = 22050;
	fmt.format = AUDIO_S16;
	fmt.channels = 2;
	fmt.samples = 512;        /* A good value for games */
	fmt.callback = mixaudio;
	fmt.userdata = NULL;

	/* Open the audio device and start playing sound! */
	if ( SDL_OpenAudio(&fmt, NULL) < 0 ) {
		fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_PauseAudio(0);

	SDL_CloseAudio();
	SDL_Quit();
	return 0;
}
