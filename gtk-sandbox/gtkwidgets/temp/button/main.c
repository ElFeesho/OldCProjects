#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "pmbutton.h"
static void on_click(GtkWidget *pmbutton, gpointer user_data)
{
	printf("It's been clicked!\n");	
}

static void on_hover(GtkWidget *button, gpointer text)
{
	printf("Text: %s\n",text);
}

int main( int   argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *button[5];
    GtkWidget *vbox;
  
  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  
  gtk_window_set_title(GTK_WINDOW (window), "Button Test");
  
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK (exit), NULL);
  
  vbox = gtk_vbox_new(1,0);
  
  button[0] = gtk_pmbutton_new("Test");
  g_signal_connect(G_OBJECT(button[0]),"clicked",G_CALLBACK(on_click),NULL);
  button[1] = gtk_pmbutton_new("Case");
  button[2] = gtk_pmbutton_new("Custo");
  button[3] = gtk_pmbutton_new("Custom");
  button[4] = gtk_pmbutton_new("Several");
	int i;
	for(i=0;i<=0;i++)
	{  
		gtk_box_pack_start(GTK_BOX(vbox),button[i],1,1,0);
	}
	  
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show_all(vbox);
  gtk_widget_show (window);
  
  gtk_main ();
  
  return 0;
}
