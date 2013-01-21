#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <math.h>

snd_pcm_t *open_sound()
{
	int err;
	snd_pcm_t *playback_handle;
	snd_pcm_hw_params_t *hw_params;
	if((err = snd_pcm_open(&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0)
		exit(1);
	if((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
		exit(1);
	if((err = snd_pcm_hw_params_any(playback_handle, hw_params)) < 0)
		exit(1);
	if((err = snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
		exit(1);
	if((err = snd_pcm_hw_params_set_format(playback_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0)
		exit(1);

	int rate = 22050;
	if((err = snd_pcm_hw_params_set_rate_near(playback_handle, hw_params, &rate, 0)) < 0)
		exit(1);
	if((err = snd_pcm_hw_params_set_channels(playback_handle, hw_params, 2)) < 0)
		exit(1);
	if((err = snd_pcm_hw_params(playback_handle, hw_params)) < 0)
		exit(1);
	snd_pcm_hw_params_free(hw_params);
	if((err = snd_pcm_prepare(playback_handle)) < 0)
		exit(1);
	return playback_handle;
}

void play_sound(snd_pcm_t *ply_hnd, short buf[50])
{
	snd_pcm_writei(ply_hnd, buf, 50/2);
}

void generate_tone(short buf[50], double tone)
{
	int i;
	double interval = 1.0/tone;
	double samp_per_cycle = interval * 22050; 
	for(i = 0; i < 50; i++)
	{
		double x = (i / samp_per_cycle) * 2*3.142;
		buf[i] = .1 * (8 * sin(x) +sin(6 * x) + sin(4 * x))*4000;
	}
}

int main(int argc, char *argv[])
{
	short buf[50];
	snd_pcm_t *plybck_hnd = open_sound();
	printf("Playing\n");
	generate_tone(buf,440.0);
	while(1)
		play_sound(plybck_hnd,buf);
	
	snd_pcm_close(plybck_hnd);
	return 0;
}

