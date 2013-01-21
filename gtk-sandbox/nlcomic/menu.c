#include <gtk/gtk.h>


GtkWidget *menu_create()
{
	GtkWidget *menu = 0, *file = 0, *options = 0, *help = 0;
	
	menu = gtk_menu_bar_new();

	file = gtk_menu_item_new_with_mnemonic("_File");
	options = gtk_menu_item_new_with_mnemonic("_Options");
	help = gtk_menu_item_new_with_mnemonic("_Help");

	gtk_menu_shell_append(GTK_MENU_SHELL(menu),file);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),options);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),help);
	return menu;
}
