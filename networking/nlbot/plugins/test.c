#include "plugins.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strops.h"

char *plugFunc(char *channel, char *nick, char *msg, char priv)
{
	char *retString = 0;
	if(strcmp(nick,"NickServ")==0)
		return 0;
	if(priv)
	{
		retString = malloc(strlen(nick)+strlen(msg)+strlen(channel)+128);
		sprintf(retString,"PRIVMSG %s :WOOHOO!\r\n",nick);
	}
	else
	{
		retString = malloc(strlen(nick)+strlen(msg)+strlen(channel)+128);
		sprintf(retString,"PRIVMSG %s :WOOHOO!\r\n",channel);
	}
	return retString;
}

plugIn *loadPlug()
{
	plugIn *nPlug = malloc(sizeof(plugIn));
	nPlug->pluginFunc = &plugFunc;
	return nPlug;
}

