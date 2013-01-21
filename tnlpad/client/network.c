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
	chat_append("*** Connected to server successfully!\n");
	globals_sock_chan = gnet_tcp_socket_get_io_channel(globals_tcp_sock);
	g_io_channel_set_buffer_size(globals_sock_chan,128);
	g_io_channel_set_buffered(globals_sock_chan,1);
	g_io_channel_set_encoding(globals_sock_chan, NULL, NULL);
	g_io_add_watch(globals_sock_chan,G_IO_IN|G_IO_HUP|G_IO_ERR,(GIOFunc)network_receive,NULL);

	unsigned char *data = malloc(18); 
	gnet_pack("!bs",(char*)data,17,(char)0x08,globals_nick);
	network_send(data,17);
	free(data);
	data = malloc(34);
	if(strlen(globals_name)>0)
	{
		gnet_pack("!bs",(char*)data,33,(char)0x11,globals_name);
		network_send(data,33);
	}
	free(data);
	data = malloc(34);
	if(strlen(globals_website)>0)
	{
		gnet_pack("!bs",(char*)data,33,(char)0x12,globals_website);
		network_send(data,33);
	}
	free(data);
	data = malloc(34);
	if(strlen(globals_email)>0)
	{
		gnet_pack("!bs",(char*)data,33,(char)0x13,globals_email);
		network_send(data,33);
	}	
	free(data);
	data = malloc(17);
	gnet_pack("!bs",(char*)data,17,(char)0x0D,globals_channel);
	network_send(data,17);
	free(data);
	data = malloc(2);
	data[0] = '\x10';
	network_send(data,1);
	free(data);
}

void network_disconnect(int code)
{
	if(globals_connected)
	{
		network_send((unsigned char*)"\x04",1);
		if(code!=-1)
			chat_append("*** Disconnected from the server\n");
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

void network_send_circle(int x, int y, int x2, int y2)
{
	if(globals_connected)
	{
		unsigned char data[30] = { 0 };
		gnet_pack("!b7ib",(char*)data,30,(char)0x1C,x,y,x2,y2,globals_red,globals_green,globals_blue,(char)(globals_stroke_width));
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
		main_window_clear_topic();
		client_clear_list();
		network_send(data,17);
	}
}

void network_send_topic(char *topic)
{
	if(globals_connected)
	{
		unsigned char data[131];
		sprintf((char*)data,"\x0E\x02%s\x03",topic);
		network_send(data,strlen(topic)+3);
	}
}

void network_send_register(char *pass)
{
	if(globals_connected)
	{
		unsigned char *data = g_malloc0(17);
		sprintf((char*)data,"\x15%s",pass);
		network_send(data,17);
	}
}

void network_send_login(char *pass)
{
	if(globals_connected)
	{
		unsigned char *data = g_malloc0(17);
		sprintf((char*)data,"\x14%s",pass);
		network_send(data,17);
	}
}

void network_send_lock_chan()
{
	if(globals_connected)
	{
		unsigned char data[2] = { '\0' };
		data[0] = '\x17';
		network_send(data,1);
	}
}

void network_send_lock_topic()
{
	if(globals_connected)
	{
		unsigned char data[2] = { '\0' };
		data[0] = '\x18';
		network_send(data,1);
	}
}

void network_send_take_voice(char id)
{
	if(globals_connected)
	{
		unsigned char data[2] = { '\0' };
		data[0] = '\x1A';
		data[1] = id;
		network_send(data,2);
	}
}

void network_send_take_ink(char id)
{
	if(globals_connected)
	{
		unsigned char data[2] = { '\0' };
		data[0] = '\x19';
		data[1] = id;
		network_send(data,2);
	}
}

void network_send_give_voice(char id)
{
	if(globals_connected)
	{
		unsigned char data[2] = { '\0' };
		data[0] = '\x1D';
		data[1] = id;
		network_send(data,2);
	}
}

void network_send_give_ink(char id)
{
	if(globals_connected)
	{
		unsigned char data[2] = { '\0' };
		data[0] = '\x1B';
		data[1] = id;
		network_send(data,2);
	}
}

gboolean network_receive(GIOChannel *source, GIOCondition condition, gpointer data)
{
	if((condition&G_IO_HUP)==G_IO_HUP)
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
	else if((condition&G_IO_IN) == G_IO_IN)
	{
		GIOError error = G_IO_ERROR_NONE;
		char header;
		char idn;
		gsize n = 1;
		error = gnet_io_channel_readn(globals_sock_chan, &header, n, &n); 
		if(n == 0)
		{
			/* Assume disconnect */
			globals_connected = 0;
			toolbar_switch_conn();
			gnet_tcp_socket_delete(globals_tcp_sock);
			error_dialog("Error receiving data - disconnecting");
			return 0;
		}
		if (error != G_IO_ERROR_NONE) 
		{
			error_dialog("Error receiving network data!");
			return 0;
		}
		
		n = 1;
		error = gnet_io_channel_readn(globals_sock_chan, &idn, n, &n); 
		if (error != G_IO_ERROR_NONE) 
		{
			error_dialog("Error receiving client ID len\n");
			return 0;
		}
		
		if(header==0x01) /* Line received */
		{
			n = 29;
			unsigned char data[29] = { 0 };
			error = gnet_io_channel_readn(globals_sock_chan, data, n, &n);
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receving line data");
				return 0;
			}		
			char stroke;
			int32_t x,y,x2,y2,r,g,b;
			gnet_unpack("!7ib",(char*)data,29,&x,&y,&x2,&y2,&r,&g,&b,&stroke);
			draw_line(x,y,x2,y2,r,g,b,stroke);
#ifdef LOG
			if(client_get_logged(idn))
			{
				char *client_nick = client_get_name(idn);
				char *log_name = g_strdup_printf("%s.log",client_nick);
				FILE *log_file = fopen(log_name,"a");
				free(client_nick);
				free(log_name);
				fprintf(log_file,"\x01%c",idn);
				fwrite(&x,1,4,log_file);
				fwrite(&y,1,4,log_file);
				fwrite(&x2,1,4,log_file);
				fwrite(&y2,1,4,log_file);
				fwrite(&r,1,4,log_file);
				fwrite(&g,1,4,log_file);
				fwrite(&b,1,4,log_file);
				fprintf(log_file,"%c\n",stroke);
				fclose(log_file);
			}
#endif
		}
		if(header==0x0A) /* Rectangle data */
		{
			n = 29;
			unsigned char data[29];
			error = gnet_io_channel_readn(globals_sock_chan, data, n, &n);
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving data\n");
				return 0;
			}		
			char stroke;
			int32_t x,y,x2,y2,r,g,b;
			gnet_unpack("!7ib",(char*)data,29,&x,&y,&x2,&y2,&r,&g,&b,&stroke);
			draw_rect(x,y,x2,y2,r,g,b,stroke);
#ifdef LOG
			if(client_get_logged(idn))
			{
				char *client_nick = client_get_name(idn);
				char *log_name = g_strdup_printf("%s.log",client_nick);
				FILE *log_file = fopen(log_name,"a");
				free(log_name);
				free(client_nick);
				fprintf(log_file,"\x0A%c%d%d%d%d%d%d%d%c\n",idn,x,y,x2,y2,r,g,b,stroke);
				fclose(log_file);
			}
#endif
		}
		if(header==0x14)
		{
			if(idn>0)
			{
				char *client_nick = client_get_name(idn);
				char *msg = g_strdup_printf("*** %s granted you ops\n",client_nick);
				chat_append(msg);
				free(client_nick);
				free(msg);
				globals_has_ops = 1;
			}
			else
			{
				globals_has_ops = 1;
				//chat_append(g_strdup_printf("*** You are now an operator\n"));
			}
		}
		
		if(header==0x1C) /* Circle data */
		{
			n = 29;
			unsigned char data[29];
			error = gnet_io_channel_readn(globals_sock_chan, data, n, &n);
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving data\n");
				return 0;
			}		
			char stroke;
			int32_t x,y,x2,y2,r,g,b;
			gnet_unpack("!7ib",(char*)data,29,&x,&y,&x2,&y2,&r,&g,&b,&stroke);
			draw_circle(x,y,x2,y2,r,g,b,stroke);
#ifdef LOG
			if(client_get_logged(idn))
			{
				char *client_nick = client_get_name(idn);
				char *log_name = g_strdup_printf("%s.log",client_nick);
				FILE *log_file = fopen(log_name,"a");
				free(log_name);
				free(client_nick);
				fprintf(log_file,"\x1C%c%d%d%d%d%d%d%d%c\n",idn,x,y,x2,y2,r,g,b,stroke);
				fclose(log_file);
			}
#endif
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
			
			n = 1;
			unsigned char curChar = 0;
			unsigned char len = 0;
			char *msg = malloc(258);
			memset(msg,'\0',258);
			while(curChar!=3)
			{
				error = gnet_io_channel_readn(globals_sock_chan, &curChar, n, &n);
				if (error != G_IO_ERROR_NONE) 
				{
					error_dialog("Error receiving message length\n");
					return 0;
				}
				if(curChar>31)
				{
					sprintf((char*)msg,"%s%c",msg,curChar);
					len++;
				}
			}
			if(idn>0)
			{
				chat_add_msg((int)idn,(char*)msg,len);
			}
			else
			{
				chat_append(g_strdup_printf("*** %s\n",(char*)msg));
			}	
			free(msg);
		}
		
		if(header==0x03) /* Client connect - Verbosely adds clients */
		{
			char nick[16];
			n = 16;
			error = gnet_io_channel_readn(globals_sock_chan, nick, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving new nick ID\n");
				return 0;
			}
			client_change(nick, (int)idn);
			char *nclient = malloc(64);
			sprintf(nclient,"*** New Client Joined: %s\n",nick);
			chat_append(nclient);
			free(nclient);
		}

		if(header==0x04) /* Client Disconnect */
		{
			char *msg = malloc(128);
			if(idn>0)
			{
				char *client_nick = client_get_name(idn);
				sprintf(msg, "*** Client Quit: %s\n",client_nick);
				free(client_nick);
			}
			else
			{
				gnet_tcp_socket_delete(globals_tcp_sock);
		
				globals_connected = 0;
				toolbar_switch_conn();
				return 0;
			}
			chat_append(msg);
			client_remove((int)idn);
			free(msg);
		}
		if(header==0x05) /* Client Changed channel */
		{
			char *msg = malloc(128);
			char *nick = client_get_name(idn);
			sprintf(msg, "*** Client Left the Channel: %s\n",nick);;
			chat_append(msg);
			client_remove((int)idn);
			free(msg);
			free(nick);
		}
		if(header==0x06) /* Board Cleanup */
		{
			char *msg = malloc(128);
			char *client_nick = client_get_name(idn);
			sprintf(msg,"*** %s cleared the board\n",client_nick);
			chat_append(msg);
			draw_clear_buffers();
			free(msg);
			free(client_nick);
#ifdef LOG
			if(client_get_logged(idn))
			{
				char *client_nick = client_get_name(idn);
				char *log_name = g_strdup_printf("%s.log",client_nick);
				FILE *log_file = fopen(log_name,"a");
				free(log_name);
				free(client_nick);
				fprintf(log_file,"\x06%c\n",idn);
				fclose(log_file);
			}
#endif
		}
		/* 0x07 unused now */
		if(header==0x09) /* Name change */
		{
			char nname[16];
			n = 16;
			error = gnet_io_channel_readn(globals_sock_chan, nname, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving new nick len\n");
				return 0;
			}
			char *msg;
			char *client_nick = client_get_name(idn);
			if(client_nick!=NULL)
			{
				if(idn!=-1)
				{
					msg = g_strdup_printf("*** %s changed name to %s\n",client_nick,nname);
					free(client_nick);
				}
				else
				{
					msg = g_strdup_printf("*** Server changed your name to %s\n",nname);
					strcpy(globals_nick,nname);
				}
				chat_append(msg);
			}
			client_change(nname, (int)idn);
			free(msg);
		}
		if(header==0x0B) /* Bucket Fill */
		{
			n = 20;
			unsigned char data[20] = { 0 };
			error = gnet_io_channel_readn(globals_sock_chan, data, n, &n);
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving data\n");
				return 0;
			}
			int32_t x,y,r,g,b;
			gnet_unpack("!5i",(char*)data,20,&x,&y,&r,&g,&b);
			int w, h;
			gdk_drawable_get_size(main_window_canvas_win,&w,&h);
			if(x < w && y < h)
			{
				draw_flood_fill(x,y,draw_16_to_8(r),draw_16_to_8(g),draw_16_to_8(b));
#ifdef LOG
				if(client_get_logged(idn))
				{
					char *client_nick = client_get_name(idn);
					char *log_name = g_strdup_printf("%s.log",client_nick);
					FILE *log_file = fopen(log_name,"a");
					free(log_name);
					free(client_nick);
					fprintf(log_file,"\x0B%c",idn);
					fwrite(&x,1,4,log_file);
					fwrite(&y,1,4,log_file);
					fwrite(&r,1,4,log_file);
					fwrite(&g,1,4,log_file);
					fwrite(&b,1,4,log_file);
					fprintf(log_file,"\n");
					fclose(log_file);
				}
#endif
			}
		}
		if(header==0x0C) /* Action received */
		{
			n = 1;
			unsigned char curChar = 0;
			unsigned char len = 0;
			char *msg = malloc(258);
			memset(msg,'\0',258);
			while(curChar!=3)
			{
				error = gnet_io_channel_readn(globals_sock_chan, &curChar, n, &n);
				if (error != G_IO_ERROR_NONE) 
				{
					error_dialog("Error receiving msg len\n");
					return 0;
				}
				if(curChar>31)
				{
					sprintf((char*)msg,"%s%c",msg,curChar);
					len++;
				}
			}
			if(idn!=0)
				chat_add_action((int)idn,(char*)msg,len);
			free(msg);
		}
		if(header==0x0E) /* Topic received */
		{
			n = 1;
			unsigned char curChar = 0;
			unsigned char len = 0;
			char *msg = malloc(130);
			memset(msg,'\0',130);
			while(curChar!=3)
			{
				error = gnet_io_channel_readn(globals_sock_chan, &curChar, n, &n);
				if (error != G_IO_ERROR_NONE) 
				{
					error_dialog("Error receiving topic length\n");
					return 0;
				}
				if(curChar>31)
				{
					sprintf((char*)msg,"%s%c",msg,curChar);
					len++;
				}
			}
			char *change_message;
			if(idn!=-1)
			{
				char *client_nick = client_get_name(idn);
				change_message = g_strdup_printf("*** Topic was changed by %s\n",client_nick);
				free(client_nick);
			}
			else
				change_message = g_strdup_printf("*** Received Topic from Channel\n");
				
			chat_append(change_message);
			gtk_entry_set_text(GTK_ENTRY(main_window_topic),(char*)msg);
			free(change_message);
			free(msg);
		}
		if(header==0x0F) /* Client list update - Silently adds users*/
		{
			char nick[16];
			n = 16;
			error = gnet_io_channel_readn(globals_sock_chan, nick, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving new nick ID\n");
				return 0;
			}
			client_change(nick, (int)idn);
		}
		if(header==0x11) /* REAL Name change */
		{
			char nname[33];
			n = 32;
			error = gnet_io_channel_readn(globals_sock_chan, nname, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving new name\n");
				return 0;
			}
			client_set_name(nname, (int)idn);
		}
		if(header==0x12) /* Website change */
		{
			char nname[33];
			n = 32;
			error = gnet_io_channel_readn(globals_sock_chan, nname, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving new name\n");
				return 0;
			}
			client_set_website(nname, (int)idn);
		}
		
		if(header==0x13) /* Email change */
		{
			char nname[33];
			n = 32;
			error = gnet_io_channel_readn(globals_sock_chan, nname, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving new name\n");
				return 0;
			}
			client_set_email(nname, (int)idn);
		}
		if(header==0x1E) /* Client list update - Silently adds users*/
		{
			char channel[16];
			n = 16;
			error = gnet_io_channel_readn(globals_sock_chan, channel, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving new channel\n");
				return 0;
			}
			strcpy(globals_channel,channel);
			char *msg = g_strdup_printf("*** Joined %s\n",channel);
			chat_append(msg);
			free(msg);
		}
		if(header==0x1F) /* Client had their status changed */
		{
			n = 1;
			char status = 0;
			error = gnet_io_channel_readn(globals_sock_chan, &status, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving new nick ID\n");
				return 0;
			}
			char cur_status = client_get_status(-1);
			if(cur_status == -1)
			{
				fprintf(stderr, "Couldn't find the client to set the status to 1FS!\n");
				return 1; // The show can still go on
			}
			client_set_status(-1,(int)status);
			if(idn>0)
			{
				if((status&USER_INK)==USER_INK && (cur_status&USER_INK)==0)
				{
					// It's an important message, so I'll write it in my own ink
					char *client_nick = client_get_name(idn);
					char *msg = g_strdup_printf("*** %s gave you ink\n",client_nick);
					chat_append(msg);
					free(client_nick);
					free(msg);
					globals_has_ink = 1;
					main_window_signals_on();
				}
				if((status&USER_INK)==0 && (cur_status&USER_INK)==USER_INK)
				{
					// No more ink for mr rapid fire
					char *client_nick = client_get_name(idn);
					char *msg = g_strdup_printf("*** %s took away your ink\n",client_nick);
					chat_append(msg);
					free(client_nick);
					free(msg);
					globals_has_ink = 0;
					main_window_signals_off();
					/* Maybe just remove call backs to action on mouse pressing on the canvas? */
				}
				if((status&USER_VOICE)==USER_VOICE && (cur_status&USER_VOICE)==0)
				{
					// SPREAD THE WORD
					char *client_nick = client_get_name(idn);
					char *msg =g_strdup_printf("*** %s gave you voice\n",client_nick);
					chat_append(msg);
					free(client_nick);
					free(msg);
					globals_has_voice = 1;
				}
				if((status&USER_VOICE)==0 && (cur_status&USER_VOICE)==USER_VOICE)
				{
					//First he giveth, then he taketh away
					char *client_nick = client_get_name(idn);
					char *msg = g_strdup_printf("*** %s took away your voice\n",client_nick);
					chat_append(msg);
					free(client_nick);
					free(msg);
					globals_has_voice = 0;
				}
			}
			else
			{
				if((status&USER_INK)==USER_INK)
				{
					globals_has_ink = 1;
					main_window_signals_on();
				}
				if((status&USER_INK)==0)
				{
					globals_has_ink = 0;
					main_window_signals_off();
				}
				if((status&USER_VOICE)==USER_VOICE)
				{
					globals_has_voice = 1;
				}
				if((status&USER_VOICE)==0)
				{
					globals_has_voice = 0;
				}
			}
		}
		if(header==0x20) /* Another Client had their status changed by some one*/
		{
			n = 1;
			char by_who = 0;
			error = gnet_io_channel_readn(globals_sock_chan, &by_who, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Failed finding out who set someone's status\n");
				return 0;
			}
			
			n = 1;
			char status = 0;
			error = gnet_io_channel_readn(globals_sock_chan, &status, n, &n); 
			if (error != G_IO_ERROR_NONE) 
			{
				error_dialog("Error receiving new nick ID\n");
				return 0;
			}
			
			char cur_status = client_get_status(idn);
			if(cur_status == -1)
			{
				fprintf(stderr, "Couldn't find the client to set the status to 20! %d %d\n", idn, status);
				return 1; // The show can still go on
			}
			client_set_status(idn,(int)status);
			if(by_who>0)
			{
				char *client_nick = client_get_name(idn);
				char *client_whom = client_get_name(by_who);
				char *msg;

				if((status&USER_INK)==USER_INK && (cur_status&USER_INK)==0)
				{
					// It's an important message, so I'll write it in my own ink
					msg = g_strdup_printf("*** %s was given ink by %s\n",client_nick,client_whom);
					chat_append(msg);
				}
				if((status&USER_INK)==0 && (cur_status&USER_INK)==USER_INK)
				{
					// No more ink for mr rapid fire
					msg = g_strdup_printf("*** %s had ink taken away by %s\n",client_nick, client_whom);
					chat_append(msg);
					/* Maybe just remove call backs to action on mouse pressing on the canvas? */
				}
				if((status&USER_VOICE)==USER_VOICE && (cur_status&USER_VOICE)==0)
				{
					// SPREAD THE WORD
					msg = g_strdup_printf("*** %s was given voice by %s\n",client_nick,client_whom);
					chat_append(msg);
				}
				if((status&USER_VOICE)==0 && (cur_status&USER_VOICE)==USER_VOICE)
				{
					//First he giveth, then he taketh away
					msg = g_strdup_printf("*** %s had voice taken away by %s\n",client_nick,client_whom);
					chat_append(msg);
				}
				free(msg);
				free(client_nick);
				free(client_whom);
			}
		}
	}
	return 1;
}
