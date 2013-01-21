#include <gtk/gtk.h>

static GtkWidget *num = NULL;

void convert(GtkWidget *button, GtkWidget *output)
{
	int actual_num = 0;

	char *text = (char*)gtk_entry_get_text(GTK_ENTRY(num));
	sscanf(text, "%d", &actual_num);
	char *output_txt = g_malloc0(33);

	int i = 31;
	gboolean foundone = FALSE;
	int pos = 0;
	for(;i>=0;i--)
	{
		if((actual_num&(1<<i))==(1<<i))
		{
			output_txt[pos++] = '1';
			foundone = TRUE;
		}
		else
		{
			if(foundone)
			{
				output_txt[pos++] = '0';
			}
		}
	}
	gtk_entry_set_text(GTK_ENTRY(output), output_txt);
	g_free(output_txt);
}

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);

	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *output = gtk_entry_new();
	GtkWidget *con_btn = gtk_button_new_with_label("Convert");

	num = gtk_entry_new();

	GtkWidget *table = gtk_table_new(2,3,0);
	gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Enter a Number:"),0,1,0,1);
	gtk_table_attach_defaults(GTK_TABLE(table), gtk_label_new("Binary Output:"),0,1,1,2);
	gtk_table_attach_defaults(GTK_TABLE(table), num,1,2,0,1);
	gtk_table_attach_defaults(GTK_TABLE(table), output,1,2,1,2);
	gtk_table_attach_defaults(GTK_TABLE(table), con_btn,1,2,2,3);

	gtk_container_add(GTK_CONTAINER(window), table);

	g_signal_connect(G_OBJECT(window),"delete-event", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(con_btn),"clicked", G_CALLBACK(convert), output);

	gtk_widget_show_all(window);

	gtk_main();
	return 0;
}
