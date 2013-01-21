#ifndef __FILELIST_H__
#define __FILELIST_H__

typedef struct _filelist_t
{
	char *name;		//Filename		"test.log"
	char *user;		//User name		"feesh"
	char *group;	//Group Name	"users"
	char *size;		//Size			"16.0K"
	char *mode;		//Mode on file	"-rwxr--r--"
	char *time;		//2007-06-22 17:00
	char *nested;
	struct _filelist_t *next;
}filelist_node_t;

extern void filelist_add(char *name, char *root);
extern void filelist_print();

#endif
