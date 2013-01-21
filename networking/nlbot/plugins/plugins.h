#ifndef __PLUGINS_H__
#define __PLUGINS_H__

typedef struct plugIn
{
	char *(*pluginFunc)(char*,char*,char*,char);
}plugIn;

plugIn *loadPlug();

#endif
