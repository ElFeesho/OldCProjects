#include <gtk/gtk.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <linux/videodev.h>

int wc_handle;
guint update_handle;
void *wc_data;

GTimer *fps_timer = NULL;

struct video_mmap vid_map;
struct video_mbuf vid_buf;
struct video_picture vid_pinfo;

void simple_switch(char *data)
{
	if(!data)
		return;
	int i = 0;
	for(i = 0;i<(vid_map.width*vid_map.height*3)-3;i+=3)
	{
		char tmp = data[i];
		data[i] = data[i+2];
		data[i+2] = tmp;
	}
}

static gboolean update_cam(GtkWidget *webcam)
{
	static double ltime = 0;
	static int frame = 0;
	if(ioctl(wc_handle,VIDIOCMCAPTURE,&vid_map)==-1)
	{
		fprintf(stderr,"Dropped a frame!\n");
		return TRUE;
	}
	while(ioctl(wc_handle,VIDIOCSYNC,&frame)<0 && errno==EINTR);
	simple_switch(wc_data+vid_buf.offsets[frame]);
	gdk_draw_rgb_image(webcam->window,gdk_gc_new(webcam->window),0,0,vid_map.width,vid_map.height,GDK_RGB_DITHER_NORMAL,wc_data+vid_buf.offsets[frame],vid_map.width*3);
	frame=1-frame;
	vid_map.frame = 1-vid_map.frame;
	if(ltime==0)
	{
		ltime = g_timer_elapsed(fps_timer,NULL);
	}
	else
	{
		double ctime = g_timer_elapsed(fps_timer,NULL);
		printf("Last update: %.2f FPS: %.2f\n",ctime-ltime, 1.0/(ctime-ltime));
		ltime = ctime;
	}   	
	return TRUE;
}

static void pause_clicked(GtkWidget *pause, gpointer user_data)
{
	static int running = 1;
	if(running)
	{
		g_source_remove(update_handle);
		gtk_button_set_label(GTK_BUTTON(pause),"Play");
		running = 0;
	}
	else
	{
		update_handle = g_idle_add((GSourceFunc)update_cam,user_data);
		gtk_button_set_label(GTK_BUTTON(pause),"Pause");
		running = 1;
	}
	
}

static gboolean window_delete(GtkWidget *window, gpointer user_data)
{
	close(wc_handle);
	gtk_exit(0);
	return TRUE;
}
static gboolean contrast_scroll(GtkRange *range, GtkScrollType scroll, gdouble value, gpointer user_data) 
{
	vid_pinfo.contrast = (int)gtk_range_get_value(range);
	if(ioctl(wc_handle,VIDIOCSPICT,&vid_pinfo)<0)
	{
		fprintf(stderr,"Couldn't set picture mode!\n");
		return 1;
	}
}

static gboolean brightness_scroll(GtkRange *range, GtkScrollType scroll, gdouble value, gpointer user_data) 
{
	vid_pinfo.brightness = (int)gtk_range_get_value(range);
	if(ioctl(wc_handle,VIDIOCSPICT,&vid_pinfo)<0)
	{
		fprintf(stderr,"Couldn't set picture mode!\n");
		return 1;
	}
}

int main(int argc, char **argv)
{
	g_set_application_name("GTKLwebcam");
	gtk_init(&argc, &argv);
	
	wc_handle = open("/dev/video0",O_RDWR);
	if(wc_handle<0)
	{
		fprintf(stderr, "Failed to open webcam!\n");
		return 1;
	}
	
	GtkWidget *window, *webcam, *pause, *table, *contrast, *brightness;
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	webcam = gtk_drawing_area_new();
	
	pause = gtk_button_new_with_label("Pause");
	
	contrast = gtk_hscale_new_with_range(0,65535,1);
	brightness = gtk_hscale_new_with_range(0,65535,1);
	
	table = gtk_table_new(5,2,0);
	gtk_table_attach(GTK_TABLE(table),gtk_label_new("Believe me, it's part of something huge"),0,2,0,1,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(table),webcam,0,1,1,4,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(table),pause,0,1,4,5,GTK_FILL,0,0,0);
	gtk_table_attach(GTK_TABLE(table),gtk_label_new("Contrast"),1,2,1,2,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(table),contrast,1,2,2,3,GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(table),gtk_label_new("Brightness"),1,2,3,4,GTK_FILL,GTK_FILL,0,0);	
	gtk_table_attach(GTK_TABLE(table),brightness,1,2,4,5,GTK_FILL,GTK_FILL,0,0);
	struct video_capability vid_caps;
	if(ioctl(wc_handle,VIDIOCGCAP,&vid_caps)==-1)
	{
		fprintf(stderr,"Failed to get the webcamera's capabiltiies\n");
		return 1;
	}
	
	struct video_window vid_cwin;
	if(ioctl(wc_handle,VIDIOCGWIN,&vid_cwin)<0)
	{
		fprintf(stderr,"Failed to read video window capture jive\n");
		return 1;
	}

	vid_cwin.width = 640;
	vid_cwin.height = 480;

	if(ioctl(wc_handle,VIDIOCSWIN,&vid_cwin)<0)
	{
		fprintf(stderr,"Failed to read video window capture jive\n");
		return 1;
	}
		


	if(ioctl(wc_handle,VIDIOCGPICT,&vid_pinfo)<0)
	{
		fprintf(stderr,"Failed to get picture mode\n");
	}
	vid_pinfo.contrast = 10485;
	vid_pinfo.brightness = 51772;
	vid_pinfo.palette = VIDEO_PALETTE_RGB24;
	gtk_range_set_value(GTK_RANGE(contrast),10485.0);
	gtk_range_set_value(GTK_RANGE(brightness),51772.0);
	
	if(ioctl(wc_handle,VIDIOCSPICT,&vid_pinfo)<0)
	{
		fprintf(stderr,"Couldn't set picture mode!\n");
		return 1;
	}

	if(ioctl(wc_handle, VIDIOCGMBUF,&vid_buf)<0)
	{
		
		fprintf(stderr,"Failed to get mbuf\n");
		return 1;
	}
	
	wc_data = mmap(0, vid_buf.size, PROT_READ | PROT_WRITE, MAP_SHARED, wc_handle,0);
	if((unsigned char*)wc_data == (unsigned char*)-1)
		return 1;
		
	vid_map.width = vid_cwin.width;
	vid_map.height = vid_cwin.height;
	vid_map.format = VIDEO_PALETTE_RGB24;
	vid_map.frame = 0;
	
	if(ioctl(wc_handle,VIDIOCMCAPTURE,&vid_map)==-1)
	{
		fprintf(stderr,"Failed to get first capture\n");
		return 1;
	}
	vid_map.frame = 1;
	gtk_widget_set_size_request(webcam, vid_cwin.width, vid_cwin.height);
	
	gtk_container_add(GTK_CONTAINER(window),table);
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(window_delete),NULL);
	g_signal_connect(G_OBJECT(pause),"clicked",G_CALLBACK(pause_clicked),webcam);
	g_signal_connect(G_OBJECT(contrast),"value-changed",G_CALLBACK(contrast_scroll),NULL);
	g_signal_connect(G_OBJECT(brightness),"value-changed",G_CALLBACK(brightness_scroll),NULL);
	
	update_handle = g_idle_add((GSourceFunc)update_cam,webcam);
	
	gtk_widget_show_all(window);
	fps_timer = g_timer_new();
	g_timer_start(fps_timer);
	gtk_main();	
	return 0;
}
