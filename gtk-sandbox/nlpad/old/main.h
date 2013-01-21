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

#ifndef __MAIN_H__
#define __MAIN_H__

#include <gtk/gtk.h>
#include <gnet.h>
#include <stdlib.h> // FOR INT32_T DEF!
#ifdef WIN32
#include <stdint.h>
#endif
#include "helper.h"

// Main Window
typedef struct _mainGui
{
	GtkWidget *window;
	GtkWidget *onlineList, *scrolledWin, *scrolledWin2, *vp, *vp2;
	GtkListStore *onlineLS;
	GtkWidget *drwArea;
	GtkWidget *hbox,*hbox2,*vbox,*vbox2, *vbox3,*table, *toolbar;
	GtkWidget *msgbox, *history;
	GtkTextBuffer *historyBuf;
	GtkWidget *connect, *clear, *dump;
	GtkWidget *strWidth, *gridS;
	GtkToolItem *conn;
	GtkTooltips *toolbartips;
#if GTK_MINOR_VERSION >= 10
	GtkStatusIcon *nlIcon;
#endif
}mainGui;

// ID Change Dialog
typedef struct _nickDiag
{
	GtkWidget *window;
	GtkWidget *banner;
	GtkWidget *table;
	GtkWidget *nick;
	GtkWidget *ok, *cancel;
}nickDiag;

// Grid Dialog
typedef struct _gridDiag
{
	GtkWidget *window;
	GtkWidget *banner;
	GtkWidget *table;
	GtkWidget *gridS;
	GtkWidget *snap;
	GtkWidget *ok;
}gridDiag;

// Connect Dialog
typedef struct _subGui
{
	GtkWidget *window;
	GtkWidget *banner;
	GtkWidget *table;
	GtkWidget *server, *port;
	GtkWidget *connect, *cancel;
}subGui;

// About dialog
typedef struct _aboutDiag
{
	GtkWidget *window;
	GtkWidget *banner;
	GtkWidget *table;
	GtkWidget *ok;
	GtkWidget *donate;
}aboutDiag;

mainGui mainGUI;
subGui subGUI;
nickDiag nickDIAG;
gridDiag gridDIAG;
aboutDiag aboutDIAG;

#endif
