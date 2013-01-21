#include "plugins.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strops.h"

char *plugFunc(char *channel, char *nick, char *msg, char priv)
{
	char *retString = 0;
	if(strncmp(msg,"!poke",5)==0)
	{
		retString = malloc(strlen(nick)+strlen(msg)+strlen(channel)+128);
		sprintf(retString,"PRIVMSG %s :I'll poke you %s!\r\n",channel,msg+6);
	}
	return retString;
}

plugIn *loadPlug()
{
	plugIn *nPlug = malloc(sizeof(plugIn));
	nPlug->pluginFunc = &plugFunc;
	return nPlug;
}
