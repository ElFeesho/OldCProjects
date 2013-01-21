#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "globals.h"

static char *config_get_file()
{
	char *file_name = malloc(256);
#ifndef WIN32
	sprintf(file_name, "%s/.nlpconf.xml", getenv("HOME"));
#else
	char username[16];
	long len = 16; 
	GetUserName(username,&len);
	sprintf(file_name, "C:\\Documents and Settings\\%s\\nlpconf.xml", username);
#endif
	return file_name;
}

static void config_load_default()
{
	strcpy(globals_nick,"Anon");
	strcpy(globals_server,"nlpad.nonlogic.org");
	strcpy(globals_channel,"#nonlogic");
	
	globals_port = 10101;
}

void config_load()
{
	xmlDoc *conf_file;
	xmlNode *node;

	/* 
		Load defaults incase user doesnt have all settings in config file 
		or config file doesnt exist 
	*/
	
	config_load_default();
	char *file_name = config_get_file();
	conf_file = xmlReadFile(file_name, NULL, 0);
	if(!conf_file)
	{
		printf("%s: Could not load config file.\n", file_name);
		return;
	}

	node = xmlDocGetRootElement(conf_file);
	if(!node)
	{
		xmlFreeDoc(conf_file);
		return;
	}

	for(node = node->xmlChildrenNode; node; node = node->next)
	{
		unsigned char *setting_name, *setting_value;
		setting_name = xmlGetProp(node, (const xmlChar *)"name");
		setting_value = xmlGetProp(node, (const xmlChar *)"value");

		if(!setting_name || !setting_value)
			continue;

		if(strcmp((char*)setting_name, "server") == 0)
			strcpy(globals_server,(char*)setting_value);
		else if(strcmp((char*)setting_name, "port") == 0)
			globals_port = atoi((char*)setting_value);
		else if(strcmp((char*)setting_name, "nick") == 0)
			strcpy(globals_nick,(char*)setting_value);
		else if(strcmp((char*)setting_name, "channel") == 0)
			strcpy(globals_channel,(char*)setting_value);
	}
	free(file_name);
	xmlFreeDoc(conf_file);
}

void config_save()
{
	FILE *conf_file;
	char *file_name = config_get_file();
	conf_file = fopen(file_name, "w");
	if(!conf_file)
		return;

	fprintf(conf_file, "<config>\n");
	fprintf(conf_file, "\t<option name=\"nick\" value=\"%s\" />\n", globals_nick);
	fprintf(conf_file, "\t<option name=\"server\" value=\"%s\" />\n", globals_server);
	fprintf(conf_file, "\t<option name=\"channel\" value=\"%s\" />\n", globals_channel);
	fprintf(conf_file, "\t<option name=\"port\" value=\"%i\" />\n", globals_port);
	
	fprintf(conf_file, "</config>");

	fclose(conf_file);
	free(file_name);
}
