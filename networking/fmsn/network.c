#include <gtk/gtk.h>
#include <gnet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/ssl.h>

#include "network.h"
#include "mwin.h"
#include "error.h"
#include "core_ssl.h"
#include "globals.h"

/* Local global variables */
static gboolean connected = FALSE;
static GTcpSocket *msn_socket = NULL;
static GIOChannel *msn_channel = NULL;
static char *uemail = NULL;
static char *upass = NULL;

static int msg_num = 6;
static int log_level = 0;

/* Callback prototypes */
static gboolean network_receive(GIOChannel *source, GIOCondition condition, gpointer data);

/* Helper function prototypes */

static char *net_ssl_auth(char *orig_ticket);

void net_send(unsigned char *data)
{
	printf("Sending: %s\n",data);
	GIOError error = G_IO_ERROR_NONE;
	gsize n = strlen(data);
	error = gnet_io_channel_writen(msn_channel, data, n, &n);
	if (error != G_IO_ERROR_NONE)
	{
		error_dialog("Data couldn't be sent");
		return;
	}
}

void net_send_nick(char *nick)
{
	printf("Setting Nick to: %s\n",nick);
	char *request = g_strdup_printf("REA %d %s %s\r\n",msg_num++, globals_email, nick);
	net_send(request);
}

void net_close()
{
	if(msn_channel == NULL)
		return;
	printf("Logging out\n");
	GIOError error = G_IO_ERROR_NONE;
	gsize n = 5;
	error = gnet_io_channel_writen(msn_channel, "OUT\r\n", n, &n);
	if (error != G_IO_ERROR_NONE)
	{
		error_dialog("Couldn't log out.");
		return;
	}
}

void net_login(char *email, char *password)
{
	uemail = g_strdup(email);
	upass = g_strdup(password);
	mwin_create();
	
	GInetAddr* addr;
	addr = gnet_inetaddr_new("messenger.hotmail.com", 1863);
	if(addr)
	{
		msn_socket = gnet_tcp_socket_new(addr);
		gnet_inetaddr_delete(addr);
	}
	else
	{
		error_dialog("Failed to resolve messenger.msn.com");
		return;
	}
	
	msn_channel = gnet_tcp_socket_get_io_channel(msn_socket);
	g_io_channel_set_buffer_size(msn_channel,128);
	g_io_channel_set_buffered(msn_channel,1);
	g_io_channel_set_encoding(msn_channel, NULL, NULL);
	g_io_add_watch(msn_channel,G_IO_IN|G_IO_HUP|G_IO_ERR,(GIOFunc)network_receive,NULL);
	
	/* Start off the authentication process */
	net_send("VER 0 MSNP8 MYPROTOCOL CVR0 \r\n");
}

gboolean net_connected(void)
{
	return connected;
}

gboolean network_receive(GIOChannel *source, GIOCondition condition, gpointer data)
{
	/* Primarily, we'll only be reading in a small amount of info */
	if(log_level == 0)
	{
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
				error = gnet_io_channel_readn(msn_channel, &data, n, &n); 
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
			/*
				TODO: action to take
			*/
			if(strncmp(message, "VER",3)==0)
			{
				/* Got a response from VER, time to send CVR */
				char *tmp_message = g_strdup_printf("CVR 2 0x0c0a winnt 5.1 i386 MSNMSGR 8.0.0792 msmsgs %s\r\n",uemail);
				net_send(tmp_message);
				g_free(tmp_message);
			}
			else if(strncmp(message, "CVR", 3) == 0)
			{
				/* Got a response from CVR, time to send USR */
				char *tmp_message = g_strdup_printf("USR 3 TWN I %s\r\n",uemail);
				net_send(tmp_message);
				g_free(tmp_message);
			}
			else if(strncmp(message, "XFR", 3) == 0)
			{
				/* The tricky part... */
				log_level = 1;

				/*
					Now we close our socket, and point it at the ip given to us in the XFR message

					Sample specimen

					XFR 3 NS 207.46.108.91:1863 0 65.54.239.210:1863

				*/

				char *ip_start = strstr(message, "NS ");
				/* Skip past the "NS " */
				ip_start+=3;

				/* Find the splitter, " 0 " */
				char *ip_end = strstr(ip_start, " 0 ");
				ip_end[0] = '\0';

				char *ip_port_split = strstr(ip_start, ":");
				ip_port_split[0] = '\0';

				/* Voila */
				printf("IP: %s\n",ip_start);
				gnet_tcp_socket_delete(msn_socket);
				
				GInetAddr* addr;
				addr = gnet_inetaddr_new(ip_start, 1863);
				if(addr)
				{
					msn_socket = gnet_tcp_socket_new(addr);
					gnet_inetaddr_delete(addr);
				}
				else
				{
					error_dialog("Failed to resolve messenger.msn.com");
					return;
				}
	
				msn_channel = gnet_tcp_socket_get_io_channel(msn_socket);
				g_io_channel_set_buffer_size(msn_channel,128);
				g_io_channel_set_buffered(msn_channel,1);
				g_io_channel_set_encoding(msn_channel, NULL, NULL);
				g_io_add_watch(msn_channel,G_IO_IN|G_IO_HUP|G_IO_ERR,(GIOFunc)network_receive,NULL);
				printf("Transfered\n");
				net_send("VER 0 MSNP8 MYPROTOCOL CVR0 \r\n");
				return FALSE;
			}
		}
	}
	/* When the primary login stage (jumping connection) is done log_level = 1 */
	else if(log_level == 1)
	{
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
				error = gnet_io_channel_readn(msn_channel, &data, n, &n); 
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
			/*
				TODO: action to take
			*/
			if(strncmp(message, "VER",3)==0)
			{
				/* Got a response from VER, time to send CVR */
				char *tmp_message = g_strdup_printf("CVR 2 0x0c0a winnt 5.1 i386 MSNMSGR 8.0.0792 msmsgs %s\r\n",uemail);
				net_send(tmp_message);
				g_free(tmp_message);
			}
			else if(strncmp(message, "CVR", 3) == 0)
			{
				/* Got a response from CVR, time to send USR */
				char *tmp_message = g_strdup_printf("USR 3 TWN I %s\r\n",uemail);
				net_send(tmp_message);
				g_free(tmp_message);
			}
			else if(strncmp(message, "USR 3", 5) == 0)
			{
				/* 
					REALLY Tricky part now. OpenSSL - woohoo :( 

					Basically, the MSN protocol uses SSL to autheticate clients; so what happens now is, the MSN server sends us a 'challenge'
					for the SSL login page that we must send to it. So we need to extract that data, and send it to MSN to grab our golden 'ticket'
					which is used in the final login period.
				*/
				
				/* Example: USR 3 TWN S ct=1202475836,rver=5.0.3270.0,wp=FS_40SEC_0_COMPACT,lc=1033,id=507,ru=http:%2F%2Fmessenger.msn.com,tw=0,kpp=1,kv=4,ver=2.1.6000.1,rn=1lgjBfIL,tpf=b0735e3a873dfb5e75054465196398e0 */
				char *ticket = strstr(message, " ct=")+1;
				printf("Ticket: %s\n",ticket);
				
				char *return_ticket = net_ssl_auth(ticket);
				printf("Got ticket: %s\n",return_ticket);
				char *tmp_message = g_strdup_printf("USR 4 TWN S %s\r\n", return_ticket);
				net_send(tmp_message);
				g_free(tmp_message);
			}
			else if(strncmp(message, "USR 4", 5) == 0)
			{
				/* Woot I'm in! - We just got our email (useless) AND OUR NICKNAME! DAMN RIGHT BITCHES! */

				char *nick = split(message, ' ', 4);
				char *email = split(message, ' ', 3);
				globals_email = email;
				mwin_change_nick(nick);

				g_free(nick);
				char *tmp_message = g_strdup_printf("SYN 5 0\r\n");
				net_send(tmp_message);
				g_free(tmp_message);
				tmp_message = g_strdup_printf("CHG %d NLN 0\r\n",msg_num++);
				net_send(tmp_message);
				g_free(tmp_message);
				
			}
			else if(strncmp(message, "CHL", 3) == 0)
			{
				/* Got a challenge from MSN, and who am I to refuse? */
				char *challenge = message + 6;
				char *end = strstr(message, "\r\n");
				end[0] = '\0';
				char *chal_string = g_strdup_printf("%s%s",challenge,"Q1P7W2E4J9R8U3S5");
				GMD5* chal_md5 = gnet_md5_new(chal_string,strlen(chal_string));
				g_free(chal_string);
				
				gchar *hash = gnet_md5_get_string(chal_md5);
				gnet_md5_delete(chal_md5);
				
				printf("Challenge reply: %s\n",hash);
				
				char *tmp_message = g_strdup_printf("QRY %d msmsgs@msnmsgr.com 32\r\n%s",msg_num++, hash);
				net_send(tmp_message);
				g_free(tmp_message);	
			}
			else if(strncmp(message,"RNG",3) == 0)
			{
				/* Got an incoming message! */
				printf("Got an incoming message, someone hast double clicked upon us!\n");
				printf("MESSAGE: %s\n",message);
				char *phost = split(message,' ', 2);
				char *host = split(phost,':',0);
				free(phost);
				printf("Host: %s\n",host);
				
				char *session = split(message,' ', 1);
				printf("Session: %s\n",session);
				
				char *auth_str = split(message,' ',4);
				char *email = split(message, ' ', 5);
				char *nick = split(message, ' ', 6);
				nick[strlen(nick)-2]='\0';
			
				printf("Auth: %s\n",auth_str);
				printf("Email: %s\n",email);
				printf("Nick: %s\n", nick);

				conversation_new(host, session, auth_str, email, nick, uemail);
			}
			else if(strncmp(message,"MSG",3) == 0)
			{
				// Got a message, most likely it's gonna be from Hotmail
				// Important thing is, MSG's have a payload size!
				char *nick = split(message,' ',2);
				char *len = split(message,' ',3);
				len[strlen(len)-2] = '\0';
				int length = atoi(len);
				printf("Inbound message length: %d\n",length);

				error = G_IO_ERROR_NONE;
				char incoming_msg[length+1];
				memset(incoming_msg, '\0',length+1);
				gsize n = length;
				error = gnet_io_channel_readn(msn_channel, incoming_msg, n, &n);
				if(error != G_IO_ERROR_NONE)
				{
				   fprintf(stderr, "Error occurred reading message\n");
				   return FALSE;
				}
				printf("Message: %s\n",incoming_msg);
			}
		}
	}
	return TRUE;
}

char *net_ssl_auth(char *orig_ticket)
{
	char *ticket = g_malloc0(2048);
	int res = https_auth("login.passport.com", 443, "login2.srf", uemail,upass, orig_ticket, ticket, 2048, NULL);
	return ticket;
}
