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

#include "callbacks.h"
#include "globals.h"

//mainGUI.window delete-event callback
gboolean windowDeleteEvent(GtkWidget *widget, gpointer user_data)
{
	if(globals.connected)
	{
		//Add a check to make sure they want to quit
		sendData("\x04",1);
		gnet_tcp_socket_delete(globals.tcpSock);
		globals.connected = 0;
	}
#if GTK_MINOR_VERSION >= 10
	gtk_status_icon_set_visible(mainGUI.nlIcon,0);
#endif
	gtk_exit(0);
	return 0;
}

//*.window delete-event callback
gboolean dlgWindowDeleteEvent(GtkWidget *widget, gpointer user_data)
{
	gtk_widget_hide(widget);
	return 1;
}

//mainGUI.colour color-set callback
void colourSetOk(GtkWidget *widget, GtkWidget *user_data)
{
	GdkColor ncol;
	gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(((GtkColorSelectionDialog*)user_data)->colorsel),&ncol);
	globals.r = ncol.red;
	globals.g = ncol.green;
	globals.b = ncol.blue;
	gtk_widget_destroy(user_data);
}

void colourSet(GtkWidget *widget, gpointer user_data)
{
	GdkColor ncol;
	ncol.red = globals.r;
	ncol.green = globals.g;
	ncol.blue = globals.b;
	GtkWidget *colDiag = gtk_color_selection_dialog_new("Choose an ink Colour");
	gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(((GtkColorSelectionDialog*)colDiag)->colorsel),&ncol);
	g_signal_connect_swapped(G_OBJECT(((GtkColorSelectionDialog*)colDiag)->cancel_button),"clicked",G_CALLBACK(gtk_widget_destroy),colDiag);
	g_signal_connect(G_OBJECT(((GtkColorSelectionDialog*)colDiag)->ok_button),"clicked",G_CALLBACK(colourSetOk),colDiag);
	gtk_widget_show_all(colDiag);
}

//mainGUI.drwArea expose-event callback
gboolean drwAreaExpose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data)
{
	if(globals.gridSize>0)
		drawGrid(globals.gridSize);
/*	cairo_t *cr = gdk_cairo_create(widget->window);
	cairo_move_to(cr,0,0);
	cairo_line_to(cr,widget->allocation.width,0);
	cairo_line_to(cr,widget->allocation.width,widget->allocation.height);
	cairo_line_to(cr,0,widget->allocation.height);
	cairo_line_to(cr,0,0);
	cairo_stroke(cr);
	cairo_destroy(cr);
	lineData *temp = lineList;
	for(;temp!=NULL;temp=temp->next)
	{
		//ONLY DRAW IFF!!! temp is infact INSIDE the expose box
		//Instead of drawing billions of lines for no need since
		//they are already on screen

		//Plans for future: make all drawings be blitted onto a pixmap
		//so exposign doesn't take seperate lines, but just blitting
		//Opens the door for flood fills :D
		if(((temp->x > event->area.x && temp->x < event->area.x+event->area.width) && (temp->y > event->area.y && temp->y < event->area.y+event->area.height)) || ((temp->x2 > event->area.x && temp->x2 < event->area.x+event->area.width) && (temp->y2 > event->area.y && temp->y < event->area.y+event->area.height)))
			drawLine(temp);
	}
*/
	if(globals.image!=NULL)
		gdk_draw_image(GDK_DRAWABLE(mainGUI.drwArea->window),gdk_gc_new(GDK_DRAWABLE(mainGUI.drwArea->window)),globals.image,0,0,0,0,mainGUI.drwArea->allocation.width,mainGUI.drwArea->allocation.height);
	if(globals.tline!=NULL)
	{
	}
	if(globals.trect!=NULL)
	{
	}
	if(!globals.start)
		showWelcome();
	return 1;
}

//nickDIAG.ok activate callback
void nickOkClicked(GtkWidget *widget, gpointer user_data)
{
	const char *nick = gtk_entry_get_text(GTK_ENTRY(nickDIAG.nick));
	memset(globals.nick,'\0',16);
	strncpy(globals.nick,nick,16);
	changeName((char*)nick,-1);
	if(globals.connected)
	{
		unsigned char data[17];
		gnet_pack("!bs",data,17,(char)0x09,globals.nick);
		sendData(data,17);
	}
	gtk_widget_hide(nickDIAG.window);
	return;
}

//mainGUI.msgbox activate callback
void msgboxActivate(GtkWidget *widget, gpointer user_data)
{
	const char *msg = gtk_entry_get_text(GTK_ENTRY(widget));
	if(strlen(msg)==0)
		return;
		
	if(msg[0]=='/' && strlen(msg)>1)
	{
		if(strncmp(subStr((char*)msg,' ',0,5),"/nick",5)==0) // Nick change
		{
			if(subStr((char*)msg,' ',1,16)!=NULL) // Catch more segs
			{
				memset(globals.nick,'\0',16);
				strncpy(globals.nick,subStr((char*)msg,' ',1,16),16);
				changeName((char*)subStr((char*)msg,' ',1,16),-1);
				if(globals.connected)
				{
					unsigned char data[17];
					gnet_pack("!bs",data,17,(char)0x09,globals.nick);
					sendData(data,17);
				}	
			}
		}else if(strncmp(subStr((char*)msg,' ',0,3),"/me",3)==0)
		{
			addActionHistory(-1,(char*)(msg+4),strlen(msg+4));	
			unsigned char data[258];
			memset(data,'\0',258);
			unsigned char len = strlen((msg+4));
			sprintf(data,"\x0C\x02%s\x03",(msg+4));
			if(globals.connected)
				sendData(data,len+3);
		}
		else if(strncmp(subStr((char*)msg,' ',0,5),"/list",5)==0)
		{
			addToHistory("List of Commands:\n\t/nick [new name]\n\t/me [some action]");
		}
		gtk_entry_set_text(GTK_ENTRY(widget),"");
		return;
	}
	addMsgHistory(-1,(char*)msg,strlen(msg));
	unsigned char data[258];
	memset(data,'\0',258);
	unsigned char len = strlen(msg);
	sprintf(data,"\x02\x02%s\x03",msg);
	if(globals.connected)
		sendData(data,len+3);
	gtk_entry_set_text(GTK_ENTRY(widget),"");
}

//mainGUI.drwArea mouse click callback
gboolean drwAreaPress(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
	if(event->type == GDK_BUTTON_PRESS)
	{
		globals.x = event->x;
		globals.y = event->y;
		if(globals.toolMode == 1)
		{
			addLineD(-1,globals.x, globals.y,event->x, event->y,globals.r,globals.g,globals.b,(char)((int)gtk_range_get_value(GTK_RANGE(mainGUI.strWidth))),globals.alpha);
			if(globals.connected)
			{
				unsigned char data[30];	
				gnet_pack("!b7ib",data,30,(char)0x01,(int)globals.x,(int)globals.y,(int)event->x,(int)event->y,(int)globals.r,(int)globals.g,(int)globals.b,(char)((int)gtk_range_get_value(GTK_RANGE(mainGUI.strWidth))));
				sendData(data,30);
			}
		}
		if(globals.snap)
		{
			if(globals.gridSize>0)
			{
				if(globals.toolMode==3||globals.toolMode==4)
				{
					globals.x = (int)(event->x/globals.gridSize)*globals.gridSize;
					globals.y = (int)(event->y/globals.gridSize)*globals.gridSize;
				}	
			}	
		}
		return 1;
	}
	return 0;
}

gboolean drwAreaRelease(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
	if(event->type == GDK_BUTTON_RELEASE)
	{
		if(globals.toolMode == 3)
		{
			if(globals.gridSize>0)
			{
				if(globals.snap)
				{
					event->x = (int)(event->x/globals.gridSize)*globals.gridSize;
					event->y = (int)(event->y/globals.gridSize)*globals.gridSize;
				}	
			}	
			addLineD(-1,globals.x, globals.y,event->x, event->y,globals.r,globals.g,globals.b,(char)((int)gtk_range_get_value(GTK_RANGE(mainGUI.strWidth))),globals.alpha);
			if(globals.connected)
			{
				unsigned char data[30];	
				gnet_pack("!b7ib",data,30,(char)0x01,(int)globals.x,(int)globals.y,(int)event->x,(int)event->y,(int)globals.r,(int)globals.g,(int)globals.b,(char)((int)gtk_range_get_value(GTK_RANGE(mainGUI.strWidth))));
				sendData(data,30);
			}
			return 1;
		}
		if(globals.toolMode == 4)
		{
			if(globals.gridSize>0)
			{
				if(globals.snap)
				{
					event->x = (int)(event->x/globals.gridSize)*globals.gridSize;
					event->y = (int)(event->y/globals.gridSize)*globals.gridSize;
				}	
			}
			addRectD(-1,globals.x, globals.y,event->x, event->y,globals.r,globals.g,globals.b,(char)((int)gtk_range_get_value(GTK_RANGE(mainGUI.strWidth))),globals.alpha);
			GdkRectangle rect;
			rect.x = 0;
			rect.y = 0;
			rect.width = 500;
			rect.height = 500;
			gdk_window_invalidate_rect(mainGUI.drwArea->window,&rect,0);
			if(globals.connected)
			{
				unsigned char data[30];	
				gnet_pack("!b7ib",data,30,(char)0x0A,(int)globals.x,(int)globals.y,(int)event->x,(int)event->y,(int)globals.r,(int)globals.g,(int)globals.b,(char)((int)gtk_range_get_value(GTK_RANGE(mainGUI.strWidth))));
				sendData(data,30);
			}
			return 1;
		}
	}
	globals.image = gdk_drawable_get_image(GDK_DRAWABLE(mainGUI.drwArea->window),0,0,mainGUI.drwArea->allocation.width,mainGUI.drwArea->allocation.height);
	return 0;
}

//mainGUI.drwArea mouse move whilst button down callback
gboolean drwAreaMove(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
	static int32_t cTime = 0;
	if(globals.toolMode == 3)
	{
		//Line mode
		if(globals.gridSize>0)
		{
			if(globals.snap)
			{
				event->x = (int)(event->x/globals.gridSize)*globals.gridSize;
				event->y = (int)(event->y/globals.gridSize)*globals.gridSize;
			}	
		}
		addTempLineD(globals.x, globals.y,event->x, event->y,globals.r,globals.g,globals.b,(char)((int)gtk_range_get_value(GTK_RANGE(mainGUI.strWidth))));
		return 1;
	}
	if(globals.toolMode == 4)
	{
		if(globals.gridSize>0)
		{
			if(globals.snap)
			{
				event->x = (int)(event->x/globals.gridSize)*globals.gridSize;
				event->y = (int)(event->y/globals.gridSize)*globals.gridSize;
			}	
		}
		addTempRectD(globals.x, globals.y,event->x, event->y,globals.r,globals.g,globals.b,(char)((int)gtk_range_get_value(GTK_RANGE(mainGUI.strWidth))));
		return 1;
	}
	if(globals.toolMode == 2)
	{
		//eraser mode
		if(event->time>cTime+15)
		{
		}
	}
	if(globals.toolMode == 1)
	{
		if(event->time>cTime+15)
		{
			addLineD(-1,globals.x, globals.y,event->x, event->y,globals.r,globals.g,globals.b,(char)((int)gtk_range_get_value(GTK_RANGE(mainGUI.strWidth))),globals.alpha);
			if(globals.connected)
			{
				unsigned char data[30];	
				gnet_pack("!b7ib",data,30,(char)0x01,(int)globals.x,(int)globals.y,(int)event->x,(int)event->y,(int)globals.r,(int)globals.g,(int)globals.b,(char)((int)gtk_range_get_value(GTK_RANGE(mainGUI.strWidth))));
				sendData(data,30);
			}
			globals.x = event->x;
			globals.y = event->y;
			cTime = event->time;
			return 1;
		}
	}
	return 1;
}

//mainGUI.connect clicked callback -> Launches subGUI
void connectClicked(GtkWidget *widget, gpointer user_data)
{
	globals.start = 1;
	clearCanvas();
	if(!globals.connected)
	{
		gtk_widget_show_all(subGUI.window);
	}
	if(globals.connected)
	{
		sendData("\x04",1);
		gnet_tcp_socket_delete(globals.tcpSock);
		globals.connected = 0;
		addToHistory("Disconnected from the server");
		purgeClientList();
		gtk_image_set_from_file(GTK_IMAGE(gtk_tool_button_get_icon_widget(GTK_TOOL_BUTTON(widget))),"pixbufs/conn.png");
	}
}

//subGUI.connect clicked, initiates the network connection
void connectSubClicked(GtkWidget *widget, gpointer user_data)
{
	if(globals.channel == NULL || strlen(globals.channel) == 0 || globals.channel[0]!='#')
	{
		errorDialog("Please enter a valid channel name!");
	}
	else
	{
		connectToServer((char*)gtk_entry_get_text(GTK_ENTRY(subGUI.server)), atoi(gtk_entry_get_text(GTK_ENTRY(subGUI.port))),globals.channel);
	}
	gtk_widget_hide(subGUI.window);
}

//On network data arival
gboolean sockData(GIOChannel *source, GIOCondition condition, gpointer data)
{
	if((condition&G_IO_IN) == G_IO_IN)
	{
		GIOError error = G_IO_ERROR_NONE;
		char header;
		gsize n = 1;
		error = gnet_io_channel_readn(globals.sockChan, &header, n, &n); 
		if(n == 0)
		{
			//Assume disconnect
			globals.connected = 0;
			gnet_tcp_socket_delete(globals.tcpSock);
			return 0;
		}
	   if (error != G_IO_ERROR_NONE) 
		{
			printf("Error receiving data\n");
			return 0;
		}

		if(header==0x01) // Line received
		{
			char idn[3];
			int idi;
			n = 3;
			error = gnet_io_channel_readn(globals.sockChan, idn, n, &n);
		   if (error != G_IO_ERROR_NONE) 
			{
				printf("Error receiving data\n");
				return 1;
			}		
			
			idi = atoi(idn);
			
			n = 29;
			unsigned char data[29];
			error = gnet_io_channel_readn(globals.sockChan, data, n, &n);
		   if (error != G_IO_ERROR_NONE) 
			{
				printf("Error receiving data\n");
				return 1;
			}		
			char stroke, alpha;
			int32_t x,y,x2,y2,r,g,b;
			gnet_unpack("!7ib",data,29,&x,&y,&x2,&y2,&r,&g,&b,&stroke);
			addLineD(idi,x,y,x2,y2,r,g,b,stroke,255);
		}
		if(header==0x0A) // Rect received
		{
			char idn[3];
			int idi;
			n = 3;
			error = gnet_io_channel_readn(globals.sockChan, idn, n, &n);
		   if (error != G_IO_ERROR_NONE) 
			{
				printf("Error receiving data\n");
				return 1;
			}		
			
			idi = atoi(idn);
			
			n = 30;
			unsigned char data[30];
			error = gnet_io_channel_readn(globals.sockChan, data, n, &n);
		   if (error != G_IO_ERROR_NONE) 
			{
				printf("Error receiving data\n");
				return 1;
			}		
			char stroke, alpha;
			int32_t x,y,x2,y2,r,g,b;
			gnet_unpack("!7ibB",data,30,&x,&y,&x2,&y2,&r,&g,&b,&stroke,&alpha);
			addRectD(idi,x,y,x2,y2,r,g,b,stroke,alpha);
		}
		if(header==0x02) // Msg received
		{
			char idn[3];
			int32_t idi;
			unsigned char *msg = 0;

			n = 3;

			error = gnet_io_channel_readn(globals.sockChan, idn, n, &n); 
	   	if (error != G_IO_ERROR_NONE) 
			{
				printf("Error receiving msger ID\n");
				return 1;
			}

			idi = atoi(idn);

			n = 1;
			unsigned char curChar = 0;
			unsigned char len = 0;
			msg = malloc(258);
			memset(msg,'\0',258);
			while(curChar!=3)
			{
				error = gnet_io_channel_readn(globals.sockChan, &curChar, n, &n);
		   	if (error != G_IO_ERROR_NONE) 
				{
					printf("Error receiving msg len\n");
					return 1;
				}
				if(curChar>31)
				{
					sprintf(msg,"%s%c",msg,curChar);
					len++;
				}
			}
			if(idi!=0)
				addMsgHistory(idi,msg,len);
			else
				addToHistory(msg);
			//Messages tend to get screwed up on sending as well as on reception :(
		}
		if(header==0x03) // Client connect
		{
			char idn[3];
			n = 3;
			error = gnet_io_channel_readn(globals.sockChan, idn, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				printf("Error receiving new user ID\n");
				return 1;
			}
			char nick[16];
			n = 16;
			error = gnet_io_channel_readn(globals.sockChan, nick, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				printf("Error receiving new user ID\n");
				return 1;
			}
			changeName(nick, atoi(idn));
			char *nclient = malloc(64);
			sprintf(nclient,"*** New Client Joined: %s",nick);
			addToHistory(nclient);
			free(nclient);
		}
		if(header==0x04) // Client Disconnect/Channel deserter
		{
			char idn[3];
			n = 3;
			error = gnet_io_channel_readn(globals.sockChan, idn, n, &n);
			if (error != G_IO_ERROR_NONE) 
			{
				printf("Error receiving user ID\n");
				return 1;
			}
			int32_t idi = atoi(idn);
			char *msg = malloc(128);
			sprintf(msg, "*** Client left the channel: %s",getClientName(idi));
			addToHistory(msg);
			removeClient(idi);
		}
		if(header==0x06) // Board Cleanup
		{
			char idn[3];
			n = 3;
			error = gnet_io_channel_readn(globals.sockChan, idn, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				printf("Error receiving client id\n");
				return 1;
			}
			int32_t idi = atoi(idn);
			char *msg = malloc(128);
			sprintf(msg,"*** %s cleared the board",getClientName(idi));
			addToHistory(msg);
			clearCanvas();
		}
		if(header==0x07) // Undo
		{
			char idn[3];
			n = 3;
			error = gnet_io_channel_readn(globals.sockChan, idn, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				printf("Error receiving client id\n");
				return 1;
			}
			int32_t idi = atoi(idn);
		}
		if(header==0x09) // Name change
		{
			char idn[3];
			n = 3;
			error = gnet_io_channel_readn(globals.sockChan, idn, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				printf("Error receiving msg len\n");
				return 1;
			}
			int32_t idi = atoi(idn);
			char nname[16];
			n = 16;
			error = gnet_io_channel_readn(globals.sockChan, nname, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				printf("Error receiving msg len\n");
				return 1;
			}
			char *msg = malloc(128);
			if(getClientName(idi)!=NULL)
			{
				if(idi!=-1)
					sprintf(msg,"*** %s changed name to %s",getClientName(idi),nname);
				else
				{
					sprintf(msg,"*** Server changed your name to %s",nname);
					strcpy(globals.nick,nname);
				}
				addToHistory(msg);
			}
			changeName(nname, idi);
		}
		if(header==0x0B) // Eraser
		{
			char idn[3];
			n = 3;
			error = gnet_io_channel_readn(globals.sockChan, idn, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				printf("Error receiving msg len\n");
				return 1;
			}
			int32_t idi = atoi(idn);
			char data[8];
			n = 8;
			error = gnet_io_channel_readn(globals.sockChan, data, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				printf("Error receiving msg len\n");
				return 1;
			}
			char stroke;
			int32_t x,y;
			gnet_unpack("!2i",data,8,&x,&y);
		}
		if(header==0x0C) // Msg received
		{
			char idn[3];
			int32_t idi;
			unsigned char *msg = 0;

			n = 3;

			error = gnet_io_channel_readn(globals.sockChan, idn, n, &n); 
		   	if (error != G_IO_ERROR_NONE) 
			{
				printf("Error receiving msger ID\n");
				return 1;
			}

			idi = atoi(idn);

			n = 1;
			unsigned char curChar = 0;
			unsigned char len = 0;
			msg = malloc(258);
			memset(msg,'\0',258);
			while(curChar!=3)
			{
				error = gnet_io_channel_readn(globals.sockChan, &curChar, n, &n);
			   	if (error != G_IO_ERROR_NONE) 
				{
					printf("Error receiving msg len\n");
					return 1;
				}
				if(curChar>31)
				{
					sprintf(msg,"%s%c",msg,curChar);
					len++;
				}
			}
			if(idi!=0)
				addActionHistory(idi,msg,len);
			else
				addToHistory(msg);
			//Messages tend to get screwed up on sending as well as on reception :(
		}
	}
	else if((condition&G_IO_HUP)==G_IO_HUP)
	{
		addToHistory("*** Server closed the connection!");
		errorDialog("Server closed the connection!");
		globals.connected = 0;
		gnet_tcp_socket_delete(globals.tcpSock);
		gtk_image_set_from_file(GTK_IMAGE(gtk_tool_button_get_icon_widget(GTK_TOOL_BUTTON(mainGUI.conn))),"pixbufs/conn.png");
		purgeClientList();
		return 0;
	}
	else if((condition&G_IO_ERR)==G_IO_ERR)
	{
		addToHistory("*** Connection error, closing conection!");
		errorDialog("Connection error, closing conection!");
		globals.connected = 0;
		gnet_tcp_socket_delete(globals.tcpSock);
		gtk_image_set_from_file(GTK_IMAGE(gtk_tool_button_get_icon_widget(GTK_TOOL_BUTTON(mainGUI.conn))),"pixbufs/conn.png");
		purgeClientList();
		return 0;
	}
	return 1;
}

//mainGUI.clear clicked - Clear the board
void clearClicked(GtkWidget *widget, gpointer user_data)
{
	clearCanvas();
	globals.start = 1;
	if(globals.connected)
		sendData("\x06",1);
}

void gridChange(GtkSpinButton *spinbutton, gpointer user_data)
{
	globals.gridSize = gtk_spin_button_get_value_as_int(spinbutton);
	flushCanvas();
}

//Dump the canvas contents to a png
void dumpClicked(GtkWidget *widget, gpointer user_data)
{
	dumpCanvas();
}

// read WINDOW KEYS now windows keys. Used to hook up the good ol CTRL+Z
gboolean windowKeys(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	static long int cTime = 0;
	if(event->state==GDK_CONTROL_MASK && event->type == GDK_KEY_PRESS)
	{
		if((event->keyval&GDK_Z) == GDK_Z && event->time>cTime+15)
		{
			if(globals.connected)
				sendData("\x07",1);
			cTime = event->time;
		}
	}
	return 0;
}

void changeTool(GtkWidget *widget, gpointer user_data)
{
		globals.toolMode = (int)user_data;
		changeCursor();
}

void gridSnap(GtkWidget *widget, gpointer user_data)
{
	globals.snap = 1-globals.snap;
}

gboolean windowEvent(GtkWidget *widget, GdkEventWindowState *event, gpointer user_data)
{
#if GTK_MINOR_VERSION >= 10
	if((event->new_window_state&GDK_WINDOW_STATE_ICONIFIED) == GDK_WINDOW_STATE_ICONIFIED)
	{		
		globals.visible = 0;
	}else{
		gtk_status_icon_set_from_file(mainGUI.nlIcon,"pixbufs/sicon.png");
		globals.visible = 1;
	}
#endif
}

#if GTK_MINOR_VERSION >= 10
void onMenu(GtkStatusIcon *status_icon, guint button, guint activate_time, gpointer user_data)
{
	GtkMenu *pmenu = (GtkMenu*)gtk_menu_new();
	GtkWidget *menuItem;
	if(globals.connected)		
		menuItem = gtk_menu_item_new_with_label("Disconnect");
	else
		menuItem = gtk_menu_item_new_with_label("Connect");
	gtk_menu_append(GTK_MENU(pmenu), menuItem);
	g_signal_connect_swapped(G_OBJECT(menuItem),"activate",G_CALLBACK(connectClicked),mainGUI.conn);
	menuItem = gtk_separator_menu_item_new();
	gtk_menu_append(GTK_MENU(pmenu),menuItem);
	menuItem = gtk_menu_item_new_with_label("Exit");
	gtk_menu_append(GTK_MENU(pmenu), menuItem);
	g_signal_connect_swapped(G_OBJECT(menuItem),"activate",G_CALLBACK(windowDeleteEvent),NULL);
	gtk_widget_show_all((GtkWidget*)pmenu);
	gtk_menu_popup(pmenu,NULL,NULL,NULL,NULL,button,activate_time);
}

void onIconActivate(GtkStatusIcon *status_icon, gpointer user_data)
{
	gtk_window_deiconify(GTK_WINDOW(mainGUI.window));
}

#endif

gboolean msgboxKeyPress(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	if(event->type == GDK_KEY_PRESS)
	{
		if((event->keyval&GDK_Up) == GDK_Up)
		{
			
		}
	}
	return 0;
}
