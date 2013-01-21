#ifndef __SCAT_H__
#define __SCAT_H__

struct scat_node
{
	int id;
	char *name;
	void *next;
};

extern void scat_create_table();
extern int scat_load_list();

#endif
