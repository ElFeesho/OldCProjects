/*
	Copyright Christopher "Feesh" Sawczuk 2006 (c)
	
	This file is part of Nonlogicpad.

	Nonlogicpad is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Nonlogic Pad is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Nonlogic Pad; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <gtk/gtk.h>

#include "main.h"
#include "globals.h"
#include "gui.h"
#include "chanDialog.h"

void makeConnectDialog()
{
	subGUI.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_resizable(GTK_WINDOW(subGUI.window),0);
	gtk_window_set_type_hint(GTK_WINDOW(subGUI.window),GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_title(GTK_WINDOW(subGUI.window),"Connect to a Nonlogic Pad Server");
	subGUI.table = gtk_table_new(6,2,1);
	subGUI.banner = gtk_image_new_from_file("pixbufs/banner.png");
	gtk_table_set_row_spacings(GTK_TABLE(subGUI.table),4);
	subGUI.server = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(subGUI.server),globals.server);
	subGUI.port = gtk_entry_new();
	char ptext[6];
	sprintf(ptext,"%d",globals.port);
	gtk_entry_set_text(GTK_ENTRY(subGUI.port),ptext);
	subGUI.connect = gtk_button_new_with_label("Connect");
	subGUI.cancel = gtk_button_new_with_label("Cancel");

	gtk_container_set_border_width(GTK_CONTAINER(subGUI.window),3);
	gtk_table_attach_defaults(GTK_TABLE(subGUI.table),subGUI.banner,0,2,0,2);
	gtk_table_attach_defaults(GTK_TABLE(subGUI.table),gtk_label_new("Server:"),0,1,2,3);
	gtk_table_attach_defaults(GTK_TABLE(subGUI.table),gtk_label_new("Port:"),0,1,3,4);
	gtk_table_attach_defaults(GTK_TABLE(subGUI.table),subGUI.server,1,2,2,3);
	gtk_table_attach_defaults(GTK_TABLE(subGUI.table),subGUI.port,1,2,3,4);
	gtk_table_attach_defaults(GTK_TABLE(subGUI.table),subGUI.connect,0,1,4,5);
	gtk_table_attach_defaults(GTK_TABLE(subGUI.table),subGUI.cancel,1,2,4,5);
	gtk_container_add(GTK_CONTAINER(subGUI.window),subGUI.table);

	g_signal_connect(G_OBJECT(subGUI.window),"delete-event",G_CALLBACK(dlgWindowDeleteEvent),NULL);
	g_signal_connect(G_OBJECT(subGUI.connect),"clicked",G_CALLBACK(connectSubClicked),NULL);
	g_signal_connect_swapped(G_OBJECT(subGUI.cancel),"clicked",G_CALLBACK(gtk_widget_hide),subGUI.window);
}

void makeNickDialog()
{
	nickDIAG.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_type_hint(GTK_WINDOW(nickDIAG.window),GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_resizable(GTK_WINDOW(nickDIAG.window),0);
	gtk_window_set_title(GTK_WINDOW(nickDIAG.window),"Change your ID");
	nickDIAG.table = gtk_table_new(3,2,0);
	nickDIAG.banner = gtk_image_new_from_file("pixbufs/banner.png");
	gtk_table_set_row_spacings(GTK_TABLE(nickDIAG.table),4);
	nickDIAG.nick = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(nickDIAG.nick),globals.nick);
	gtk_entry_set_text(GTK_ENTRY(nickDIAG.nick),globals.nick);
	gtk_entry_set_max_length(GTK_ENTRY(nickDIAG.nick),16);
	nickDIAG.ok = gtk_button_new_with_label("Ok");
	nickDIAG.cancel = gtk_button_new_with_label("Cancel");
	gtk_container_set_border_width(GTK_CONTAINER(nickDIAG.window),3);
	gtk_table_attach_defaults(GTK_TABLE(nickDIAG.table),nickDIAG.banner,0,2,0,1);
	gtk_table_attach_defaults(GTK_TABLE(nickDIAG.table),gtk_label_new("ID:"),0,1,1,2);
	gtk_table_attach_defaults(GTK_TABLE(nickDIAG.table),nickDIAG.nick,1,2,1,2);
	gtk_table_attach_defaults(GTK_TABLE(nickDIAG.table),nickDIAG.ok,0,1,2,3);
	gtk_table_attach_defaults(GTK_TABLE(nickDIAG.table),nickDIAG.cancel,1,2,2,3);
	gtk_container_add(GTK_CONTAINER(nickDIAG.window),nickDIAG.table);

	g_signal_connect(G_OBJECT(nickDIAG.ok),"clicked",G_CALLBACK(nickOkClicked),NULL);
	g_signal_connect(G_OBJECT(nickDIAG.window),"delete-event",G_CALLBACK(dlgWindowDeleteEvent),NULL);
	g_signal_connect_swapped(G_OBJECT(nickDIAG.cancel),"clicked",G_CALLBACK(gtk_widget_hide),nickDIAG.window);
}

void makeGridDialog()
{
	gridDIAG.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_type_hint(GTK_WINDOW(gridDIAG.window),GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_resizable(GTK_WINDOW(gridDIAG.window),0);
	gtk_window_set_title(GTK_WINDOW(gridDIAG.window),"Grid Properties");
	gridDIAG.snap = gtk_check_button_new_with_label("Snap to Grid");
	gridDIAG.table = gtk_table_new(5,2,0);
	gridDIAG.banner = gtk_image_new_from_file("pixbufs/banner.png");
	gtk_table_set_row_spacings(GTK_TABLE(gridDIAG.table),4);
	gridDIAG.ok = gtk_button_new_with_label("Ok");
	gridDIAG.gridS = gtk_spin_button_new_with_range(0,50,10);
	gtk_container_set_border_width(GTK_CONTAINER(gridDIAG.window),3);
	gtk_table_attach_defaults(GTK_TABLE(gridDIAG.table),gridDIAG.banner,0,2,0,1);
	gtk_table_attach_defaults(GTK_TABLE(gridDIAG.table),gtk_label_new("Grid Size:"),0,1,1,2);
	gtk_table_attach_defaults(GTK_TABLE(gridDIAG.table),gridDIAG.gridS,1,2,1,2);
	gtk_table_attach_defaults(GTK_TABLE(gridDIAG.table),gridDIAG.snap,1,2,2,3);
	gtk_table_attach_defaults(GTK_TABLE(gridDIAG.table),gridDIAG.ok,1,2,3,4);
	gtk_container_add(GTK_CONTAINER(gridDIAG.window),gridDIAG.table);

	g_signal_connect(G_OBJECT(gridDIAG.window),"delete-event",G_CALLBACK(dlgWindowDeleteEvent),NULL);
	g_signal_connect(G_OBJECT(gridDIAG.gridS),"value-changed",G_CALLBACK(gridChange),NULL);
	g_signal_connect(G_OBJECT(gridDIAG.snap),"clicked",G_CALLBACK(gridSnap),NULL);
	g_signal_connect_swapped(G_OBJECT(gridDIAG.ok),"clicked",G_CALLBACK(gtk_widget_hide),gridDIAG.window);

}

void makeAboutDialog()
{
	aboutDIAG.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_type_hint(GTK_WINDOW(aboutDIAG.window),GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_resizable(GTK_WINDOW(aboutDIAG.window),0);
	gtk_window_set_title(GTK_WINDOW(aboutDIAG.window),"About Nonlogic Pad");
	aboutDIAG.table = gtk_table_new(5,2,0);
	aboutDIAG.banner = gtk_image_new_from_file("pixbufs/banner.png");
	gtk_table_set_row_spacings(GTK_TABLE(aboutDIAG.table),4);
	aboutDIAG.ok = gtk_button_new_with_label("Ok");
	gtk_container_set_border_width(GTK_CONTAINER(aboutDIAG.window),3);
	gtk_table_attach_defaults(GTK_TABLE(aboutDIAG.table),aboutDIAG.banner,0,2,0,1);
	GtkWidget *aboutLabel = gtk_label_new("Nonlogic Pad was based on the concept of Techpad which\nwas originally created using Clanlib libraries.Nonlogic\npad is completely new, written in C using GTK+, Cairo and\nGNet libraries.\n\nNonlogic Pad lives on the Nonlogic.org server. Nonlogic.org is\na Non-profit organisation that runs off the continual support of its\nmembers. If you find this program at all useful, please consider\nmaking a small donation at http://www.nonlogic.org/?donate.");
	gtk_label_set_justify(GTK_LABEL(aboutLabel),GTK_JUSTIFY_FILL);
	gtk_table_attach_defaults(GTK_TABLE(aboutDIAG.table),aboutLabel,0,2,1,3);
	gtk_table_attach_defaults(GTK_TABLE(aboutDIAG.table),aboutDIAG.ok,1,2,3,4);
	gtk_container_add(GTK_CONTAINER(aboutDIAG.window),aboutDIAG.table);

	g_signal_connect(G_OBJECT(aboutDIAG.window),"delete-event",G_CALLBACK(dlgWindowDeleteEvent),NULL);
	g_signal_connect_swapped(G_OBJECT(aboutDIAG.ok),"clicked",G_CALLBACK(gtk_widget_hide),aboutDIAG.window);
}

void makeMainWindow()
{
	//Define widgets
	mainGUI.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(mainGUI.window),"Nonlogic Pad");
	gtk_widget_set_size_request(mainGUI.window,660,660);

#if GTK_MINOR_VERSION >= 10
	mainGUI.nlIcon = gtk_status_icon_new_from_file("pixbufs/sicon.png"); //Status Icon
#endif

	mainGUI.vbox3 = gtk_vbox_new(0,0); // contains EVERYTHING
	mainGUI.vbox = gtk_vpaned_new(); // Holds [canvas userlist]/[history messagebox]
	mainGUI.hbox = gtk_hpaned_new(); // Holds the canvas and user list
	mainGUI.vbox2 = gtk_vbox_new(0,0);
	mainGUI.hbox2 = gtk_hbox_new(0,0);
	mainGUI.table = gtk_table_new(14,2,0);
	mainGUI.scrolledWin = gtk_scrolled_window_new(0,0);
	mainGUI.scrolledWin2 = gtk_scrolled_window_new(0,0);
	mainGUI.vp = gtk_viewport_new(0,0);
	mainGUI.vp2 = gtk_viewport_new(0,0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(mainGUI.scrolledWin2),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(mainGUI.vp,105,0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(mainGUI.scrolledWin),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	mainGUI.onlineList = gtk_tree_view_new();
	gtk_container_add(GTK_CONTAINER(mainGUI.scrolledWin),mainGUI.onlineList);
	gtk_container_add(GTK_CONTAINER(mainGUI.vp),mainGUI.scrolledWin);
	mainGUI.onlineLS = gtk_list_store_new(2,G_TYPE_STRING,G_TYPE_INT);
	GtkCellRenderer *textR = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(mainGUI.onlineList),-1,"Clients",textR,"text",0,NULL);
	gtk_tree_view_set_model(GTK_TREE_VIEW(mainGUI.onlineList),GTK_TREE_MODEL(mainGUI.onlineLS));
	gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(mainGUI.onlineLS), 0, GTK_SORT_ASCENDING);
	mainGUI.history = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(mainGUI.history),GTK_WRAP_WORD_CHAR);
	mainGUI.historyBuf = gtk_text_buffer_new(NULL);
	GtkTextTag *blue = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(mainGUI.historyBuf),"blue","foreground","#0000ff","weight","bold",NULL);
	GtkTextTag *purple = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(mainGUI.historyBuf),"purple","foreground","#990099","weight","bold",NULL);
	GtkTextTag *bold = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(mainGUI.historyBuf),"bold","weight","bold",NULL);
	
	gtk_container_add(GTK_CONTAINER(mainGUI.scrolledWin2),mainGUI.history);
	gtk_container_add(GTK_CONTAINER(mainGUI.vp2),mainGUI.scrolledWin2);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(mainGUI.history),mainGUI.historyBuf);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(mainGUI.history),0);
	gtk_widget_set_name(mainGUI.history,"chatHistory");

	mainGUI.drwArea = gtk_drawing_area_new();
	gtk_widget_set_size_request(mainGUI.drwArea,500,500);
	
	addClient(globals.nick,-1);
	mainGUI.msgbox = gtk_entry_new();
	gtk_widget_set_name(mainGUI.msgbox,"chatBox");
	gtk_entry_set_text(GTK_ENTRY(mainGUI.msgbox),"");
	gtk_entry_set_max_length(GTK_ENTRY(mainGUI.msgbox),128);
	mainGUI.strWidth = gtk_hscale_new_with_range(1,15,1);
	gtk_scale_set_value_pos(GTK_SCALE(mainGUI.strWidth),GTK_POS_RIGHT);
	gtk_widget_set_size_request(mainGUI.strWidth,60,30);

	makeToolbar();
	
	gtk_box_pack_start(GTK_BOX(mainGUI.vbox3),mainGUI.toolbar,0,0,0);
	gtk_box_pack_start(GTK_BOX(mainGUI.vbox3),mainGUI.vbox,1,1,0);
	gtk_box_pack_start(GTK_BOX(mainGUI.vbox2),mainGUI.vp2,1,1,0);
	gtk_box_pack_start(GTK_BOX(mainGUI.vbox2),mainGUI.msgbox,0,1,0);
	gtk_box_pack_start(GTK_BOX(mainGUI.hbox2),mainGUI.vbox2,1,1,0);
	gtk_paned_add1(GTK_PANED(mainGUI.hbox),mainGUI.drwArea);
	gtk_paned_pack2(GTK_PANED(mainGUI.hbox),mainGUI.vp,0,0);
	//gtk_box_pack_start(GTK_BOX(mainGUI.vbox),mainGUI.toolbar,0,0,0);
	gtk_paned_add1(GTK_PANED(mainGUI.vbox),mainGUI.hbox);
	gtk_paned_add2(GTK_PANED(mainGUI.vbox),mainGUI.hbox2);
	gtk_container_add(GTK_CONTAINER(mainGUI.window),mainGUI.vbox3);
	
	gtk_widget_set_events(mainGUI.drwArea, gtk_widget_get_events(mainGUI.drwArea) | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_BUTTON_MOTION_MASK);

	g_signal_connect(G_OBJECT(mainGUI.window),"delete-event",G_CALLBACK(windowDeleteEvent),NULL);
	g_signal_connect(G_OBJECT(mainGUI.drwArea),"expose-event",G_CALLBACK(drwAreaExpose),NULL);
	g_signal_connect(G_OBJECT(mainGUI.msgbox),"activate",G_CALLBACK(msgboxActivate),NULL);
	g_signal_connect(G_OBJECT(mainGUI.msgbox),"key-press-event",G_CALLBACK(msgboxKeyPress),NULL);	
	g_signal_connect(G_OBJECT(mainGUI.drwArea),"button-press-event", G_CALLBACK (drwAreaPress), NULL);
	g_signal_connect(G_OBJECT(mainGUI.drwArea),"button-release-event", G_CALLBACK (drwAreaRelease), NULL);
	g_signal_connect(G_OBJECT(mainGUI.drwArea),"motion-notify-event", G_CALLBACK (drwAreaMove), NULL);
	g_signal_connect(G_OBJECT(mainGUI.window),"key-press-event",G_CALLBACK(windowKeys),NULL);
	g_signal_connect(G_OBJECT(mainGUI.window),"window-state-event",G_CALLBACK(windowEvent),NULL);
#if GTK_MINOR_VERSION >= 10
	g_signal_connect(G_OBJECT(mainGUI.nlIcon),"popup-menu",G_CALLBACK(onMenu),NULL);
	g_signal_connect(G_OBJECT(mainGUI.nlIcon),"activate",G_CALLBACK(onIconActivate),NULL);
#endif

}

void makeToolbar()
{
	mainGUI.toolbartips = gtk_tooltips_new();
	GtkToolItem *toolItem;
	mainGUI.toolbar = gtk_toolbar_new();
	gtk_toolbar_set_tooltips(GTK_TOOLBAR(mainGUI.toolbar),1);
	gtk_toolbar_set_style(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOLBAR_ICONS);
	mainGUI.conn = gtk_tool_button_new(gtk_image_new_from_file("pixbufs/conn.png"),NULL);
	gtk_tool_item_set_tooltip(mainGUI.conn,mainGUI.toolbartips,"Connect/Disconnect from a server",NULL);
	g_signal_connect(G_OBJECT(mainGUI.conn),"clicked",G_CALLBACK(connectClicked),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),mainGUI.conn,-1);

	toolItem = gtk_tool_button_new(gtk_image_new_from_file("pixbufs/id.png"),NULL);
	gtk_tool_item_set_tooltip(toolItem,mainGUI.toolbartips,"Change your ID","Your ID is used in the client list and in the text chat");
	g_signal_connect_swapped(G_OBJECT(toolItem),"clicked",G_CALLBACK(gtk_widget_show_all),nickDIAG.window);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);

	toolItem = gtk_tool_button_new(gtk_image_new_from_file("pixbufs/chan.png"),NULL);
	gtk_tool_item_set_tooltip(toolItem,mainGUI.toolbartips,"Change the channel","The channel you are in affects what content you see");
	g_signal_connect_swapped(G_OBJECT(toolItem),"clicked",G_CALLBACK(gtk_widget_show_all),chanDLG.window);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	
	toolItem = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	
	toolItem = gtk_tool_item_new();
	gtk_container_add(GTK_CONTAINER(toolItem),mainGUI.strWidth);
	gtk_tool_item_set_tooltip(toolItem,mainGUI.toolbartips,"Change the size of your stroke",NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	
	toolItem = gtk_tool_button_new(gtk_image_new_from_file("pixbufs/grid.png"),NULL);
	g_signal_connect_swapped(G_OBJECT(toolItem),"clicked",G_CALLBACK(gtk_widget_show_all),gridDIAG.window);
	gtk_tool_item_set_tooltip(toolItem,mainGUI.toolbartips,"Alter the grid settings",NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	
	toolItem = gtk_tool_button_new(gtk_image_new_from_file("pixbufs/col.png"),NULL);
	gtk_tool_item_set_tooltip(toolItem,mainGUI.toolbartips,"Change the colour of your stroke",NULL);
	g_signal_connect(G_OBJECT(toolItem),"clicked",G_CALLBACK(colourSet),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
		
	toolItem = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	
	toolItem = gtk_tool_button_new(gtk_image_new_from_file("pixbufs/brush.png"),NULL);
	gtk_tool_item_set_tooltip(toolItem,mainGUI.toolbartips,"Freehand Tool",NULL);
	g_signal_connect(G_OBJECT(toolItem),"clicked",G_CALLBACK(changeTool),(int*)1);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
//	toolItem = gtk_tool_button_new(gtk_image_new_from_file("pixbufs/eraser.xpm"),NULL);
//	g_signal_connect(G_OBJECT(toolItem),"clicked",G_CALLBACK(changeTool),(int*)2);
//	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);

	toolItem = gtk_tool_button_new(gtk_image_new_from_file("pixbufs/line.png"),NULL);
	gtk_tool_item_set_tooltip(toolItem,mainGUI.toolbartips,"Line tool",NULL);
	g_signal_connect(G_OBJECT(toolItem),"clicked",G_CALLBACK(changeTool),(int*)3);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);

	toolItem = gtk_tool_button_new(gtk_image_new_from_file("pixbufs/rect.png"),NULL);
	gtk_tool_item_set_tooltip(toolItem,mainGUI.toolbartips,"Rectangle tool",NULL);
	g_signal_connect(G_OBJECT(toolItem),"clicked",G_CALLBACK(changeTool),(int*)4);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);

	toolItem = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);

	toolItem = gtk_tool_button_new(gtk_image_new_from_file("pixbufs/clear.png"),NULL);
	gtk_tool_item_set_tooltip(toolItem,mainGUI.toolbartips,"Clear the canvas", NULL);
	g_signal_connect(G_OBJECT(toolItem),"clicked",G_CALLBACK(clearClicked),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	
	toolItem = gtk_tool_button_new(gtk_image_new_from_file("pixbufs/dump.png"),NULL);
	gtk_tool_item_set_tooltip(toolItem,mainGUI.toolbartips,"Dump the canvas to a .png",NULL);
	g_signal_connect(G_OBJECT(toolItem),"clicked",G_CALLBACK(dumpClicked),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	
	toolItem = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	
	toolItem = gtk_tool_button_new(gtk_image_new_from_file("pixbufs/about.png"),NULL);
	gtk_tool_item_set_tooltip(toolItem,mainGUI.toolbartips,"About Nonlogic Pad", NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	g_signal_connect_swapped(G_OBJECT(toolItem),"clicked",G_CALLBACK(gtk_widget_show_all),aboutDIAG.window);
}
