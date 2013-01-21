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

#include "helper.h"
#include "globals.h"
#include <math.h>

/*
	Definition of helper functions
*/

void showWelcome()
{
	//Introduction function (shows how to use the program on the canvas)
	cairo_t *cr = gdk_cairo_create(mainGUI.drwArea->window);
	cairo_select_font_face(cr, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_move_to(cr,13,0);
	cairo_line_to(cr,13,90);
	cairo_move_to(cr,5,100);
	cairo_set_font_size(cr,12);
	cairo_show_text(cr, "(Dis)Connect");
	
	cairo_move_to(cr,47,0);
	cairo_line_to(cr,47,70);
	cairo_move_to(cr,20,80);
	cairo_show_text(cr, "Change ID");

	cairo_move_to(cr,95,0);
	cairo_line_to(cr,95,110);
	cairo_move_to(cr,60,120);
	cairo_show_text(cr, "Stroke Width");
	
	cairo_move_to(cr,147,0);
	cairo_line_to(cr,147,70);
	cairo_move_to(cr,115,80);
	cairo_show_text(cr, "Grid Settings");
	
	cairo_move_to(cr,180,0);
	cairo_line_to(cr,180,40);
	cairo_move_to(cr,160,50);
	cairo_show_text(cr, "Colours");
	
	cairo_move_to(cr,220,0);
	cairo_line_to(cr,220,60);
	cairo_move_to(cr,200,70);
	cairo_show_text(cr, "Brush");
	
	cairo_move_to(cr,250,0);
	cairo_line_to(cr,250,80);
	cairo_move_to(cr,230,90);
	cairo_show_text(cr, "Eraser");
	
	cairo_move_to(cr,280,0);
	cairo_line_to(cr,280,100);
	cairo_move_to(cr,255,110);
	cairo_show_text(cr, "Line Tool");
	
	cairo_move_to(cr,310,0);
	cairo_line_to(cr,310,120);
	cairo_move_to(cr,290,130);
	cairo_show_text(cr, "Rectangle");
	
	cairo_move_to(cr,352,0);
	cairo_line_to(cr,352,20);
	cairo_move_to(cr,335,30);
	cairo_show_text(cr, "Clear");
	
	cairo_move_to(cr,380,0);
	cairo_line_to(cr,380,40);
	cairo_move_to(cr,340,50);
	cairo_show_text(cr, "Dump a .png");
	
	cairo_move_to(cr,423,0);
	cairo_line_to(cr,423,60);
	cairo_move_to(cr,403,70);
	cairo_show_text(cr, "About");
	
	cairo_set_font_size(cr,20);
	cairo_move_to(cr,10,170);
	cairo_show_text(cr,"Welcome!");
	cairo_set_font_size(cr,12);
	cairo_move_to(cr,10,200);
	cairo_show_text(cr,"Welcome to Nonlogic Pad. This screen will familiarise you with the interface.");
	cairo_move_to(cr,10,215);
	cairo_show_text(cr,"Look at the labels and click the respective buttons.");
	cairo_move_to(cr,10,240);
	cairo_show_text(cr,"The connect and clear button will remove this screen!");
	cairo_move_to(cr,10,255);
	cairo_show_text(cr,"Before you do this, try using the tools, change the colour of your ink, and draw a face.");
	
	cairo_move_to(cr,10,285);
	cairo_set_font_size(cr,20);
	cairo_show_text(cr,"Tools");
	cairo_set_font_size(cr,12);
	cairo_move_to(cr,10,305);
	cairo_show_text(cr,"Brush: The brush is a freeform tool, click and drag to draw anything you wish.");
	cairo_move_to(cr,10,320);
	cairo_show_text(cr,"Eraser: The eraser allows you to remove sections of your drawings.");
	cairo_move_to(cr,10,335);
	cairo_show_text(cr,"Line: The line tool draws perfectly straight lines, with a click, drag and release.");
	cairo_move_to(cr,10,350);
	cairo_show_text(cr,"Rectangle: The rectangle tool works likes the line tool, but the result is a rectangle.");
	cairo_move_to(cr,10,375);
	cairo_set_font_size(cr,20);
	cairo_show_text(cr,"Misc.");
	cairo_set_font_size(cr,12);
	cairo_move_to(cr,10,400);
	cairo_show_text(cr,"The grid settings only affect YOU, so other clients wont be affected by your settings.");
	cairo_move_to(cr,10,415);
	cairo_show_text(cr,"If you turn on 'Snap to Grid' the line and rectangle tools will stick to the grid.");
	cairo_move_to(cr,10,430);
	cairo_show_text(cr,"This makes it easy to draw accurate diagrams, or a noughts and crosses board!");
	cairo_move_to(cr,10,455);
	cairo_show_text(cr,"Below is the text chat window, if you need to talk to some one you can do it by");
	cairo_move_to(cr,10,470);
	cairo_show_text(cr,"entering your message in the bottom box and then hitting enter.");
	
	cairo_move_to(cr,10,490);
	cairo_show_text(cr,"Please check the 'About' dialog for more information on Nonlogic Pad.");

	cairo_fill_preserve(cr);
	cairo_stroke(cr);
	globals.image = gdk_drawable_get_image(mainGUI.drwArea->window,0,0,mainGUI.drwArea->allocation.width,mainGUI.drwArea->allocation.height);
}

void flushCanvas()
{
	GdkRectangle rect;
	rect.x = 0;
	rect.y = 0;
	rect.width = mainGUI.drwArea->allocation.width;
	rect.height = mainGUI.drwArea->allocation.height;
	GdkGC *gc = gdk_gc_new(GDK_DRAWABLE(mainGUI.drwArea->window));
	GdkColor myCol;
	myCol.red = 65535;
	myCol.green = 65535;
	myCol.blue = 65535;
	myCol.pixel = 0xffffff;
	//gdk_gc_set_background(gc,&myCol);
	gdk_gc_set_foreground(gc,&myCol);
	gdk_draw_rectangle(GDK_DRAWABLE(mainGUI.drwArea->window),gc,1,rect.x,rect.y,rect.width,rect.height);
	globals.image = gdk_drawable_get_image(GDK_DRAWABLE(mainGUI.drwArea->window),0,0,rect.width,rect.height);
	gdk_window_invalidate_rect(mainGUI.drwArea->window,&rect,0);
}

void flushCanvasArea(int32_t x, int32_t y, int32_t width, int32_t height)
{
	GdkRectangle rect;
	rect.x = x;
	rect.y = y;
	rect.width = width;
	rect.height = height;
	gdk_window_invalidate_rect(mainGUI.drwArea->window,&rect,0);	
}

void addLineD(int32_t id,int32_t x, int32_t y, int32_t x2, int32_t y2, int32_t r, int32_t g, int32_t b, unsigned char width, unsigned char alpha)
{
	cairo_t *cr = gdk_cairo_create(mainGUI.drwArea->window);
	cairo_set_operator(cr,CAIRO_OPERATOR_OVER);
	cairo_set_line_cap(cr,CAIRO_LINE_CAP_ROUND);
	cairo_set_source_rgba(cr, r/65535.0, g/65535.0, b/65535.0,alpha/255.0);
	cairo_set_line_width(cr, width);
	cairo_move_to(cr,x,y);
	cairo_line_to(cr,x2,y2);
	cairo_stroke(cr);
	cairo_destroy(cr);
	
	gdk_drawable_copy_to_image(mainGUI.drwArea->window,globals.image,x,y,x,y,x2-x,y2-y);
}

void addRectD(int32_t id,int32_t x, int32_t y, int32_t x2, int32_t y2, int32_t r, int32_t g, int32_t b, unsigned char width, unsigned char alpha)
{
	cairo_t *cr = gdk_cairo_create(mainGUI.drwArea->window);
	cairo_set_line_cap(cr,CAIRO_LINE_CAP_ROUND);
	cairo_set_source_rgb(cr, r/65535.0, g/65535.0, b/65535.0);
	cairo_set_line_width(cr, width);
	cairo_move_to(cr,x,y);
	cairo_line_to(cr,x2,y);
	cairo_line_to(cr,x2,y2);
	cairo_line_to(cr,x,y2);
	cairo_line_to(cr,x,y);
	cairo_stroke(cr);
	cairo_destroy(cr);
}

void addTempLineD(int32_t x, int32_t y, int32_t x2, int32_t y2, int32_t r, int32_t g, int32_t b, char width)
{
}

void addTempRectD(int32_t x, int32_t y, int32_t x2, int32_t y2, int32_t r, int32_t g, int32_t b, char width)
{
}

void historyScrollToEnd()
{
	GtkAdjustment *spos = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(mainGUI.scrolledWin2));
	if(spos->value == spos->upper-spos->page_size)
	{
		GtkTextIter end;
		gtk_text_buffer_get_end_iter(mainGUI.historyBuf,&end);
		GtkTextMark *pos = gtk_text_buffer_create_mark(GTK_TEXT_BUFFER(mainGUI.historyBuf),NULL,&end,0);
		gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(mainGUI.history),pos,0,1,1,1);
	}
}

void addClient(char *name, int32_t id)
{
	GtkTreeIter curPos;
	gtk_list_store_insert(GTK_LIST_STORE(mainGUI.onlineLS),&curPos,0);
	gtk_list_store_set(mainGUI.onlineLS,&curPos,0,name,1,id,-1);
}

void purgeClientList() // Clears client list on disconnect
{
	gtk_list_store_clear(mainGUI.onlineLS);
	addClient(globals.nick, -1);
}

void changeName(char *nname, int32_t id)
{
	GtkTreeIter curPos;
	int32_t nid = 0;
	char found = 0;
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(mainGUI.onlineLS),&curPos);
	do
	{
		gtk_tree_model_get(GTK_TREE_MODEL(mainGUI.onlineLS),&curPos,1,&nid,-1);
		if(nid == id)
		{
			found = 1;
			break;
		}
	}while(gtk_tree_model_iter_next(GTK_TREE_MODEL(mainGUI.onlineLS),&curPos));
	if(found)
	{
		gtk_list_store_set(mainGUI.onlineLS,&curPos,0,nname,-1);
		if(id == -1)
		{
			char msg[128];
			sprintf(msg,"*** You are know known as %s",nname);
			addToHistory(msg);
		}
	}
	else
		addClient(nname,id);
}

void connectToServer(char *name, int32_t port, char *channel)
{
	GInetAddr* addr;
	gnet_init();
	addr = gnet_inetaddr_new(name, port);
	globals.tcpSock = gnet_tcp_socket_new(addr);
	gnet_inetaddr_delete(addr);
	if (!globals.tcpSock)
	{
		errorDialog("Couldn't connect to the server!");
		return;
	}
	gtk_image_set_from_file(GTK_IMAGE(gtk_tool_button_get_icon_widget(GTK_TOOL_BUTTON(mainGUI.conn))),"pixbufs/disconn.png");
	globals.connected = 1;
	addToHistory("Connected to server successfully!");
	strcpy(globals.server,name);
	globals.port = port;
	globals.sockChan = gnet_tcp_socket_get_io_channel(globals.tcpSock);
	g_io_channel_set_buffer_size(globals.sockChan,128);
	g_io_channel_set_buffered(globals.sockChan,1);
	g_io_channel_set_encoding(globals.sockChan, NULL, NULL);
	g_io_add_watch(globals.sockChan,G_IO_IN|G_IO_HUP|G_IO_ERR,(GIOFunc)sockData,NULL); 
	unsigned char data[17];
	gnet_pack("!bs",data,17,(char)0x08,globals.nick);
	sendData(data,17);
	gnet_pack("!bs",data,17,(char)0x0D,globals.channel);
	sendData(data,17);
}

void changeChannel()
{
	if(globals.connected)
	{
		purgeClientList();
		clearCanvas();
		unsigned char data[17];
		gnet_pack("!bs",data,17,(char)0x0D,globals.channel);
		sendData(data,17);
	}
}

void sendData(unsigned char *data, int32_t len)
{
	GIOError error = G_IO_ERROR_NONE;
	gsize n = len;
	error = gnet_io_channel_writen(globals.sockChan, data, n, &n); // Should check if n = 21 again :(
   	if (error != G_IO_ERROR_NONE) 
	{
		printf("Data couldn't be sent for some reason\n");
		return;
	}
	if(n!=len)
	{
		printf("Didn't send ALL the data\n");
	}
}

void errorDialog(char *msg)
{
	GtkWidget *dialog = gtk_message_dialog_new(NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,msg,0);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void addMsgHistory(int32_t id, char *msg, int32_t len)
{
	GtkTreeIter curPos;
	int32_t nid = 0;
	char *nick;
	char found = 0;
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(mainGUI.onlineLS),&curPos);
	do
	{
		gtk_tree_model_get(GTK_TREE_MODEL(mainGUI.onlineLS),&curPos,0,&nick,1,&nid,-1);
		if(nid == id)
		{
			found = 1;
			break;
		}
	}while(gtk_tree_model_iter_next(GTK_TREE_MODEL(mainGUI.onlineLS),&curPos));
	if(found)
	{
		GtkTextIter end;
		gtk_text_buffer_get_end_iter(mainGUI.historyBuf,&end);
		if(id!=-1)
			gtk_text_buffer_insert_with_tags_by_name(mainGUI.historyBuf,&end,"<",1,"blue",NULL);
		else
			gtk_text_buffer_insert_with_tags_by_name(mainGUI.historyBuf,&end,"<",1,"purple",NULL);
		gtk_text_buffer_get_end_iter(mainGUI.historyBuf,&end);
		gtk_text_buffer_insert_with_tags_by_name(mainGUI.historyBuf,&end,nick,strlen(nick),"bold",NULL);
		gtk_text_buffer_get_end_iter(mainGUI.historyBuf,&end);
		if(id!=-1)
			gtk_text_buffer_insert_with_tags_by_name(mainGUI.historyBuf,&end,">",1,"blue",NULL);
		else
			gtk_text_buffer_insert_with_tags_by_name(mainGUI.historyBuf,&end,">",1,"purple",NULL);
		gtk_text_buffer_get_end_iter(mainGUI.historyBuf,&end);
		gtk_text_buffer_insert(mainGUI.historyBuf,&end," ",1);
		gtk_text_buffer_get_end_iter(mainGUI.historyBuf,&end);
		gtk_text_buffer_insert(mainGUI.historyBuf,&end,msg,len);
		gtk_text_buffer_get_end_iter(mainGUI.historyBuf,&end);
		gtk_text_buffer_insert(mainGUI.historyBuf,&end,"\n",1);
		historyScrollToEnd();
	}
	else
		return;
}

void addActionHistory(int32_t id, char *action, int32_t len)
{
	GtkTreeIter curPos;
	int32_t nid = 0;
	char *nick;
	char found = 0;
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(mainGUI.onlineLS),&curPos);
	do
	{
		gtk_tree_model_get(GTK_TREE_MODEL(mainGUI.onlineLS),&curPos,0,&nick,1,&nid,-1);
		if(nid == id)
		{
			found = 1;
			break;
		}
	}while(gtk_tree_model_iter_next(GTK_TREE_MODEL(mainGUI.onlineLS),&curPos));
	if(found)
	{
		GtkTextIter end;
		gtk_text_buffer_get_end_iter(mainGUI.historyBuf,&end);
		if(id!=-1)
			gtk_text_buffer_insert_with_tags_by_name(mainGUI.historyBuf,&end,"*** ",4,"blue",NULL);
		else
			gtk_text_buffer_insert_with_tags_by_name(mainGUI.historyBuf,&end,"*** ",4,"purple",NULL);
		gtk_text_buffer_get_end_iter(mainGUI.historyBuf,&end);
		gtk_text_buffer_insert_with_tags_by_name(mainGUI.historyBuf,&end,nick,strlen(nick),"bold",NULL);
		gtk_text_buffer_get_end_iter(mainGUI.historyBuf,&end);
		gtk_text_buffer_get_end_iter(mainGUI.historyBuf,&end);
		gtk_text_buffer_insert(mainGUI.historyBuf,&end," ",1);
		gtk_text_buffer_get_end_iter(mainGUI.historyBuf,&end);
		gtk_text_buffer_insert(mainGUI.historyBuf,&end,action,len);
		gtk_text_buffer_get_end_iter(mainGUI.historyBuf,&end);
		gtk_text_buffer_insert(mainGUI.historyBuf,&end,"\n",1);
		historyScrollToEnd();
	}
	else
		return;
}

void addToHistory(char *message)
{
	GtkTextIter end;
	gtk_text_buffer_get_end_iter(mainGUI.historyBuf,&end);
	gtk_text_buffer_insert(mainGUI.historyBuf,&end,message,strlen(message));
	gtk_text_buffer_get_end_iter(mainGUI.historyBuf,&end);
	gtk_text_buffer_insert(mainGUI.historyBuf,&end,"\n",1);
#if GTK_MINOR_VERSION >= 10
	if(!globals.visible)
	{
		gtk_status_icon_set_from_file(mainGUI.nlIcon,"pixbufs/sicona.png");
	}
#endif
	historyScrollToEnd();
}

void clearCanvas()
{
	flushCanvas();
}

void dumpCanvas()
{
/*	cairo_surface_t *surf = cairo_image_surface_create(CAIRO_FORMAT_RGB24,500,500);
	cairo_t *clear = cairo_create(surf);
	cairo_set_source_rgb(clear,1,1,1);
	cairo_move_to(clear,0,0);
	cairo_line_to(clear,500,0);
	cairo_line_to(clear,500,500);
	cairo_line_to(clear,0,500);
	cairo_line_to(clear,0,0);
	cairo_fill(clear);
	cairo_destroy(clear);
	lineData *temp;
	temp = lineList;
	while(temp!=NULL)
	{
		cairo_t *cr = cairo_create(surf);
		cairo_set_line_cap(cr,CAIRO_LINE_CAP_ROUND);
		cairo_set_source_rgba(cr, temp->r/65535.0, temp->g/65535.0, temp->b/65535.0,temp->alpha/255.0);
		cairo_set_line_width(cr, temp->width);
		cairo_move_to(cr,temp->x,temp->y);
		cairo_line_to(cr,temp->x2,temp->y2);
		cairo_stroke(cr);
		cairo_destroy(cr);
		temp = temp->next;
	}
	GtkWidget *saveTo = gtk_file_chooser_dialog_new("Save Dump As",NULL,GTK_FILE_CHOOSER_ACTION_SAVE,GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,NULL);
	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*.png");
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(saveTo),filter);
	if(gtk_dialog_run(GTK_DIALOG(saveTo)) == GTK_RESPONSE_ACCEPT)
	{
		char *fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(saveTo));
		if(strncmp(fname+strlen(fname)-4,".png",4)!=0)
		{
			// Add .png to the end of the filename
		//	fname = strcat(fname,".png");
		}
		if(cairo_surface_write_to_png(surf,fname)!=CAIRO_STATUS_SUCCESS)
			errorDialog("Failed to dump the current drawing!");
	}
	gtk_widget_destroy(saveTo);
	free(surf);
*/
}

char *getClientName(int32_t id)
{
	GtkTreeIter curPos;
	int32_t nid = 0;
	char found = 0;
	char *nick;
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(mainGUI.onlineLS),&curPos);
	do
	{
		gtk_tree_model_get(GTK_TREE_MODEL(mainGUI.onlineLS),&curPos,0,&nick,1,&nid,-1);
		if(nid == id)
		{
			found = 1;
			break;
		}
	}while(gtk_tree_model_iter_next(GTK_TREE_MODEL(mainGUI.onlineLS),&curPos));
	if(found)
		return nick;
	else
		return 0;	
}

void removeClient(int32_t id)
{
	GtkTreeIter curPos;
	int32_t nid = 0;
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(mainGUI.onlineLS),&curPos);
	do
	{
		gtk_tree_model_get(GTK_TREE_MODEL(mainGUI.onlineLS),&curPos,1,&nid,-1);
		if(nid == id)
		{
			gtk_list_store_remove(mainGUI.onlineLS,&curPos);
			return;
		}
	}while(gtk_tree_model_iter_next(GTK_TREE_MODEL(mainGUI.onlineLS),&curPos));
	printf("Client not found\n");
}

GdkColor *createColour(int r, int g, int b)
{
	GdkColor *retCol = malloc(sizeof(GdkColor));
	retCol->red = r;
	retCol->blue = b;
	retCol->green = g;
	return retCol;
}

void drawGrid(int sepSize)
{
	register int i = 0;
	cairo_t *cr = gdk_cairo_create(mainGUI.drwArea->window);
	cairo_set_line_width(cr,0.8);
	cairo_set_source_rgb(cr, 0.8,0.8,0.8);
	for(;i<mainGUI.drwArea->allocation.width;i+=sepSize)
	{
		cairo_move_to(cr,0,i);
		cairo_line_to(cr,mainGUI.drwArea->allocation.width,i);
		cairo_move_to(cr,i,0);
		cairo_line_to(cr,i,mainGUI.drwArea->allocation.width);
	}
	cairo_stroke(cr);
	cairo_destroy(cr);
}

void changeCursor()
{
	GdkCursor *ncursor = 0;
	GdkPixbuf *brush = gdk_pixbuf_new_from_file("pixbufs/brush.png",NULL);
	GdkPixbuf *eraser = gdk_pixbuf_new_from_file("pixbufs/eraser.png",NULL);
	if(globals.toolMode == 1)
	{
		ncursor = gdk_cursor_new_from_pixbuf(gdk_display_get_default(),brush,2,18);
		
	}
	else if(globals.toolMode == 2)
	{
		ncursor = gdk_cursor_new_from_pixbuf(gdk_display_get_default(),eraser,2,18);
	}
	else if(globals.toolMode == 3)
		ncursor = gdk_cursor_new(GDK_CROSSHAIR);
	else if(globals.toolMode == 4)
		ncursor = gdk_cursor_new(GDK_CROSSHAIR);
	else
		return; // You never know
	gdk_window_set_cursor(mainGUI.drwArea->window,ncursor);
}

char *subStr(char *string, char delim, int num, size_t size)
{
	char *retString = malloc(size);
	memset(retString,'\0',size);
	int i;
	int dc = 0;
	int k = 0;
	for(i = 0;i<strlen(string);i++)
	{
		if(string[i]==delim)
		{
			dc++;
			if(k==0)
				continue;
			else
				return retString;
		}
		if(dc == num)
		{
			retString[k++] = string[i];
		}
	}
	if(retString[0] == '\0')
		return NULL;
	else
		return retString;
}
