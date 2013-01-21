#include <gnet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network.h"
#include "gui.h"

static GUdpSocket *network_socket;
static GInetAddr *network_send_addr;

/*
	Receive network data callback!
*/

static gboolean network_receive(GIOChannel *source, GIOCondition condition, gpointer data)
{
	if((condition&G_IO_IN) == G_IO_IN)
	{
		gchar header = 0;
		gnet_udp_socket_receive(network_socket,(gchar*)&header,1,NULL);
		if(header!=16)
		{
			/*
				If the header doesn't indicate that what we are receiving is infact
				a message from this program... Flush it away!
			*/
			if (g_io_channel_flush(source, NULL)==G_IO_CHANNEL_ERROR)
			{
				printf("Didn't flush correctly!\n");
			}
			return TRUE;
		}
		
		/*
			We have ourselves a message! (hopefully!)

			Nickname's get packed to 16 bytes, so lets read us in a nick name!
		*/

		gchar nickname[17] = { 0 };
		gnet_udp_socket_receive(network_socket,nickname,16,NULL);
		
		/*printf("DEBUG: NICKNAME: %s\n",nickname);*/
		
		/*
			Now that we've got the nickname, let's read us in a the length of
			text we are gonna need to be reading in. (2 bytes in BIG ENDIAN)
		*/

		short length = 0;
		gnet_udp_socket_receive(network_socket,(gchar*)&length,2,NULL);

		/* ntohs = network to host short (network endian = big endian) */
		length = ntohs(length);

		/*printf("DEBUG: LENGTH: %d\n",length);*/

		gchar *message = g_malloc0(length+1);

		gnet_udp_socket_receive(network_socket,message,length,NULL);

		gui_scrollback_add(nickname, message);
	}
	return TRUE;
}

void network_change_ip(char *ip)
{
	gnet_inetaddr_delete(network_send_addr);
	network_send_addr = gnet_inetaddr_new(ip, 9348);
}

void network_send_message(char *who, char *message)
{
	char nickname[16] = { 0 };
	strncpy(nickname, who, 16);
	short length = htons(strlen(message));
	char header = 16;
	gnet_udp_socket_send(network_socket, (const gchar*)&header, 1, network_send_addr);
	gnet_udp_socket_send(network_socket, (const gchar*)nickname, 16, network_send_addr);
	gnet_udp_socket_send(network_socket, (const gchar*)&length, 16, network_send_addr);
	gnet_udp_socket_send(network_socket, (const gchar*)message, strlen(message), network_send_addr);
}

void network_init(void)
{
	gnet_init();

	network_socket = gnet_udp_socket_new_with_port(9348);
	if(!network_socket)
	{
		fprintf(stderr,"Failed creating UDP socket on port 9348! Could already be in use?\n");
		exit(1);
	}
 
	network_send_addr = gnet_inetaddr_new("255.255.255.255", 9348);
	GIOChannel *io_chan = gnet_udp_socket_get_io_channel(network_socket);
	g_io_channel_set_buffer_size(io_chan,128);
	g_io_channel_set_buffered(io_chan,1);
	g_io_channel_set_encoding(io_chan, NULL, NULL);
	g_io_add_watch(io_chan,G_IO_IN|G_IO_HUP|G_IO_ERR,(GIOFunc)network_receive,NULL);
}
