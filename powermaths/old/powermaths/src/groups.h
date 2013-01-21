#ifndef __GROUPS_H__
#define __GROUPS_H__

struct groups_node
{
	int id;
	char *name;
	char *caption;
	void *next;
};


extern GtkWidget *groups_create_viewport();
extern int groups_load_list();

#endif
