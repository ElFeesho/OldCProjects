#ifndef __NLDT_TTP_H__
#define __NLDT_TTP_H__

typedef struct _nldttp_header_t
{
	uint8_t type_id;
	uint8_t priority;
	uint16_t packet_num;
	uint16_t packet_tot;
	uint8_t client_id;
	uint8_t client_rec;
	uint16_t size;
} nldttp_header_t;

extern nldttp_header_t *nldttp_header_new(uint8_t type_id, uint8_t priority, uint16_t packet_num, uint16_t packet_tot, uint8_t client_id, uint8_t client_rec, uint16_t size);
extern nldttp_header_t *nldttp_header_read(int sock);
extern void *nldttp_header_prefix(nldttp_header_t *header, void *data, int *size);

#endif
