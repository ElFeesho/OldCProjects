/*
	Copyright Andrew "shadowarts" Alm 2007 (c)
				 
	This file is part of Nonlogicpad.

	Nonlogicpad is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	Nonlogic Pad is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Nonlogic Pad; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "config.h"
#include "globals.h"

char *getConfigFile()
{
	char *fileName;
#ifndef WIN32
	asprintf(&fileName, "%s/.nlpconf.xml", getenv("HOME"));
#else
	char username[16];
	long len = 16; 
	GetUserName(username,&len);
	asprintf(&fileName, "C:\\Documents and Settings\\%s\\nlpconf.xml", username);
#endif

	return fileName;
}

void loadDefaultConfig()
{
	globals.nick = malloc(strlen("Anon"));
	strcpy(globals.nick,"Anon");
	
	globals.server= malloc(strlen("nlpad.nonlogic.org"));
	strcpy(globals.server,"nlpad.nonlogic.org");
	
	globals.channel = malloc(strlen("#nonlogic"));
	strcpy(globals.channel,"#nonlogic");
	
	globals.port = 10101;
}

void loadConfig(char *fileName)
{
	xmlDoc *confFile;
	xmlNode *node;

	// load defaults incase user doesnt have all settings in config file or config file doesnt exist
	loadDefaultConfig();

	confFile = xmlReadFile(fileName, NULL, 0);
	if(!confFile)
	{
		printf("%s: Could not load config file.\n", fileName);
		return;
	}

	node = xmlDocGetRootElement(confFile);
	if(!node)
	{
		xmlFreeDoc(confFile);
		return;
	}

	for(node = node->xmlChildrenNode; node; node = node->next)
	{
		unsigned char *settingName, *settingValue;
		settingName = xmlGetProp(node, (const xmlChar *)"name");
		settingValue = xmlGetProp(node, (const xmlChar *)"value");

		if(!settingName || !settingValue)
			continue;

		if(strcmp(settingName, "server") == 0)
			globals.server = settingValue;
		else if(strcmp(settingName, "port") == 0)
			globals.port = atoi(settingValue);
		else if(strcmp(settingName, "nick") == 0)
			globals.nick = settingValue;
		else if(strcmp(settingName, "channel") == 0)
			globals.channel = settingValue;
	}

	xmlFreeDoc(confFile);
}

void saveConfig(char *fileName)
{
	FILE *confFile;
	
	confFile = fopen(fileName, "w");
	if(!confFile)
		return;

	fprintf(confFile, "<config>\n");
	fprintf(confFile, "\t<option name=\"nick\" value=\"%s\" />\n", globals.nick);
	fprintf(confFile, "\t<option name=\"server\" value=\"%s\" />\n", globals.server);
	fprintf(confFile, "\t<option name=\"channel\" value=\"%s\" />\n", globals.channel);
	fprintf(confFile, "\t<option name=\"port\" value=\"%i\" />\n", globals.port);
	
	fprintf(confFile, "</config>");

	fclose(confFile);
}
