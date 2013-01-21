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

#ifndef __CALL__
#define __CALL__

#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcelanguagesmanager.h>
#include "filelist.h"


GtkWidget *newPage(GtkSourceBuffer *buf);
int newNotePage(char *name);
gboolean onClose(GtkWidget *widget, gpointer user_data);
void saveFile(int page);
char saveAs(int page);
void onChange(GtkWidget *widget, gpointer user_data);
void onMenuNew(GtkWidget *widget, gpointer userdata);
void onMenuOpen(GtkWidget *widget, gpointer userdata);
void onMenuSave(GtkWidget *widget, gpointer userdata);
void onMenuSaveAs(GtkWidget *widget, gpointer userdata);
void onMenuNewGroup(GtkWidget *widget, gpointer userdata);
void onMenuOpenGroup(GtkWidget *widget, gpointer userdata);
void onMenuSaveGroup(GtkWidget *widget, gpointer userdata);
void onMenuNewFileGroup(GtkWidget *widget, gpointer userdata);
void onMenuAddFileGroup(GtkWidget *widget, gpointer userdata);
void onGroupActivate(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
void onMenuExit(GtkWidget *widget, gpointer userdata);
void onMenuToolbar(GtkWidget *widget, gpointer userdata);
void onMenuGroupbar(GtkWidget *widget, gpointer userdata);
void onMenuPageClose(GtkWidget *widget, gpointer userdata);
void onMenuPageFirst(GtkWidget *widget, gpointer userdata);
void onMenuPageLast(GtkWidget *widget, gpointer userdata);
void onMenuPageNext(GtkWidget *widget, gpointer userdata);
void onMenuPagePrev(GtkWidget *widget, gpointer userdata);
void onMenuFont(GtkWidget *widget, gpointer userdata);
void onMenuAbout(GtkWidget *widget, gpointer userdata);
void onNoteMenu(GtkWidget *widget, gpointer userdata);
gboolean onNoteMouse(GtkWidget *widget, GdkEventButton *event);
gboolean onTreeViewMouse(GtkWidget *widget, GdkEventButton *event);
void groupRemoveFile(GtkWidget *widget, gpointer user_data);
void onMenuAddToGroup(GtkWidget *widget, gpointer userdata);
void do_popup_menu (GtkWidget *my_widget, GdkEventButton *event);
void onScroll(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
void onBufferChange(GtkWidget *buffer, gpointer user_data);
void highlightBuffer(GtkSourceBuffer *buff, char *filename);
int ovector[30];
void setGroupName(char *group);

typedef struct
{
	char *cGroup;
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *treeView;
	GtkListStore *treeStore;
	GtkWidget *notebook;
	GtkWidget *menu;
	GtkWidget *toolbar;
	GtkWidget *menuBar;
	GtkWidget *menuFile;
	GtkWidget *menuEdit;
	GtkWidget *menuView;
	GtkWidget *menuGroup;
	GtkWidget *menuPage;
	GtkWidget *menuHelp;
	GtkWidget *statusBar;
	char *font; //TODO: move to config file structure
}guiWid;

guiWid mainGUI;

#endif
