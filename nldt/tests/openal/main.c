#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <cwiid.h>

cwiid_wiimote_t *wii_remote = NULL;

// Buffers hold sound data.
ALuint Buffer;

// Sources are points emitting sound.
ALuint Source;

// Position of the source sound.
ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };

// Velocity of the source sound.
ALfloat SourceVel[] = { 0.0, 0.0, 1.0 };


// Position of the listener.
ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };

// Velocity of the listener.
ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };

// Orientation of the listener. (first 3 elements are "at", second 3 are "up")
ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };

struct acc_cal cal;

void cwiid_callback(cwiid_wiimote_t *wiimote, int mesg_count, union cwiid_mesg mesg[], struct timespec *timestamp)
{
	

	if(mesg_count > 0)
	{
		int i;
		for (i=0; i < mesg_count; i++) 
		{
			switch (mesg[i].type) 
			{
				case CWIID_MESG_ACC:
				{
					double a_x, a_y;
					a_x = ((double)mesg[i].acc_mesg.acc[CWIID_X] - cal.zero[CWIID_X]) /	(cal.one[CWIID_X] - cal.zero[CWIID_X]);
					a_y = ((double)mesg[i].acc_mesg.acc[CWIID_Y] - cal.zero[CWIID_Y]) /	(cal.one[CWIID_Y] - cal.zero[CWIID_Y]);
					alSourcef (Source, AL_PITCH,    1.0+((a_x)/4));
					alSourcef (Source, AL_GAIN,    1.0+a_y);
				}
				break;
				default:
				break;
			}
		}
	}
}

void find_wiimote(GtkWidget *widget, gpointer user_data)
{
	static int timer = 0;
	if(timer!=0)
	{
		cwiid_close(wii_remote);
		wii_remote == NULL;
		cwiid_set_mesg_callback(wii_remote,NULL);
		gtk_button_set_label(GTK_BUTTON(widget),"Connect to Wiimote");
		return;
	}
	bdaddr_t baddr;
	int ret = 0;
	
	ret = cwiid_find_wiimote(&baddr, 5); 
	if(ret==0)
	{
		wii_remote = cwiid_open(&baddr, CWIID_FLAG_MESG_IFC);
		cwiid_set_led(wii_remote, CWIID_LED1_ON);
		cwiid_set_rpt_mode(wii_remote, CWIID_RPT_ACC);
		cwiid_get_acc_cal(wii_remote, CWIID_EXT_NONE, &cal);
		gtk_button_set_label(GTK_BUTTON(widget),"Disconnect Wiimote");
		cwiid_set_mesg_callback(wii_remote,&cwiid_callback);
		timer = 1;
	}
	else
	{
		printf("Failed to find wiimote\n");
	}
	
}

void openOggVorbis(char *file, ALenum *format, ALvoid **data, ALsizei *size, ALsizei *freq, ALboolean *loop)
{
	OggVorbis_File vf;
	int eof=0;
	int current_section;
	if(ov_fopen(file,&vf) < 0) 
	{
		fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
		exit(1);
	}
	
	char **ptr=ov_comment(&vf,-1)->user_comments;
	vorbis_info *vi=ov_info(&vf,-1);
	while(*ptr)
	{
		fprintf(stderr,"%s\n",*ptr);
		++ptr;
	}
	*format = AL_FORMAT_STEREO16;
	*freq = vi->rate;
	*size = ov_pcm_total(&vf,-1)*4; // 16 bit stereo, so multiply by 2 then 2 again because of stereo :D
	*loop = AL_TRUE;
	*data = malloc((*size));
	int pos = 0;
	char pcmout[4096];
	do
	{
		long ret=ov_read(&vf,pcmout,sizeof(pcmout),0,2,1,&current_section);
		if (ret == 0) {
			/* EOF */
			eof=1;
		} 
		else if (ret < 0) 
		{
			fprintf(stderr,"Read error?\n");
		}
		else
		{
			memcpy((*data)+pos,pcmout,ret);
			pos+=ret;
		}
	}while(!eof);
	printf("Loaded Ogg!\n");
}

ALboolean LoadALData(char *filename)
{
    // Variables to load into.

    ALenum format;
    ALsizei size;
    ALvoid* data;
    ALsizei freq;
    ALboolean loop;
    // Load wav data into a buffer.
    alGenBuffers(1, &Buffer);
    if (alGetError() != AL_NO_ERROR)
        return AL_FALSE;

    openOggVorbis(filename, &format, &data, &size, &freq, &loop);
    alBufferData(Buffer, format, data, size, freq);
    free(data);
    // Bind buffer with a source.
    alGenSources(1, &Source);

    if (alGetError() != AL_NO_ERROR)
        return AL_FALSE;

    alSourcei (Source, AL_BUFFER,   Buffer   );
    alSourcef (Source, AL_PITCH,    1.0f     );
    alSourcef (Source, AL_GAIN,     1.0f     );
    alSourcefv(Source, AL_POSITION, SourcePos);
    alSourcefv(Source, AL_VELOCITY, SourceVel);
    alSourcei (Source, AL_LOOPING,  AL_TRUE);

    // Do another error check and return.
    if (alGetError() == AL_NO_ERROR)
        return AL_TRUE;

    return AL_FALSE;
}

void SetListenerValues()
{
    alListenerfv(AL_POSITION,    ListenerPos);
    alListenerfv(AL_VELOCITY,    ListenerVel);
    alListenerfv(AL_ORIENTATION, ListenerOri);
}

static gboolean window_close()
{
    alDeleteBuffers(1, &Buffer);
    alDeleteSources(1, &Source);
    alutExit();
	 if(wii_remote!=NULL)
		cwiid_close(wii_remote);
	 gtk_exit(0);
	 return FALSE;
}

static void play_clicked(GtkWidget *button, gpointer data)
{
	alSourcePlay(Source);
}

static void stop_clicked(GtkWidget *button, gpointer data)
{
	alSourceStop(Source);
}
static gboolean pitch_bend(GtkRange *range, GtkScrollType scroll, gdouble value, gpointer user_data)
{
	alSourcef (Source, AL_PITCH,value);
	return FALSE;
}

int main(int argc, char *argv[])
{
    // Initialize OpenAL and clear the error bit.
	gtk_init(&argc, &argv);
	GtkWidget *window, *hbox, *connect, *play, *stop, *pitch;
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	hbox = gtk_hbox_new(FALSE,0);
	connect = gtk_button_new_with_label("Connect to Wiimote");
	play = gtk_button_new_with_label("Play");
	stop = gtk_button_new_with_label("Stop");
	pitch = gtk_hscale_new_with_range(0.5,2.0,0.01);
	gtk_range_set_value(GTK_RANGE(pitch),1.0);

	gtk_box_pack_start(GTK_BOX(hbox),connect,TRUE,TRUE,0);	
	gtk_box_pack_start(GTK_BOX(hbox),play,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbox),stop,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbox),pitch,TRUE,TRUE,0);
	
	gtk_container_add(GTK_CONTAINER(window),hbox);
	
	alutInit(&argc, argv);
	alGetError();
	// Load the wav data.
	if (LoadALData(argv[1]) == AL_FALSE)
		return -1;

	SetListenerValues();
	
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(window_close),NULL);
	g_signal_connect(G_OBJECT(connect),"clicked",G_CALLBACK(find_wiimote),NULL);
	g_signal_connect(G_OBJECT(play),"clicked",G_CALLBACK(play_clicked),NULL);
	g_signal_connect(G_OBJECT(stop),"clicked",G_CALLBACK(stop_clicked),NULL);
	g_signal_connect(G_OBJECT(pitch),"change-value",G_CALLBACK(pitch_bend),NULL);
	gtk_widget_show_all(window);
	gtk_main();		
	
	return 0;
}
