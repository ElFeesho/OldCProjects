#include <gtk/gtk.h>
#include <gnet.h>
#include <stdlib.h>
#include <string.h>
#include "main_window.h"
#include "globals.h"
#include "toolbar.h"
#include "chat.h"
#include "client.h"
#include "draw.h"
#include "error.h"

static void network_send(unsigned char *data, int len);
static gboolean network_receive(GIOChannel *source, GIOCondition condition, gpointer data);

void network_connect()
{
	GInetAddr* addr;
	gnet_init();
	addr = gnet_inetaddr_new(globals_server, globals_port);
	if(addr)
	{
		globals_tcp_sock = gnet_tcp_socket_new(addr);
		gnet_inetaddr_delete(addr);
	}
	else
	{
		error_dialog("Couldn't resolve the address, is it correct?");
		return;
	}
	if (!globals_tcp_sock)
	{
		error_dialog("Couldn't connect to the server!");
		return;
	}
	globals_connected = 1;
	toolbar_switch_conn();
	chat_append("Connected to server successfully!\n");
	globals_sock_chan = gnet_tcp_socket_get_io_channel(globals_tcp_sock);
	g_io_channel_set_buffer_size(globals_sock_chan,128);
	g_io_channel_set_buffered(globals_sock_chan,1);
	g_io_channel_set_encoding(globals_sock_chan, NULL, NULL);
	g_io_add_watch(globals_sock_chan,G_IO_IN|G_IO_HUP|G_IO_ERR,(GIOFunc)network_receive,NULL);
	unsigned char data[17];
	gnet_pack("!bs",(char*)data,17,(char)0x08,globals_nick);
	network_send(data,17);
	gnet_pack("!bs",(char*)data,17,(char)0x0D,globals_channel);
	network_send(data,17);
}

void network_disconnect(int code)
{
	if(globals_connected)
	{
		network_send((unsigned char*)"\x04",1);
		gnet_tcp_socket_delete(globals_tcp_sock);
		globals_connected = 0;
		if(code!=-1)
			chat_append("Disconnected from the server\n");
		client_clear_list();
	}
}

void network_send(unsigned char *data, int len)
{
	GIOError error = G_IO_ERROR_NONE;
	gsize n = len;
	error = gnet_io_channel_writen(globals_sock_chan, data, n, &n);
	if (error != G_IO_ERROR_NONE)
	{
		error_dialog("Data couldn't be sent");
		return;
	}
	if(n!=len)
	{
		error_dialog("Didn't send ALL the data");
	}
}

void network_send_line(int x, int y, int x2, int y2)
{
	if(globals_connected)
	{
		unsigned char data[30];
		gnet_pack("!b7ib",(char*)data,30,(char)0x01,x,y,x2,y2,globals_red,globals_green,globals_blue,(char)(globals_stroke_width));
		network_send(data,30);
	}
}

void network_send_erase(int x, int y, int x2, int y2)
{
	if(globals_connected)
	{
		unsigned char data[30];
		gnet_pack("!b7ib",(char*)data,30,(char)0x01,x,y,x2,y2,65535,65535,65535,(char)(globals_stroke_width));
		network_send(data,30);
	}
}

void network_send_rect(int x, int y, int x2, int y2)
{
	if(globals_connected)
	{
		unsigned char data[30] = { 0 };
		gnet_pack("!b7ib",(char*)data,30,(char)0x0A,x,y,x2,y2,globals_red,globals_green,globals_blue,(char)(globals_stroke_width));
		network_send(data,30);
	}
}

void network_send_fill(int x, int y)
{
	if(globals_connected)
	{
		unsigned char data[21] = { 0 };
		gnet_pack("!b5i",(char*)data,21,(char)0x0B,x,y,globals_red,globals_green,globals_blue);
		network_send(data,21);
	}
}

void network_send_message(char *message)
{
	if(globals_connected)
	{
		unsigned char data[258] = { 0 };
		unsigned char len = strlen(message);
		sprintf((char*)data,"\x02\x02%s\x03",message);
		network_send(data,len+3);
	}
}

void network_send_action(char *action)
{
	if(globals_connected)
	{
		unsigned char data[258] = { 0 };
		unsigned char len = strlen((action));
		sprintf((char*)data,"\x0C\x02%s\x03",(action));
		network_send(data,len+3);
	}
}

void network_send_nick()
{
	if(globals_connected)
	{
		unsigned char data[17] = { 0 };
		gnet_pack("!bs",(char*)data,17,(char)0x09,globals_nick);
		network_send(data,17);
	}
}

void network_send_clear()
{
	if(globals_connected)
	{
		network_send((unsigned char*)"\x06",1);
	}
}

void network_send_channel()
{
	if(globals_connected)
	{
		unsigned char data[17];
		gnet_pack("!bs",(char*)data,17,(char)0x0D,globals_channel);
		draw_clear_buffers();
		client_clear_list();
		network_send(data,17);
	}
}

void network_send_topic(char *topic)
{
	if(globals_connected)
	{
		unsigned char data[129];
		gnet_pack("!bs",(char*)data,129,(char)0x0E,topic);
	}
}

gboolean network_receive(GIOChannel *source, GIOCondition condition, gpointer data)
{
	if((condition&G_IO_IN) == G_IO_IN)
	{
		GIOError error = G_IO_ERROR_NONE;
		char header;
		gsize n = 1;
		error = gnet_io_channel_readn(globals_sock_chan, &header, n, &n); 
		if(n == 0)
		{
			/* Assume disconnect */
			globals_connected = 0;
			toolbar_switch_conn();
			gnet_tcp_socket_delete(globals_tcp_sock);
			return 0;
		}
		if (error != G_IO_ERROR_NONE) 
		{
			error_dialog("Error receiving network data!");
			return 0;
		}

		if(header==0x01) /* Line received */
		{
			char idn[3]; /* The client id of who sent the line   */
			int idi;     /* The integer version of the client id */
			n = 3;
			error = gnet_io_channel_readn(globals_sock_chan, idn, n, &n);
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving line data\n");
				return 1;
			}		
			
			idi = atoi(idn);
			
			n = 29;
			unsigned char data[29] = { 0 };
			error = gnet_io_channel_readn(globals_sock_chan, data, n, &n);
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receving line data");
				return 1;
			}		
			char stroke;
			int32_t x,y,x2,y2,r,g,b;
			gnet_unpack("!7ib",(char*)data,29,&x,&y,&x2,&y2,&r,&g,&b,&stroke);
			draw_line(x,y,x2,y2,r,g,b,stroke);
		}
		if(header==0x0A) /* Rectangle data */
		{
			char idn[3];
			int idi;
			n = 3;
			error = gnet_io_channel_readn(globals_sock_chan, idn, n, &n);
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving rectangle data");
				return 1;
			}		
			
			idi = atoi(idn);
			
			n = 29;
			unsigned char data[29];
			error = gnet_io_channel_readn(globals_sock_chan, data, n, &n);
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving data\n");
				return 1;
			}		
			char stroke;
			int32_t x,y,x2,y2,r,g,b;
			gnet_unpack("!7ib",(char*)data,29,&x,&y,&x2,&y2,&r,&g,&b,&stroke);
			draw_rect(x,y,x2,y2,r,g,b,stroke);
		}
		if(header==0x02) /* Message received */
		{
			/* How Messages Work ~~~
			 * 
			 * When the client sends a message to the server,
			 * the client packs the text between two untypeable
			 * characters, 0x02 and 0x03. If you read man ascii
			 * you can find out what they actually mean, I've forgotten
			 * but I chose them for a good reason. So now when the 
			 * client receives a message, it has to find out who it's
			 * from (first 3 bytes of the data after the header is
			 * the sender's ID which can be cross checked from the
			 * current client list), then the first 0x02 byte has to
			 * be swallowed up (yum). Then it's a case of adding a
			 * character at a time till you get to the end (0x03) then
			 * the message is added to the chat box. If the sender's 
			 * id is 0, then it is a server message (client disconnect
			 * etc).
			 */
			
			char idn[3];
			int32_t idi;
			unsigned char *msg = 0;
			n = 3;

			error = gnet_io_channel_readn(globals_sock_chan, idn, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving messenger ID\n");
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
				error = gnet_io_channel_readn(globals_sock_chan, &curChar, n, &n);
				if (error != G_IO_ERROR_NONE) 
				{
					error_dialog("Error receiving message length\n");
					return 1;
				}
				if(curChar>31)
				{
					sprintf((char*)msg,"%s%c",msg,curChar);
					len++;
				}
			}
			if(idi!=0)
				chat_add_msg(idi,(char*)msg,len);
			else
				chat_append((char*)msg);
		}
		
		if(header==0x03) /* Client connect */
		{
			char idn[3];
			n = 3;
			error = gnet_io_channel_readn(globals_sock_chan, idn, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving new user ID\n");
				return 1;
			}
			char nick[16];
			n = 16;
			error = gnet_io_channel_readn(globals_sock_chan, nick, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving new nick ID\n");
				return 1;
			}
			client_change(nick, atoi(idn));
			char *nclient = malloc(64);
			sprintf(nclient,"*** New Client Joined: %s\n",nick);
			chat_append(nclient);
			free(nclient);
		}

		if(header==0x04) /* Client Disconnect */
		{
			char idn[3];
			n = 3;
			error = gnet_io_channel_readn(globals_sock_chan, idn, n, &n);
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving user ID\n");
				return 1;
			}
			int32_t idi = atoi(idn);
			char *msg = malloc(128);
			sprintf(msg, "*** Client Quit: %s\n",client_get_name(idi));
			chat_append(msg);
			client_remove(idi);
		}
		if(header==0x05) /* Client Changed channel */
		{
			char idn[3];
			n = 3;
			error = gnet_io_channel_readn(globals_sock_chan, idn, n, &n);
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving user ID\n");
				return 1;
			}
			int32_t idi = atoi(idn);
			char *msg = malloc(128);
			sprintf(msg, "*** Client Left the Channel: %s\n",client_get_name(idi));
			chat_append(msg);
			client_remove(idi);
		}
		if(header==0x06) /* Board Cleanup */
		{
			char idn[3];
			n = 3;
			error = gnet_io_channel_readn(globals_sock_chan, idn, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving client id\n");
				return 1;
			}
			int32_t idi = atoi(idn);
			char *msg = malloc(128);
			sprintf(msg,"*** %s cleared the board\n",client_get_name(idi));
			chat_append(msg);
			draw_clear_buffers();
		}
		/* 0x07 unused now */
		if(header==0x09) /* Name change */
		{
			char idn[3];
			n = 3;
			error = gnet_io_channel_readn(globals_sock_chan, idn, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving client ID len\n");
				return 1;
			}
			int32_t idi = atoi(idn);
			char nname[16];
			n = 16;
			error = gnet_io_channel_readn(globals_sock_chan, nname, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving new nick len\n");
				return 1;
			}
			char *msg = malloc(128);
			if(client_get_name(idi)!=NULL)
			{
				if(idi!=-1)
					sprintf(msg,"*** %s changed name to %s\n",client_get_name(idi),nname);
				else
				{
					sprintf(msg,"*** Server changed your name to %s\n",nname);
					strcpy(globals_nick,nname);
				}
				chat_append(msg);
			}
			client_change(nname, idi);
		}
		if(header==0x0B) /* Bucket Fill */
		{
			char idn[3];
			int idi;
			n = 3;
			error = gnet_io_channel_readn(globals_sock_chan, idn, n, &n);
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving bucket data");
				return 1;
			}		
			
			idi = atoi(idn);
			
			n = 20;
			unsigned char data[20] = { 0 };
			error = gnet_io_channel_readn(globals_sock_chan, data, n, &n);
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving data\n");
				return 1;
			}
			int32_t x,y,r,g,b;
			gnet_unpack("!5i",(char*)data,20,&x,&y,&r,&g,&b);
			draw_flood_fill(x,y,draw_16_to_8(r),draw_16_to_8(g),draw_16_to_8(b));
		}
		if(header==0x0C) /* Action received */
		{
			char idn[3];
			int32_t idi;
			unsigned char *msg = 0;

			n = 3;

			error = gnet_io_channel_readn(globals_sock_chan, idn, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving messenger ID\n");
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
				error = gnet_io_channel_readn(globals_sock_chan, &curChar, n, &n);
				if (error != G_IO_ERROR_NONE) 
				{
					error_dialog("Error receiving msg len\n");
					return 1;
				}
				if(curChar>31)
				{
					sprintf((char*)msg,"%s%c",msg,curChar);
					len++;
				}
			}
			if(idi!=0)
				chat_add_action(idi,(char*)msg,len);
		}
		if(header==0x0E) /* Topic received */
		{
			char idn[3];
			int32_t idi;
			unsigned char *msg = 0;
			n = 3;

			error = gnet_io_channel_readn(globals_sock_chan, idn, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving messenger ID\n");
				return 1;
			}

			idi = atoi(idn);

			n = 1;
			unsigned char curChar = 0;
			unsigned char len = 0;
			msg = malloc(130);
			memset(msg,'\0',130);
			while(curChar!=3)
			{
				error = gnet_io_channel_readn(globals_sock_chan, &curChar, n, &n);
				if (error != G_IO_ERROR_NONE) 
				{
					error_dialog("Error receiving topic length\n");
					return 1;
				}
				if(curChar>31)
				{
					sprintf((char*)msg,"%s%c",msg,curChar);
					len++;
				}
			}
			char *change_message = malloc(128);
			sprintf(change_message,"*** Topic was changed by %s",client_get_name(idi));
			chat_append(change_message);
			gtk_entry_set_text(GTK_ENTRY(main_window_topic),(char*)msg);
		}
		if(header==0x0F) /* Client list update */
		{
			char idn[3];
			n = 3;
			error = gnet_io_channel_readn(globals_sock_chan, idn, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving new user ID\n");
				return 1;
			}
			char nick[16];
			n = 16;
			error = gnet_io_channel_readn(globals_sock_chan, nick, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving new nick ID\n");
				return 1;
			}
			client_change(nick, atoi(idn));
		}

	}
	else if((condition&G_IO_HUP)==G_IO_HUP)
	{
		chat_append("*** Server closed the connection!\n");
		error_dialog("Server closed the connection!");
		globals_connected = 0;
		gnet_tcp_socket_delete(globals_tcp_sock);
		toolbar_switch_conn();
		client_clear_list();
		return 0;
	}
	else if((condition&G_IO_ERR)==G_IO_ERR)
	{
		chat_append("*** Connection error, closing conection!\n");
		error_dialog("Connection error, closing conection!");
		globals_connected = 0;
		gnet_tcp_socket_delete(globals_tcp_sock);
		toolbar_switch_conn();
		client_clear_list();
		return 0;
	}
	return 1;
}
