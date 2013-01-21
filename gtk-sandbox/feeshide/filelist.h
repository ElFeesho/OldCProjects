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

#ifndef __FILELIST__
#define __FILELIST__

#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>
#include "callback.h"

typedef struct _fileList
{
	char fname[128];
	GtkSourceBuffer *textBuff;
	GtkWidget *sourceView;
	gint id;
	gint pageNum;
	char saved;
	GtkWidget *label;
	struct _fileList *next;
}fileList;

fileList *filelist;
 
int fileAdd(char *filename);
void fileSetPage(int id, int pageNum);
GtkSourceBuffer *fileGetBuffer(gint page);
GtkWidget *fileGetLabel(gint page);
GtkSourceBuffer *fileGetBufferByPage(gint page);
GtkWidget *fileGetLabelByPage(gint page);
char fileGetSavedByPage(int page);
void fileSetSavedByPage(int page, char val);
void fileSetSaved(int id, char val);
char fileGetSaved(int id);
char *fileGetFileNameByPage(int page);
void fileClose(gint pageNum);
void fileSave(gint pageNum);
void fileDeleteByPage(int pageNum);
void fileClear(fileList **elem);

#endif
