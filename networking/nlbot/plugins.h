#ifndef __PLUGINS_H__
#define __PLUGINS_H__

#include <stdlib.h>
#include <memory.h>

typedef struct plugIn
{
	char *(*pluginFunc)(char*, char*, char*,char);
}plugIn;

typedef struct plugList
{
	char name[32]; // File name of the plugin 
	plugIn *plug;	// Pointer to plugin struct with plugin function
	void *next;		// Pointer to next plugin node in the list
	void *handle;  // So it is possible to dlclose the plugin when it is not needed
}plugList;

plugList *plugins;

int addPlugin(char *filename);
int removePlug(char *plug);
void clearPlugs();
void loadPlugins(char *filename);
#endif
