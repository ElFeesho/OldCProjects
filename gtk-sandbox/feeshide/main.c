/*

Feeshide- The Coders' Element: Small lightweight integrated development environment
Copyright (C) 2006  Christopher (Feesh!) Sawczuk

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#include <gtk/gtk.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filelist.h"
#include "callback.h"

void airbag()
{
	//Catch any fatal errors and back up current buffers (Hopefully wont be used)
	fileList *temp = filelist;
	while(temp)
	{
		FILE *backup = fopen(strcat(temp->fname,".save"),"w");
		if(backup)
		{
			GtkSourceBuffer *buff = temp->textBuff;
			GtkTextIter start, end;
			gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(buff),&start, &end);
			char *text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buff),&start, &end, 0);
			fprintf(backup,"!!FEESHIDE BACKUPFILE!!\n%s\n!!END OF FEESHIDE BACKUPFILE!!\n",text);
			fclose(backup);
			g_free(text);
			g_free(buff);
		}
		fclose(backup);
		temp=temp->next;
	}
	GtkWidget *dialog = gtk_message_dialog_new(NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"Caught a segmentation fault, all your data has been saved to FILENAME.save.",0);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	gtk_exit(1);
}

void init()
{
	//Initialise the GUI
	GtkToolItem *toolItem;
	mainGUI.toolbar = gtk_toolbar_new();
	mainGUI.statusBar = gtk_statusbar_new();
	gtk_statusbar_push(GTK_STATUSBAR(mainGUI.statusBar),0,"Column: 0");
	gtk_statusbar_push(GTK_STATUSBAR(mainGUI.statusBar),0,"Total Lines In File: 0");
	gtk_statusbar_push(GTK_STATUSBAR(mainGUI.statusBar),0,"Total Lines In Group: 0");
	gtk_statusbar_push(GTK_STATUSBAR(mainGUI.statusBar),0,"Total Lines In Total: 0");
	gtk_toolbar_set_style(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOLBAR_BOTH_HORIZ);
	toolItem = gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
	g_signal_connect(G_OBJECT(toolItem),"clicked",G_CALLBACK(onMenuNew),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	toolItem = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	toolItem = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
	g_signal_connect(G_OBJECT(toolItem),"clicked",G_CALLBACK(onMenuOpen),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	toolItem = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
	g_signal_connect(G_OBJECT(toolItem),"clicked",G_CALLBACK(onMenuSave),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	toolItem = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE_AS);
	g_signal_connect(G_OBJECT(toolItem),"clicked",G_CALLBACK(onMenuSaveAs),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	toolItem = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	toolItem = gtk_tool_button_new_from_stock(GTK_STOCK_QUIT);
	g_signal_connect(G_OBJECT(toolItem),"clicked",G_CALLBACK(onClose),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	toolItem = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);

	toolItem = gtk_tool_button_new_from_stock(GTK_STOCK_GOTO_FIRST);
	g_signal_connect(G_OBJECT(toolItem),"clicked",G_CALLBACK(onMenuPageFirst),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	toolItem = gtk_tool_button_new_from_stock(GTK_STOCK_GO_BACK);
	g_signal_connect(G_OBJECT(toolItem),"clicked",G_CALLBACK(onMenuPagePrev),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	toolItem = gtk_tool_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	g_signal_connect(G_OBJECT(toolItem),"clicked",G_CALLBACK(onMenuPageNext),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	toolItem = gtk_tool_button_new_from_stock(GTK_STOCK_GOTO_LAST);
	g_signal_connect(G_OBJECT(toolItem),"clicked",G_CALLBACK(onMenuPageLast),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);
	toolItem = gtk_tool_button_new_from_stock(GTK_STOCK_DELETE);
	g_signal_connect(G_OBJECT(toolItem),"clicked",G_CALLBACK(onMenuPageClose),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);

	toolItem = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);

	toolItem = gtk_tool_button_new_from_stock(GTK_STOCK_ABOUT);
	g_signal_connect(G_OBJECT(toolItem),"clicked",G_CALLBACK(onMenuAbout),NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(mainGUI.toolbar),GTK_TOOL_ITEM(toolItem),-1);

	mainGUI.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(mainGUI.window),"Feeshide");
	gtk_widget_set_size_request(mainGUI.window,640,430);
	GtkWidget *scrollWindow = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollWindow),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	mainGUI.vbox = gtk_vbox_new(0,0);
	mainGUI.hbox = gtk_hpaned_new();
	mainGUI.treeView = gtk_tree_view_new();
	g_signal_connect(G_OBJECT(mainGUI.treeView),"row-activated",G_CALLBACK(onGroupActivate),NULL);
	mainGUI.treeStore = gtk_list_store_new(3,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_UINT);
	GtkCellRenderer *textR = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(mainGUI.treeView),-1,"Group",textR,"text",0,NULL);
	mainGUI.menu = gtk_menu_new();
	mainGUI.menuBar = gtk_menu_bar_new();
	mainGUI.menuFile = gtk_menu_item_new_with_mnemonic("_File");
	mainGUI.menuEdit = gtk_menu_item_new_with_mnemonic("_Edit");
	mainGUI.menuView = gtk_menu_item_new_with_mnemonic("_View");
	mainGUI.menuGroup = gtk_menu_item_new_with_mnemonic("_Group");
	mainGUI.menuPage = gtk_menu_item_new_with_mnemonic("_Page");
	mainGUI.menuHelp = gtk_menu_item_new_with_mnemonic("_Help");

	GtkWidget *menuItem = gtk_menu_item_new_with_label("New");
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuNew),NULL);
	gtk_menu_append(GTK_MENU(mainGUI.menu), menuItem);
	menuItem = gtk_menu_item_new_with_label("Open");
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuOpen),NULL);
	gtk_menu_append(GTK_MENU(mainGUI.menu), menuItem);
	menuItem = gtk_menu_item_new_with_label("Save");
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuSave),NULL);
	gtk_menu_append(GTK_MENU(mainGUI.menu), menuItem);
	menuItem = gtk_menu_item_new_with_label("Save As");
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuSaveAs),NULL);
	gtk_menu_append(GTK_MENU(mainGUI.menu), menuItem);
	gtk_menu_append(GTK_MENU(mainGUI.menu), gtk_separator_menu_item_new());
	menuItem = gtk_menu_item_new_with_label("Exit");
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuExit),NULL);
	gtk_menu_append(GTK_MENU(mainGUI.menu), menuItem);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mainGUI.menuFile),mainGUI.menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(mainGUI.menuBar),mainGUI.menuFile);

	mainGUI.menu = gtk_menu_new();
	menuItem = gtk_menu_item_new_with_label("New Group");
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuNewGroup),NULL);
	gtk_menu_append(GTK_MENU(mainGUI.menu), menuItem);
	menuItem = gtk_menu_item_new_with_label("Open Group");
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuOpenGroup),NULL);
	gtk_menu_append(GTK_MENU(mainGUI.menu), menuItem);
	menuItem = gtk_menu_item_new_with_label("Save Group");
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuSaveGroup),NULL);
	gtk_menu_append(GTK_MENU(mainGUI.menu), menuItem);
	menuItem = gtk_menu_item_new_with_label("New File to Group");
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuNewFileGroup),NULL);
	gtk_menu_append(GTK_MENU(mainGUI.menu), menuItem);
	menuItem = gtk_menu_item_new_with_label("Add File to Group");
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuAddFileGroup),NULL);
	gtk_menu_append(GTK_MENU(mainGUI.menu), menuItem);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mainGUI.menuGroup),mainGUI.menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(mainGUI.menuBar),mainGUI.menuGroup);

	mainGUI.menu = gtk_menu_new();
	menuItem = gtk_menu_item_new_with_label("Font");
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuFont),NULL);
	gtk_menu_append(GTK_MENU(mainGUI.menu), menuItem);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mainGUI.menuEdit),mainGUI.menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(mainGUI.menuBar),mainGUI.menuEdit);

	mainGUI.menu = gtk_menu_new();
	menuItem = gtk_check_menu_item_new_with_label("Toolbar");
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menuItem),1);
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuToolbar),NULL);
	gtk_menu_append(GTK_MENU(mainGUI.menu), menuItem);
	menuItem = gtk_check_menu_item_new_with_label("Group Bar");
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menuItem),1);
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuGroupbar),NULL);
	gtk_menu_append(GTK_MENU(mainGUI.menu), menuItem);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mainGUI.menuView),mainGUI.menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(mainGUI.menuBar),mainGUI.menuView);

	mainGUI.menu = gtk_menu_new();
	menuItem = gtk_menu_item_new_with_label("Close Current");
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuPageClose),NULL);
	gtk_menu_append(GTK_MENU(mainGUI.menu), menuItem);
	menuItem = gtk_menu_item_new_with_label("Next");
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuPageNext),NULL);
	gtk_menu_append(GTK_MENU(mainGUI.menu), menuItem);
	menuItem = gtk_menu_item_new_with_label("Previous");
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuPagePrev),NULL);
	gtk_menu_append(GTK_MENU(mainGUI.menu), menuItem);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mainGUI.menuPage),mainGUI.menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(mainGUI.menuBar),mainGUI.menuPage);

	mainGUI.menu = gtk_menu_new();
	menuItem = gtk_menu_item_new_with_label("About");
	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(onMenuAbout),NULL);
	gtk_menu_append(GTK_MENU(mainGUI.menu), menuItem);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mainGUI.menuHelp),mainGUI.menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(mainGUI.menuBar),mainGUI.menuHelp);

	gtk_box_pack_start(GTK_BOX(mainGUI.vbox),mainGUI.menuBar,0,1,0);
	gtk_box_pack_start(GTK_BOX(mainGUI.vbox),mainGUI.toolbar,0,1,0);
	mainGUI.notebook = gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(mainGUI.notebook),GTK_POS_BOTTOM);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(mainGUI.notebook),TRUE);
	gtk_container_add(GTK_CONTAINER(scrollWindow),mainGUI.treeView);
	gtk_widget_set_size_request(mainGUI.treeView,140,0);
	gtk_paned_add1(GTK_PANED(mainGUI.hbox),scrollWindow);
	gtk_paned_add2(GTK_PANED(mainGUI.hbox),mainGUI.notebook);
	gtk_box_pack_start(GTK_BOX(mainGUI.vbox),mainGUI.hbox,1,1,0);
	gtk_box_pack_start(GTK_BOX(mainGUI.vbox),mainGUI.statusBar,0,1,0);

	gtk_container_add(GTK_CONTAINER(mainGUI.window),mainGUI.vbox);
	gtk_widget_show_all(mainGUI.window);
	g_signal_connect(G_OBJECT(mainGUI.window),"delete-event",G_CALLBACK(onClose),NULL);	
	mainGUI.font = malloc(32);
	g_signal_connect(G_OBJECT(mainGUI.notebook),"popup-menu",G_CALLBACK(onNoteMenu),NULL);
	g_signal_connect(G_OBJECT(mainGUI.notebook),"button-press-event",G_CALLBACK(onNoteMouse),NULL);
	g_signal_connect(G_OBJECT(mainGUI.treeView),"button-press-event",G_CALLBACK(onTreeViewMouse),NULL);
	mainGUI.cGroup = 0;
	//Hook up sigsegv signal, so people wont sue me for losing work :(
	signal( SIGSEGV, airbag);
	strcpy(mainGUI.font,"courier new 10");
}

int main(int argc, char ** argv)
{
	//Where all the magic comes together
	gtk_init(&argc, &argv);
	init();
	gtk_main();
	return 0;
}
