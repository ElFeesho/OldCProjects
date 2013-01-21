#include <gtk/gtk.h>

GtkWidget *menu_create()
{
	GtkWidget *menu, *menu_item, *file_menu, *edit_menu, *help_menu = NULL;

	menu = gtk_menu_bar_new();


	/* BEGIN FILE MENU */
	file_menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_mnemonic("_New");
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu),menu_item);
	menu_item = gtk_menu_item_new_with_mnemonic("_Open");
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu),menu_item);
	menu_item = gtk_menu_item_new_with_mnemonic("_Save");
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu),menu_item);
	menu_item = gtk_menu_item_new_with_mnemonic("Save _As");
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu),menu_item);
	menu_item = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu),menu_item);
	menu_item = gtk_menu_item_new_with_mnemonic("E_xit");
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu),menu_item);
	/* END FILE MENU */
	
	/* BEGIN EDIT MENU */
	edit_menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_mnemonic("_Cup");
	gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu),menu_item);
	menu_item = gtk_menu_item_new_with_mnemonic("C_opy");
	gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu),menu_item);
	menu_item = gtk_menu_item_new_with_mnemonic("_Paste");
	gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu),menu_item);
	/* END EDIT MENU */
	
	/* BEGIN HELP MENU */
	help_menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_mnemonic("_Help");
	gtk_menu_shell_append(GTK_MENU_SHELL(help_menu),menu_item);
	menu_item = gtk_menu_item_new_with_mnemonic("_About");
	gtk_menu_shell_append(GTK_MENU_SHELL(help_menu),menu_item);
	/* END HELP MENU */
	
	menu_item = gtk_menu_item_new_with_mnemonic("_File");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item),file_menu);
	
	menu_item = gtk_menu_item_new_with_mnemonic("_Edit");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item),edit_menu);
	
	menu_item = gtk_menu_item_new_with_mnemonic("_Help");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item),help_menu);
	
	return menu;
}
