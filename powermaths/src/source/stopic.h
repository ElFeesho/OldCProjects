#ifndef __STOPIC_H__
#define __STOPIC_H__

struct stopic_node
{
	int id;
	char *name;
	void *next;
};

extern void stopic_create_table();
extern int stopic_load_list();

#endif
