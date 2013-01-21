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

#ifndef __CALLBACKS_H__
#define __CALLBACKS_H__

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <cairo.h>
#include <string.h>
#include <stdlib.h> // FOR INT32_T DEF!
#include <math.h>
#ifdef WIN32
#include <stdint.h>
#endif
#include "helper.h"
#include "main.h"

gboolean windowDeleteEvent(GtkWidget *widget, gpointer user_data);
gboolean dlgWindowDeleteEvent(GtkWidget *widget, gpointer user_data);
void colourSet(GtkWidget *widget, gpointer user_data);
gboolean drwAreaExpose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data);
void nickOkClicked(GtkWidget *widget, gpointer user_data);
void msgboxActivate(GtkWidget *widget, gpointer user_data);
gboolean drwAreaPress(GtkWidget *widget, GdkEventMotion *event, gpointer user_data);
gboolean drwAreaRelease(GtkWidget *widget, GdkEventMotion *event, gpointer user_data);
gboolean drwAreaMove(GtkWidget *widget, GdkEventMotion *event, gpointer user_data);
void connectClicked(GtkWidget *widget, gpointer user_data);
void connectSubClicked(GtkWidget *widget, gpointer user_data);
gboolean sockData(GIOChannel *source, GIOCondition condition, gpointer data);
void clearClicked(GtkWidget *widget, gpointer user_data);
void dumpClicked(GtkWidget *widget, gpointer user_data);
void gridChange(GtkSpinButton *spinbutton, gpointer user_data);
gboolean windowKeys(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
void changeTool(GtkWidget *widget, gpointer user_data);
void gridSnap(GtkWidget *widget, gpointer user_data);
gboolean windowEvent(GtkWidget *widget, GdkEventWindowState *event, gpointer user_data);
#if GTK_MINOR_VERSION >= 10
void onMenu(GtkStatusIcon *status_icon, guint button, guint activate_time, gpointer user_data);
void onIconActivate(GtkStatusIcon *status_icon, gpointer user_data);
#endif
gboolean msgboxKeyPress(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
#endif
