#ifndef __CONVERSATION_H__
#define __CONVERSATION_H__

#include <gtk/gtk.h>
#include <gnet.h>

typedef struct 
{
	GtkWidget *window;
	GtkWidget *sent_msgs;
	GtkWidget *msg;
	GTcpSocket *socket;
	GIOChannel *channel;
	long msg_count;
} conv_data;

void conversation_new(char *host, char *session, char *auth_str, char *email, char *nick, char *uemail);

#endif
