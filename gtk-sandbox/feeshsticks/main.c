#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/timeb.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <cwiid.h>

GtkWidget *ypos[2], *dwn[2], *up[2];

time_t start_time;

typedef enum
{
	DRUM_SNAREL,
	DRUM_SNARER,
	DRUM_HIHAT,
	DRUM_HIHATC,
	DRUM_HIHATP,
	DRUM_BDRUM,
	DRUM_CRASH,
	DRUM_CHCRASH,
	DRUM_SPLASH,
	DRUM_LTOM,
	DRUM_MTOM,
	DRUM_HTOM,
	DRUM_RIDE,
	DRUM_SNAREST
}drums;

cwiid_wiimote_t *wii_remote[2] = { NULL };

// Buffers hold sound data.
ALuint Buffer[14];

// Sources are points emitting sound.
ALuint Source[14];

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

struct acc_cal cal[2], nc_cal[2];

void cwiid_callback(cwiid_wiimote_t *wiimote, int mesg_count, union cwiid_mesg mesg[], struct timespec *timestamp)
{
	static struct timeb cur_time = { 0,0,0,0 };
	if(cur_time.time == 0) // Impossible unless just initialised
		ftime(&cur_time);
	static char down[2] = { '\0' };
	static int buttons = 0;
	if(mesg_count > 0)
	{
		int i;
		for (i=0; i < mesg_count; i++) 
		{
			switch (mesg[i].type) 
			{
				case CWIID_MESG_BTN:
				{
					if(mesg[i].btn_mesg.buttons==0)
					{
						buttons = DRUM_SNARER;
					}
					else
					{
						if((mesg[i].btn_mesg.buttons&CWIID_BTN_B) == CWIID_BTN_B)
						{
							buttons = DRUM_HIHAT;
							break;
						}
						if((mesg[i].btn_mesg.buttons&CWIID_BTN_A) == CWIID_BTN_A)
						{
							buttons = DRUM_CRASH;
							break;
						}
						if((mesg[i].btn_mesg.buttons&CWIID_BTN_MINUS) == CWIID_BTN_MINUS)
						{
							buttons = DRUM_LTOM;
							break;
						}
						if((mesg[i].btn_mesg.buttons&CWIID_BTN_HOME) == CWIID_BTN_HOME)
						{
							buttons = DRUM_MTOM;
							break;
						}
						if((mesg[i].btn_mesg.buttons&CWIID_BTN_PLUS) == CWIID_BTN_PLUS)
						{
							buttons = DRUM_HTOM;
							break;
						}
						if((mesg[i].btn_mesg.buttons&CWIID_BTN_DOWN) == CWIID_BTN_DOWN)
						{
							buttons = DRUM_RIDE;
							break;
						}
						if((mesg[i].btn_mesg.buttons&CWIID_BTN_LEFT) == CWIID_BTN_LEFT)
						{
							buttons = DRUM_CHCRASH;
							break;
						}
						if((mesg[i].btn_mesg.buttons&CWIID_BTN_RIGHT) == CWIID_BTN_RIGHT)
						{
							buttons = DRUM_SPLASH;
							break;
						}
						if((mesg[i].btn_mesg.buttons&CWIID_BTN_1) == CWIID_BTN_1)
						{
							buttons = DRUM_SNAREST;
							break;
						}
					}
				}
				break;
				case CWIID_MESG_ACC:
				{
					struct timeb now_time;
					ftime(&now_time);
					unsigned long n_msecs = ((now_time.time-start_time) * 1000 + now_time.millitm);
					unsigned long c_msecs = ((cur_time.time-start_time) * 1000 + cur_time.millitm);
					double a_x, a_y, a_z;
					a_x = ((double)mesg[i].acc_mesg.acc[CWIID_X] - cal[0].zero[CWIID_X]) /	(cal[0].one[CWIID_X] - cal[0].zero[CWIID_X]);
					a_y = ((double)mesg[i].acc_mesg.acc[CWIID_Y] - cal[0].zero[CWIID_Y]) /	(cal[0].one[CWIID_Y] - cal[0].zero[CWIID_Y]);
					a_z = ((double)mesg[i].acc_mesg.acc[CWIID_Z] - cal[0].zero[CWIID_Z]) /	(cal[0].one[CWIID_Z] - cal[0].zero[CWIID_Z]);
					if(a_z>gtk_range_get_value(GTK_RANGE((dwn[0]))) && down[0] == 0)
					{
						
						down[0] = 1;
						alSourcePlay(Source[buttons]);
						ftime(&cur_time);
					}
					else
					{
						//if(n_msecs < c_msecs + 100)
						//	break;
						if(a_z<gtk_range_get_value(GTK_RANGE((up[0]))))
							down[0] = 0;
					}
				}
				break;
								case CWIID_MESG_NUNCHUK:
				{
					double a_x, a_y, a_z;
					a_x = ((double)mesg[i].nunchuk_mesg.acc[CWIID_X] - nc_cal[1].zero[CWIID_X]) /	(nc_cal[1].one[CWIID_X] - nc_cal[1].zero[CWIID_X]);
					a_y = ((double)mesg[i].nunchuk_mesg.acc[CWIID_Y] - nc_cal[1].zero[CWIID_Y]) /	(nc_cal[1].one[CWIID_Y] - nc_cal[1].zero[CWIID_Y]);
					a_z = ((double)mesg[i].nunchuk_mesg.acc[CWIID_Z] - nc_cal[1].zero[CWIID_Z]) /	(nc_cal[1].one[CWIID_Z] - nc_cal[1].zero[CWIID_Z]);
					//printf("X: %.2f\n Y: %.2f Z: %.2f\n",a_x,a_y,a_z);
					if(a_z<-0.7 && down[1] == 0)
					{
						down[1] = 1;
						//if(a_x<-0.5)
							//alSourcePlay(Source[DRUM_SNARE]);
						//if(a_x>-0.5 && a_x<0.5)
							alSourcePlay(Source[DRUM_BDRUM]);
						//if(a_x>0.5)
							//alSourcePlay(Source[DRUM_CRASH]);
					}
					else
					{
						if(a_z>-0.55)
							down[1] = 0;
					}
				}
				break;
			}
		}
	}
}

void cwiid_callback2(cwiid_wiimote_t *wiimote, int mesg_count, union cwiid_mesg mesg[], struct timespec *timestamp)
{
	static struct timeb cur_time = { 0,0,0,0 };
	if(cur_time.time == 0) // Impossible unless just initialised
		ftime(&cur_time);
	static char down[2] = { '\0' };
	static buttons = 0;
	if(mesg_count > 0)
	{
		int i;
		for (i=0; i < mesg_count; i++) 
		{
			switch (mesg[i].type) 
			{
				case CWIID_MESG_BTN:
				{
					if(mesg[i].btn_mesg.buttons==0)
					{
						buttons = DRUM_SNAREL;
					}
					else
					{
						if((mesg[i].btn_mesg.buttons&CWIID_BTN_B) == CWIID_BTN_B)
						{
							buttons = DRUM_HIHAT;
							break;
						}
						if((mesg[i].btn_mesg.buttons&CWIID_BTN_A) == CWIID_BTN_A)
						{
							buttons = DRUM_CRASH;
							break;
						}
						if((mesg[i].btn_mesg.buttons&CWIID_BTN_MINUS) == CWIID_BTN_MINUS)
						{
							buttons = DRUM_LTOM;
							break;
						}
						if((mesg[i].btn_mesg.buttons&CWIID_BTN_HOME) == CWIID_BTN_HOME)
						{
							buttons = DRUM_MTOM;
							break;
						}
						if((mesg[i].btn_mesg.buttons&CWIID_BTN_PLUS) == CWIID_BTN_PLUS)
						{
							buttons = DRUM_HTOM;
							break;
						}
						if((mesg[i].btn_mesg.buttons&CWIID_BTN_DOWN) == CWIID_BTN_DOWN)
						{
							buttons = DRUM_RIDE;
							break;
						}
						if((mesg[i].btn_mesg.buttons&CWIID_BTN_LEFT) == CWIID_BTN_LEFT)
						{
							buttons = DRUM_CHCRASH;
							break;
						}
						if((mesg[i].btn_mesg.buttons&CWIID_BTN_RIGHT) == CWIID_BTN_RIGHT)
						{
							buttons = DRUM_SPLASH;
							break;
						}
						if((mesg[i].btn_mesg.buttons&CWIID_BTN_1) == CWIID_BTN_1)
						{
							buttons = DRUM_SNAREST;
							break;
						}
					}
				}
				break;
				case CWIID_MESG_ACC:
				{
					struct timeb now_time;
					ftime(&now_time);
					unsigned long n_msecs = ((now_time.time-start_time) * 1000 + now_time.millitm);
					unsigned long c_msecs = ((cur_time.time-start_time) * 1000 + cur_time.millitm);
					double a_x, a_y, a_z;
					
					a_x = ((double)mesg[i].acc_mesg.acc[CWIID_X] - cal[1].zero[CWIID_X]) /	(cal[1].one[CWIID_X] - cal[1].zero[CWIID_X]);
					a_y = ((double)mesg[i].acc_mesg.acc[CWIID_Y] - cal[1].zero[CWIID_Y]) /	(cal[1].one[CWIID_Y] - cal[1].zero[CWIID_Y]);
					a_z = ((double)mesg[i].acc_mesg.acc[CWIID_Z] - cal[1].zero[CWIID_Z]) /	(cal[1].one[CWIID_Z] - cal[1].zero[CWIID_Z]);
					
					if(a_z>gtk_range_get_value(GTK_RANGE(dwn[1])) && down[0] == 0)
					{
						down[0] = 1;
						alSourcePlay(Source[buttons]);
					}
					else
					{
						//if(n_msecs < c_msecs + 100)
						//	break;
						if(a_z<gtk_range_get_value(GTK_RANGE((up[1]))))
							down[0] = 0;
					}
				}
				break;
				case CWIID_MESG_NUNCHUK:
				{
					double a_x, a_y, a_z;
					a_x = ((double)mesg[i].nunchuk_mesg.acc[CWIID_X] - nc_cal[1].zero[CWIID_X]) /	(nc_cal[1].one[CWIID_X] - nc_cal[1].zero[CWIID_X]);
					a_y = ((double)mesg[i].nunchuk_mesg.acc[CWIID_Y] - nc_cal[1].zero[CWIID_Y]) /	(nc_cal[1].one[CWIID_Y] - nc_cal[1].zero[CWIID_Y]);
					a_z = ((double)mesg[i].nunchuk_mesg.acc[CWIID_Z] - nc_cal[1].zero[CWIID_Z]) /	(nc_cal[1].one[CWIID_Z] - nc_cal[1].zero[CWIID_Z]);
					//printf("X: %.2f\n Y: %.2f Z: %.2f\n",a_x,a_y,a_z);
					if(a_z<-0.7 && down[1] == 0)
					{
						down[1] = 1;
						//if(a_x<-0.5)
							//alSourcePlay(Source[DRUM_SNARE]);
						//if(a_x>-0.5 && a_x<0.5)
							alSourcePlay(Source[DRUM_BDRUM]);
						//if(a_x>0.5)
							//alSourcePlay(Source[DRUM_CRASH]);
					}
					else
					{
						if(a_z>-0.55)
							down[1] = 0;
					}
				}
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
		cwiid_close(wii_remote[0]);
		cwiid_close(wii_remote[1]);
		wii_remote[0] == NULL;
		wii_remote[1] == NULL;
		cwiid_set_mesg_callback(wii_remote[0],NULL);
		cwiid_set_mesg_callback(wii_remote[1],NULL);
		gtk_button_set_label(GTK_BUTTON(widget),"Connect to Wiimote");
		return;
	}
	bdaddr_t baddr;
	int ret = 0;
	
	ret = cwiid_find_wiimote(&baddr, 5);
	if(ret==0)
	{
		wii_remote[0] = cwiid_open(&baddr, CWIID_FLAG_MESG_IFC);
		cwiid_set_led(wii_remote[0], CWIID_LED1_ON);
		cwiid_set_rpt_mode(wii_remote[0], CWIID_RPT_ACC|CWIID_RPT_NUNCHUK|CWIID_RPT_BTN);
		cwiid_get_acc_cal(wii_remote[0], CWIID_EXT_NONE, &cal[0]);
		cwiid_get_acc_cal(wii_remote[0], CWIID_EXT_NUNCHUK, &nc_cal[0]);
		printf("Found one\n");
		timer = 1;
	}
	else
	{
		printf("Failed to find wiimote\n");
		return;
	}
	ret = cwiid_find_wiimote(&baddr, 5);
	if(ret==0)
	{
		printf("Found two\n");
		wii_remote[1] = cwiid_open(&baddr, CWIID_FLAG_MESG_IFC);
		cwiid_set_led(wii_remote[1], CWIID_LED2_ON);
		cwiid_set_rpt_mode(wii_remote[1], CWIID_RPT_ACC|CWIID_RPT_NUNCHUK|CWIID_RPT_BTN);
		cwiid_get_acc_cal(wii_remote[1], CWIID_EXT_NONE, &cal[1]);
		cwiid_get_acc_cal(wii_remote[1], CWIID_EXT_NUNCHUK, &nc_cal[1]);
		gtk_button_set_label(GTK_BUTTON(widget),"Disconnect Wiimote");
		cwiid_set_mesg_callback(wii_remote[0],&cwiid_callback);
		cwiid_set_mesg_callback(wii_remote[1],&cwiid_callback2);
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
	FILE *file_h = fopen(file,"rb");
	if(file_h == NULL)
	{
		fprintf(stderr,"failed to open %s for sound\n",file);
		return;
	}
   if(ov_open(file_h,&vf,0,0) < 0)
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
	*size = ov_pcm_total(&vf,-1)*4; // 16 bit mono, 
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
	printf("Loaded Ogg! %s\n",file);
}

ALboolean LoadALData()
{
    // Variables to load into.

    ALenum format;
    ALsizei size;
    ALvoid* data;
    ALsizei freq;
    ALboolean loop;
    // Load wav data into a buffer.

	char *files[] = 
	{ 
		"sounds/snarel.ogg",
		"sounds/snarer.ogg",
		"sounds/hihat.ogg",
		"sounds/hihatc.ogg",
		"sounds/hihatp.ogg",
		"sounds/bdrum.ogg",
		"sounds/crash.ogg",
		"sounds/chcrash.ogg",
		"sounds/splash.ogg",
		"sounds/ltom.ogg",
		"sounds/mtom.ogg",
		"sounds/htom.ogg",
		"sounds/ride.ogg",
		"sounds/stsnare.ogg"
	};
	int i = 0;
	alGenSources(14, Source);
	alGenBuffers(14, Buffer);
		if (alGetError() != AL_NO_ERROR)
		  return AL_FALSE;
	for(i=0;i<14;i++)
	{
		openOggVorbis(files[i], &format, &data, &size, &freq, &loop);
		alBufferData(Buffer[i], format, data, size, freq);
		free(data);
		// Bind buffer with a source.
		

		if (alGetError() != AL_NO_ERROR)
		  return AL_FALSE;

		alSourcei (Source[i], AL_BUFFER,   Buffer[i]   );
		alSourcef (Source[i], AL_PITCH,    1.0f     );
		alSourcef (Source[i], AL_GAIN,     1.0f     );
		alSourcefv(Source[i], AL_POSITION, SourcePos);
		alSourcefv(Source[i], AL_VELOCITY, SourceVel);
		alSourcei (Source[i], AL_LOOPING,  AL_FALSE);
	}
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
    alDeleteBuffers(3, Buffer);
    alDeleteSources(3, Source);
    alutExit();
	 if(wii_remote[0]!=NULL)
		cwiid_close(wii_remote[0]);
	if(wii_remote[1]!=NULL)
		cwiid_close(wii_remote[1]);
	 gtk_exit(0);
	 return FALSE;
}

int main(int argc, char *argv[])
{
    // Initialize OpenAL and clear the error bit.
	gtk_init(&argc, &argv);
	GtkWidget *window, *hbox, *connect, *table;
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	hbox = gtk_hbox_new(FALSE,0);
	connect = gtk_button_new_with_label("Connect to Wiimote");
	ypos[0] = gtk_entry_new();
	ypos[1] = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(ypos[0]),FALSE);
	gtk_entry_set_editable(GTK_ENTRY(ypos[1]),FALSE);
	
	dwn[0] = gtk_hscale_new_with_range(-1.0,1.0,0.1);
	gtk_range_set_value(GTK_RANGE(dwn[0]),0.1);
	dwn[1] = gtk_hscale_new_with_range(-1.0,1.0,0.1);
	gtk_range_set_value(GTK_RANGE(dwn[1]),0.1);
	up[0] = gtk_hscale_new_with_range(-1.0,1.0,0.1);
	gtk_range_set_value(GTK_RANGE(up[0]),-0.4);
	up[1] = gtk_hscale_new_with_range(-1.0,1.0,0.1);
	gtk_range_set_value(GTK_RANGE(up[1]),-0.4);
	
	table = gtk_table_new(3,2,0);
	
	gtk_table_attach_defaults(GTK_TABLE(table),ypos[0],0,1,0,1);
	gtk_table_attach_defaults(GTK_TABLE(table),ypos[1],1,2,0,1);
	gtk_table_attach_defaults(GTK_TABLE(table),dwn[0],0,1,1,2);
	gtk_table_attach_defaults(GTK_TABLE(table),dwn[1],1,2,1,2);
	gtk_table_attach_defaults(GTK_TABLE(table),up[0],0,1,2,3);
	gtk_table_attach_defaults(GTK_TABLE(table),up[1],1,2,2,3);
	
	gtk_box_pack_start(GTK_BOX(hbox),connect,TRUE,TRUE,0);	
	gtk_box_pack_start(GTK_BOX(hbox),table,TRUE,TRUE,0);
	gtk_container_add(GTK_CONTAINER(window),hbox);
	
	alutInit(&argc, argv);
	alGetError();
	// Load the wav data.
	if (LoadALData() == AL_FALSE)
		return -1;

	SetListenerValues();
	start_time = time(NULL);	
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(window_close),NULL);
	g_signal_connect(G_OBJECT(connect),"clicked",G_CALLBACK(find_wiimote),NULL);
	gtk_widget_show_all(window);
	gtk_main();		
	
	return 0;
}
