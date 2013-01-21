#include <stdio.h>
#include <termios.h>
#include <sys/select.h>
#include <fcntl.h>
#include <alsa/asoundlib.h>
#include <math.h>

snd_pcm_t *open_sound()
{
	int err;
	snd_pcm_t *playback_handle;
	snd_pcm_hw_params_t *hw_params;
	if((err = snd_pcm_open(&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK)) < 0)
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
	snd_pcm_writei(ply_hnd, buf, sizeof(buf));
}

void generate_tone(short buf[50], double tone)
{
	int i;
	double interval = 1.0/tone;
	double samp_per_cycle = interval * 22050; 
	for(i = 0; i < 50; i++)
	{
		double x = (i / samp_per_cycle) * 2*3.142;
		buf[i] = sin(x)*5000;
	}
}

int main(int argc, char ** argv)
{
	snd_pcm_t *phandle = open_sound();
	short sbuf[50] = { '\0' };

	printf("Termio Test\n");
	struct termios settings, original;

	tcgetattr(0,&original);
	tcgetattr(0,&settings);

	settings.c_lflag ^= ECHO;
	settings.c_lflag ^= ICANON;

	tcsetattr(0,TCSANOW,&settings);

	fd_set input;
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	for(;;)
	{
		FD_ZERO(&input);
		FD_SET(0,&input);
		int am = select(1, &input, NULL, NULL, &timeout);
		if(am>0)
		{
			if(FD_ISSET(0,&input))
			{
				int key = fgetc(stdin);
				while(key == 27)
				{
					if(fseek(stdin,1,SEEK_CUR)==-1)
					{
						printf("No more chars to read!\n");
						break;
					}
					fseek(stdin,-1,SEEK_CUR);
					key = fgetc(stdin);
					if(key == 91)
					{
						key = fgetc(stdin); //Cursor Key!
						break;
					}
				}
				if(key == 'q')
				{
					printf("Quitting\n");
					tcsetattr(0,TCSANOW,&original);
					return 0;
				}
				printf("Key: %d\n",key);
				if(key == 'a')
				{
					generate_tone(sbuf,440.0);
				}
				if(key == 's')
				{
					generate_tone(sbuf,493.88);
				}
				if(key == 'd')
				{
					generate_tone(sbuf,523.25);
				}
				if(key == 'f')
				{
					generate_tone(sbuf,587.33);
				}
				if(key == 'g')
				{
					generate_tone(sbuf,659.26);
				}
				if(key == 'h')
				{
					generate_tone(sbuf,698.46);
				}
				if(key == 'j')
				{
					generate_tone(sbuf,783.99);
				}
			}
		}
		play_sound(phandle,sbuf);
	}
	return 0;
}
