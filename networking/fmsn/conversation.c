#include <gtk/gtk.h>
#include <gnet.h>
#include <stdlib.h>

#include "conversation.h"
#include "core_ssl.h"

static char msg_template[] = "MIME-Version: 1.0\r\nContent-Type: text/plain; charset=UTF-8\r\nX-MMS-IM-Format: FN=Arial; EF=; CO=0; CS=0; PF=22\r\n\r\n";

void conversation_add_msg(conv_data *convs, char *nick, char *message)
{
	GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(convs->sent_msgs));
	GtkTextIter end;
	gtk_text_buffer_get_end_iter(buf,&end);

	char *composed = g_strdup_printf("<%s>: %s\n",nick, message);
	
	gtk_text_buffer_insert(buf,&end,composed,-1);
	free(composed);
}

gboolean conversation_receive(GIOChannel *source, GIOCondition condition, gpointer data)
{
	conv_data *convs = (conv_data*)data;
	if((condition&G_IO_HUP)==G_IO_HUP)
	{
		return FALSE;
	}
	if((condition&G_IO_IN) == G_IO_IN)
	{
		GIOError error = G_IO_ERROR_NONE;
		char data = 0;
		unsigned int offset = 0;
		char message[1024] = { '\0' };
		gsize n = 1;
		while(data!='\n')
		{
			error = gnet_io_channel_readn(convs->channel, &data, n, &n);
			if(n == 0)
			{
				return FALSE;
			}
			if (error != G_IO_ERROR_NONE)
			{
				error_dialog("Error receiving network data!");
				return FALSE;
			}
			message[offset++] = data;
		}
		printf("Received: %s\n",message);
		if(strncmp(message,"MSG",3) == 0) // GOT A MESSAGE :O!
		{
			char *nick = split(message,' ',2);
			char *len = split(message,' ',3);
			len[strlen(len)-2] = '\0';
			int length = atoi(len);
			printf("Inbound message length: %d\n",length);

			error = G_IO_ERROR_NONE;
			char incoming_msg[length+1];
			memset(incoming_msg, '\0',length+1);
			gsize n = length;
			error = gnet_io_channel_readn(convs->channel, incoming_msg, n, &n);
			if(error != G_IO_ERROR_NONE)
			{
				fprintf(stderr, "Error occurred reading message\n");
				return FALSE;
			}
			char *msg = incoming_msg;
			int i = 0;
			while(i<4)
			{
				msg = index(msg,'\n')+1;
				i++;
				if(i == 2)
				{
					if(strncmp(msg,"TypingUser",10) == 0)
					{
						// Just a user typing, do something with this later...
						return TRUE;
					}
				}
				if(msg==1)
				{
					fprintf(stderr, "Error: Couldn't find newline in msg block\n");
					break;
				}
			}

			printf("Message: %s\n",incoming_msg);
			conversation_add_msg(convs,nick, msg);
		}
	}
	return TRUE;
}

void conversation_msg_activate(GtkEntry *entry, gpointer user_data)
{
	conv_data *convs = (conv_data*)user_data;
	char *msg_text = (char*)gtk_entry_get_text(entry);
	
	char *send_msg = g_strdup_printf("MSG %d N %d\r\n%s%s",convs->msg_count++,strlen(msg_template)+strlen(msg_text),msg_template,msg_text);

	printf("Sending: %s\n",send_msg);
	GIOError error = G_IO_ERROR_NONE;
	gsize n = strlen(send_msg);
	error = gnet_io_channel_writen(convs->channel, send_msg, n, &n);
	if (error != G_IO_ERROR_NONE)
	{
		fprintf(stderr,"Data couldn't be sent: %s\n",send_msg);
		return;
	}

	g_free(send_msg);
	
	conversation_add_msg(convs,"Me",msg_text);
	gtk_entry_set_text(entry,"");
}

void conversation_new(char *host, char *session, char *auth_str, char *email, char *nick, char *uemail)
{
	conv_data *convs = g_malloc(sizeof(conv_data));
	convs->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	GtkWidget *vbox = gtk_vbox_new(0,0);
	gtk_container_add(GTK_CONTAINER(convs->window),vbox);

	convs->msg_count = 2;

	convs->sent_msgs = gtk_text_view_new();

	GtkWidget *scroll_view = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_view),GTK_POLICY_NEVER,GTK_POLICY_ALWAYS);
	gtk_container_add(GTK_CONTAINER(scroll_view),convs->sent_msgs);

	convs->msg = gtk_entry_new_with_max_length(256);

	gtk_box_pack_start(GTK_BOX(vbox),scroll_view,1,1,0);
	gtk_box_pack_start(GTK_BOX(vbox),convs->msg,0,0,0);

	g_signal_connect(G_OBJECT(convs->msg),"activate", G_CALLBACK(conversation_msg_activate), convs);

   GInetAddr* addr;
   addr = gnet_inetaddr_new(host, 1863);
	
   if(addr)
	{
		convs->socket = gnet_tcp_socket_new(addr);
		gnet_inetaddr_delete(addr);
	}
	else
	{
		fprintf(stderr,"Failed to resolve %s\n",host);
		return;
	}

	convs->channel = gnet_tcp_socket_get_io_channel(convs->socket);
   g_io_channel_set_buffer_size(convs->channel,128);
   g_io_channel_set_buffered(convs->channel,1);
   g_io_channel_set_encoding(convs->channel, NULL, NULL);
   g_io_add_watch(convs->channel,G_IO_IN|G_IO_HUP|G_IO_ERR,(GIOFunc)conversation_receive,convs);

	char *reply = g_strdup_printf("ANS 1 %s %s %s\r\n",uemail, auth_str, session);
	printf("Sending: %s\n",reply);
	GIOError error = G_IO_ERROR_NONE;
	gsize n = strlen(reply);
	error = gnet_io_channel_writen(convs->channel, reply, n, &n);
	if (error != G_IO_ERROR_NONE)
	{
		fprintf(stderr,"Data couldn't be sent: %s\n",reply);
		return;
	}
	
	char *window_title = g_strdup_printf("%s <%s>", nick, email);

	gtk_window_set_title(GTK_WINDOW(convs->window), window_title);

	g_free(window_title);

	gtk_widget_show_all(convs->window);
}
