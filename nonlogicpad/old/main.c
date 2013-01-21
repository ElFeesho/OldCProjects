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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "callbacks.h"
#include "main.h"
#include "config.h"
#include "globals.h"
#include "chanDialog.h"

#ifdef WIN32
#include <winuser.h>
#endif

int32_t main(int32_t argc, char **argv)
{
	char *cFile = getConfigFile();
	loadConfig(cFile);
	free(cFile);

	gtk_init(&argc, &argv);
	gtk_rc_parse("style.rc");

	// Load default cursor
	GdkCursor *ncursor = 0;
	GdkPixbuf *brush = gdk_pixbuf_new_from_file("pixbufs/brush.png",NULL);
	ncursor = gdk_cursor_new_from_pixbuf(gdk_display_get_default(),brush,2,18);
	
	//Initialise any outstanding globals
	initGlobals();
	
	//Initialise connect window first, because I can!
	makeConnectDialog();
	makeNickDialog();
	makeChannelDialog();
	makeGridDialog();
	makeAboutDialog();
	makeMainWindow();
	addToHistory("Welcome to Nonlogic Pad. Type /list for a list of commands!");

	//Show Widgets
	gtk_widget_show_all(mainGUI.window);
	//Set default cursor
	gdk_window_set_cursor(mainGUI.drwArea->window,ncursor);
	//Show introduction
	showWelcome();
	//Enter main loop	
	gtk_main();
	return 0;
}
