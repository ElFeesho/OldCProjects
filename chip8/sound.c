#include <SDL/SDL.h>

#include <math.h>

static Uint8 *audio_chunk;
static Uint32 audio_len;
static Uint8 *audio_pos;

static void fill_audio(void *udata, Uint8 *stream, int len)
{
	if (audio_len == 0)
	{
		return;
	}

    /* Mix as much data as possible */
    len = ( len > audio_len ? audio_len : len );
    SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
    audio_pos += len;
    audio_len -= len;

    if(audio_len == 0)
    {
    	audio_len = 512;
    	audio_pos = audio_chunk;
    }
}

void sound_init()
{
	SDL_Init(SDL_INIT_AUDIO);
	SDL_AudioSpec wanted;

    /* Set the audio format */
    wanted.freq = 22050;
    wanted.format = AUDIO_S16;
    wanted.channels = 2;    /* 1 = mono, 2 = stereo */
    wanted.samples = 1024;  /* Good low-latency value for callback */
    wanted.callback = fill_audio;
    wanted.userdata = NULL;

    /* Open the audio device, forcing the desired format */
    if ( SDL_OpenAudio(&wanted, NULL) < 0 ) {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
    }
    audio_chunk = malloc(1024);
    audio_pos = audio_chunk;
    double it = 0.0;
    double inc = (M_PI*8.0f)/440.0f;
    for(int i = 0; i < 512; i++)
    {
    	audio_chunk[i] = sin(it)*128.0;
    	it+=inc;
    }
}

void sound_start()
{
	audio_len = 512;
	SDL_PauseAudio(0);
}

void sound_stop()
{
	audio_len = 0;
	SDL_PauseAudio(1);
}