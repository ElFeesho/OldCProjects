#ifndef __TYPES_H__
#define __TYPES_H__

struct types_node
{
	int id;
	char *name;
	void *next;
};

extern void types_create_table();
extern int types_load_list();

#endif
