#ifndef __TOPICS_H__
#define __TOPICS_H__

struct topics_node
{
	int id;
	char *name;
	void *next;
};

extern void topics_create_table();
extern int topics_load_list();

#endif
