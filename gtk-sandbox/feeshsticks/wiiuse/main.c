#include <gtk/gtk.h>
#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <wiiuse.h>
#include <SDL/SDL_mixer.h>

GtkWidget *ypos[2], *dwn[2], *up[2], *ggraph;
long lhit[2];
int found = 0;
int exitting = 0;

int started = 0;

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

struct wiimote_t **wiimotes = NULL;

Mix_Chunk *drum_sounds[14];

gboolean ggraph_expose(GtkWidget *draw_wind, GdkEventExpose *ev, gpointer userdata)
{
	static lx = 0;
	static ly = 0;
	if(ly == 0 && started == 0)
	{
		ly = draw_wind->allocation.height/2;
	}
	if(started)
	{
		GdkGC *gc = gdk_gc_new(draw_wind->window);
		gdk_draw_line(draw_wind->window, gc, lx, ly, lx+1, (wiimotes[0]->gforce.z-1)*(draw_wind->allocation.height/5));
		ly = (wiimotes[0]->gforce.z-1)*(draw_wind->allocation.height/5);
		lx++;
		if(lx>draw_wind->allocation.width)
		{
			lx = 0;
			GdkColor white_col;
			gdk_color_parse("white",&white_col);
			gdk_gc_set_rgb_fg_color(gc, &white_col);
			gdk_draw_rectangle(draw_wind->window, gc, TRUE, 0,0,draw_wind->allocation.width, draw_wind->allocation.height);
		}
		g_object_unref(gc);
	}
}

gboolean handle_hit(gpointer *data)
{
	static int down[2] = { 0 };
	started = 1;
	for(;!exitting;)
	{
		if(wiiuse_poll(wiimotes,found))
		{
			if(wiimotes[0]->event == WIIUSE_EVENT)
			{
				if(wiimotes[0]->gforce.z<gtk_range_get_value(GTK_RANGE(dwn[0])) && !down[0])
				{
					lhit[0] = SDL_GetTicks();
					down[0] = 1;
					if(IS_HELD(wiimotes[0], WIIMOTE_BUTTON_A))
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_CRASH],0);
					}
					else if(IS_HELD(wiimotes[0], WIIMOTE_BUTTON_B))
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_HIHAT],0);
					}
					else if(IS_HELD(wiimotes[0], WIIMOTE_BUTTON_MINUS))
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_HTOM],0);
					}
					else if(IS_HELD(wiimotes[0], WIIMOTE_BUTTON_HOME))
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_MTOM],0);
					}
					else if(IS_HELD(wiimotes[0], WIIMOTE_BUTTON_PLUS))
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_LTOM],0);
					}
					else if(IS_HELD(wiimotes[0], WIIMOTE_BUTTON_DOWN))
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_RIDE],0);
					}
					else if(IS_HELD(wiimotes[0], WIIMOTE_BUTTON_LEFT))
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_CHCRASH],0);
					}
					else if(IS_HELD(wiimotes[0], WIIMOTE_BUTTON_RIGHT))
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_SPLASH],0);
					}
					else if(IS_HELD(wiimotes[0], WIIMOTE_BUTTON_ONE) || IS_HELD(wiimotes[0], WIIMOTE_BUTTON_TWO))
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_SNAREST],0);
					}
					else
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_SNAREL],0);
					}
				}
				if(wiimotes[0]->gforce.z>gtk_range_get_value(GTK_RANGE(up[0])) && down[0])
				{
					down[0] = 0;
				}
			}
			if(wiimotes[1]->event == WIIUSE_EVENT)
			{
				if(wiimotes[1]->gforce.z<gtk_range_get_value(GTK_RANGE(dwn[1])) && !down[1])
				{
					lhit[1] = SDL_GetTicks();
					down[1] = 1;
					if(IS_HELD(wiimotes[1], WIIMOTE_BUTTON_A))
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_CRASH],0);
					}
					else if(IS_HELD(wiimotes[1], WIIMOTE_BUTTON_B))
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_HIHAT],0);
					}
					else if(IS_HELD(wiimotes[1], WIIMOTE_BUTTON_MINUS))
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_HTOM],0);
					}
					else if(IS_HELD(wiimotes[1], WIIMOTE_BUTTON_HOME))
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_MTOM],0);
					}
					else if(IS_HELD(wiimotes[1], WIIMOTE_BUTTON_PLUS))
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_LTOM],0);
					}
					else if(IS_HELD(wiimotes[1], WIIMOTE_BUTTON_DOWN))
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_RIDE],0);
					}
					else if(IS_HELD(wiimotes[1], WIIMOTE_BUTTON_LEFT))
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_CHCRASH],0);
					}
					else if(IS_HELD(wiimotes[1], WIIMOTE_BUTTON_RIGHT))
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_SPLASH],0);
					}
					else if(IS_HELD(wiimotes[1], WIIMOTE_BUTTON_ONE) || IS_HELD(wiimotes[0], WIIMOTE_BUTTON_TWO))
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_SNAREST],0);
					}
					else
					{
						Mix_PlayChannel(-1,drum_sounds[DRUM_SNARER],0);
					}
				}
				if(wiimotes[1]->gforce.z>gtk_range_get_value(GTK_RANGE(up[1])) && down[1])
				{
					down[1] = 0;
				}
			}
		}		
		gtk_widget_queue_draw(ggraph);
	}
}

void find_wiimote(GtkWidget *widget, gpointer user_data)
{
	wiimotes = wiiuse_init(2);
	found = wiiuse_find(wiimotes, 2, 5); /* Find remotes (5s timeout) */
	if(found>0)
	{
		wiiuse_connect(wiimotes, found); /* Connect to as many wiimotes that were found */
	}
	else
	{
		printf("No wiimotes found\n");
		return;
	}
	printf("Found %d wiimote%s\n",found, (found!=1)?("s"):(""));
	
	wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_1|WIIMOTE_LED_4);
	wiiuse_motion_sensing(wiimotes[0], 1);
	if(found>1)
	{
		wiiuse_set_leds(wiimotes[1], WIIMOTE_LED_2|WIIMOTE_LED_3);
		wiiuse_motion_sensing(wiimotes[1], 1);
	}
	g_thread_create((GThreadFunc)handle_hit, NULL, FALSE, NULL);
}

void load_sounds()
{
	drum_sounds[0] = Mix_LoadWAV("sounds/snarel.ogg");
	drum_sounds[1] = Mix_LoadWAV("sounds/snarer.ogg");
	drum_sounds[2] = Mix_LoadWAV("sounds/hihat.ogg");
	drum_sounds[3] = Mix_LoadWAV("sounds/hihatc.ogg");
	drum_sounds[4] = Mix_LoadWAV("sounds/hihatp.ogg");
	drum_sounds[5] = Mix_LoadWAV("sounds/bdrum.ogg");
	drum_sounds[6] = Mix_LoadWAV("sounds/crash.ogg");
	drum_sounds[7] = Mix_LoadWAV("sounds/chcrash.ogg");
	drum_sounds[8] = Mix_LoadWAV("sounds/splash.ogg");
	drum_sounds[9] = Mix_LoadWAV("sounds/ltom.ogg");
	drum_sounds[10] = Mix_LoadWAV("sounds/mtom.ogg");
	drum_sounds[11] = Mix_LoadWAV("sounds/htom.ogg");
	drum_sounds[12] = Mix_LoadWAV("sounds/ride.ogg");
	drum_sounds[13] = Mix_LoadWAV("sounds/stsnare.ogg");
}

static gboolean window_close(GtkWidget *window, gpointer udata)
{
	exitting = 1;
	gtk_widget_destroy(window);
	gtk_main_quit();
	return FALSE;
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_AUDIO);
	if(Mix_OpenAudio(22050, AUDIO_S16, 2, 4096))
	{
		fprintf(stderr,"Unable to open audio\n");
		return 0;
	}
	load_sounds();
	g_thread_init(NULL);
	gtk_init(&argc, &argv);
	GtkWidget *window, *hbox, *connect, *table, *vbox;
	ggraph = gtk_drawing_area_new();
	vbox = gtk_vbox_new(FALSE,0);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	hbox = gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),ggraph,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,TRUE,0);
	connect = gtk_button_new_with_label("Connect to Wiimote");
	ypos[0] = gtk_entry_new();
	ypos[1] = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(ypos[0]),FALSE);
	gtk_entry_set_editable(GTK_ENTRY(ypos[1]),FALSE);
	
	dwn[0] = gtk_hscale_new_with_range(-3.0,3.0,0.1);
	gtk_range_set_value(GTK_RANGE(dwn[0]),0.1);
	dwn[1] = gtk_hscale_new_with_range(-3.0,3.0,0.1);
	gtk_range_set_value(GTK_RANGE(dwn[1]),0.1);
	up[0] = gtk_hscale_new_with_range(-3.0,3.0,0.1);
	gtk_range_set_value(GTK_RANGE(up[0]),-0.4);
	up[1] = gtk_hscale_new_with_range(-3.0,3.0,0.1);
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
	gtk_container_add(GTK_CONTAINER(window),vbox);
	
	gtk_widget_set_size_request(ggraph, 500,300);

	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(window_close),NULL);
	g_signal_connect(G_OBJECT(connect),"clicked",G_CALLBACK(find_wiimote),NULL);
	gtk_widget_show_all(window);
	gtk_main();		
	Mix_CloseAudio();
	SDL_Quit();	
	return 0;
}
