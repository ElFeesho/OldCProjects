#include <gtk/gtk.h>

GSource *idle_func(gpointer data)
{
	FILE *tempFile = fopen("/proc/acpi/thermal_zone/THM0/temperature","r");
	if(!tempFile)
	{
		gtk_label_set_text(GTK_LABEL(data),"ERROR!");
		return 1;
	}
	int temp = 0;
	fscanf(tempFile,"temperature: %d C",&temp);
	char temp_str[6] = {'\0'};
	sprintf(temp_str,"%d C",temp);
	
	gtk_label_set_text(GTK_LABEL(data),temp_str);
	fclose(tempFile);	
	return 1;
}

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_set_application_name("TempRead");
	GtkWidget *label = gtk_label_new("Updating...");
	gtk_container_add(GTK_CONTAINER(window),label);
	gtk_widget_show_all(window);
	g_signal_connect(G_OBJECT(window),"delete-event",G_CALLBACK(gtk_exit),0);
	g_timeout_add(1000,idle_func,label);
	gtk_main();
}
