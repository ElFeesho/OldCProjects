#ifndef __LEVELS_H__
#define __LEVELS_H__

struct levels_node
{
	int id;
	char *name;
	void *next;
};

extern void levels_create_table();
extern int levels_load_list();
extern struct levels_node* levels_load_list_from_group_id(int id);

#endif
