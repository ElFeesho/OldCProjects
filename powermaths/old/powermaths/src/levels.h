#ifndef __LEVELS_H__
#define __LEVELS_H__

struct levels_node
{
	int id;
	char *grade_name;
	void *next;
};

extern GtkWidget *levels_create_viewport();
extern int levels_load_list();

#endif
