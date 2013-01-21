#ifndef __GROUPS_H__
#define __GROUPS_H__

struct groups_node
{
	int id;
	char *name;
	char *caption;
	void *next;
};

extern struct groups_node *groups_list;
extern void groups_create_table();
extern int groups_load_list();

#endif
