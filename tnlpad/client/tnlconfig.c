#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "globals.h"

static char *config_get_file()
{
	char *file_name = malloc(256);
#ifndef WIN32
	sprintf(file_name, "%s/.tnlpconf.xml", getenv("HOME"));
#else
	char username[16];
	long len = 16; 
	GetUserName(username,&len);
	sprintf(file_name, "C:\\Documents and Settings\\%s\\tnlpconf.xml", username);
#endif
	return file_name;
}

static void config_load_default()
{
	strcpy(globals_nick,"Anon");
	strcpy(globals_server,"tnlpad.colon-slash.co.uk");
	strcpy(globals_channel,"#todandlorna");
	
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
		fprintf(stderr, "%s: Could not load config file. Make sure it exists or you have read permissions\n", file_name);
		return;
	}

	node = xmlDocGetRootElement(conf_file);
	if(!node)
	{
		fprintf(stderr, "Failed to find the root element in your config file (not valid XML?)\n");
		xmlFreeDoc(conf_file);
		return;
	}

	for(node = node->xmlChildrenNode; node; node = node->next)
	{
		unsigned char *setting_name = NULL, *setting_value = NULL;
		setting_name = xmlGetProp(node, (const xmlChar *)"name");
		setting_value = xmlGetProp(node, (const xmlChar *)"value");

		if(!setting_name || !setting_value)
			continue;

		if(strcmp((char*)setting_name, "server") == 0)
		{
			strncpy(globals_server,(char*)setting_value,64);
		}
		else if(strcmp((char*)setting_name, "port") == 0)
		{
			globals_port = atoi((char*)setting_value);
		}
		else if(strcmp((char*)setting_name, "nick") == 0)
		{
			strncpy(globals_nick,(char*)setting_value,16);
		}
		else if(strcmp((char*)setting_name, "channel") == 0)
		{
			strncpy(globals_channel,(char*)setting_value,32);
		}
		else if(strcmp((char*)setting_name, "name") == 0)
		{
			strncpy(globals_name,(char*)setting_value,32);
		}
		else if(strcmp((char*)setting_name, "website") == 0)
		{
			strncpy(globals_website,(char*)setting_value,32);
		}
		else if(strcmp((char*)setting_name, "email") == 0)
		{
			strncpy(globals_email,(char*)setting_value,32);
		}
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
	{
		fprintf(stderr, "Couldn't save new settings to %s\n",file_name);
		return;
	}

	fprintf(conf_file, "<config>\n");
	if(globals_nick!=NULL)
		fprintf(conf_file, "\t<option name=\"nick\" value=\"%s\" />\n", globals_nick);
	if(globals_server!=NULL)
		fprintf(conf_file, "\t<option name=\"server\" value=\"%s\" />\n", globals_server);
	if(globals_channel!=NULL)
		fprintf(conf_file, "\t<option name=\"channel\" value=\"%s\" />\n", globals_channel);
	if(globals_name!=NULL)
		fprintf(conf_file, "\t<option name=\"name\" value=\"%s\" />\n", globals_name);
	if(globals_website!=NULL)
		fprintf(conf_file, "\t<option name=\"website\" value=\"%s\" />\n", globals_website);
	if(globals_email!=NULL)
		fprintf(conf_file, "\t<option name=\"email\" value=\"%s\" />\n", globals_email);
	fprintf(conf_file, "\t<option name=\"port\" value=\"%i\" />\n", globals_port);
	fprintf(conf_file, "</config>");

	fclose(conf_file);
	free(file_name);
}
