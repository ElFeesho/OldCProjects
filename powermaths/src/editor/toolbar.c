#include <gtk/gtk.h>

GtkWidget *toolbar_create()
{
	GtkWidget *toolbar = gtk_toolbar_new();
	
	GtkToolItem *toolbar_item = NULL;
	
	GtkWidget *new = NULL, *open = NULL, *save = NULL, *saveas = NULL;
	
	new = gtk_button_new_from_stock(GTK_STOCK_NEW);
	open = gtk_button_new_from_stock(GTK_STOCK_OPEN);
	save = gtk_button_new_from_stock(GTK_STOCK_SAVE);
	saveas = gtk_button_new_from_stock(GTK_STOCK_SAVE_AS);
	
	toolbar_item = gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);
	
	toolbar_item = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);
	
	toolbar_item = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);
	
	toolbar_item = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE_AS);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);
	
	toolbar_item = gtk_tool_button_new_from_stock(GTK_STOCK_ADD);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar),GTK_TOOL_ITEM(toolbar_item),-1);
	
	
	
	return toolbar;
}
