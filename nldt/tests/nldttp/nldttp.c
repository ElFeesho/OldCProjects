#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <memory.h>
#include <string.h>
#include <stdint.h>
#include <glib.h>
#include "nldttp.h"

static void *mempcpy(void *dest, const void *src, size_t n) 
{ 
	return memcpy(dest, src, n) + n; 
}

extern nldttp_header_t *nldttp_header_new(uint8_t type_id, uint8_t priority, uint16_t packet_num, uint16_t packet_tot, uint8_t client_id, uint8_t client_rec, uint16_t size)
{
	nldttp_header_t *header = g_malloc0(sizeof(nldttp_header_t));
	header->type_id = type_id;
	header->priority = priority;
	header->packet_num = packet_num;
	header->packet_tot = packet_tot;
	header->client_id = client_id;
	header->client_rec = client_rec;
	header->size = size;
	return header;
}

extern nldttp_header_t *nldttp_header_read(int sock)
{
	nldttp_header_t *header = g_malloc0(sizeof(nldttp_header_t));
	void *buffer = g_malloc0(10);
	if(recvfrom(sock,buffer,10,MSG_PEEK,NULL,0)<0)
		return NULL;
	memcpy(&header->type_id, buffer, 1);
	memcpy(&header->priority, buffer+1, 1);
	memcpy(&header->packet_num, buffer+2, 2);
	memcpy(&header->packet_tot, buffer+4, 2);
	memcpy(&header->client_id, buffer+6, 1);
	memcpy(&header->client_rec, buffer+7, 1);
	memcpy(&header->size, buffer+8, 2);
	free(buffer);
	return header;
}

extern void *nldttp_header_prefix(nldttp_header_t *header, void *data, int *size)
{
	*size = header->size+10;
	void *result = g_malloc0(*size);
	void *pos_next = mempcpy(result,	&header->type_id, 1);
	pos_next = mempcpy(pos_next, &header->priority, 1);
	pos_next = mempcpy(pos_next, &header->packet_num, 2);
	pos_next = mempcpy(pos_next, &header->packet_tot, 2);
	pos_next = mempcpy(pos_next, &header->client_id, 1);
	pos_next = mempcpy(pos_next, &header->client_rec, 1);
	pos_next = mempcpy(pos_next, &header->size, 2);
	pos_next = mempcpy(pos_next, data, header->size);
	return result;
}
