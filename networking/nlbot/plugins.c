#include "plugins.h"
#include <dlfcn.h>
#include <stdio.h>

int addPlugin(char *filename)
{
	typedef plugIn* (*plugInt)();
	if(plugins==NULL)
	{
		plugins = malloc(sizeof(plugins));
		plugins->handle = dlopen(filename, RTLD_LAZY);
		if(plugins->handle == NULL)
			return 0;
		dlerror();

		plugInt retPlug = (plugInt) dlsym(plugins->handle, "loadPlug");
		const char *dlsym_error = dlerror();
		if (dlsym_error) 
		{
			printf("Error: %s\n",dlsym_error);
			dlclose(plugins->handle);
			return 0;
		}	
		strcpy(plugins->name,filename);
		plugins->plug = retPlug();
		plugins->next = NULL;
		printf("Loaded: %s\n",filename);
	}
	else
	{
		//Make sure we're not loading the same plugin twice
		plugList *iter = plugins;
		while(iter!=NULL)
		{
			if(strcmp(iter->name,filename)==0)
				return 0;
			iter = iter->next;
		}
		plugList *temp = malloc(sizeof(plugList));
		temp->handle = dlopen(filename, RTLD_LAZY);
		if(temp->handle == NULL)
			return 0;
		dlerror();
		plugInt retPlug = (plugInt) dlsym(temp->handle, "loadPlug");
		const char *dlsym_error = dlerror();
		if (dlsym_error) 
		{
			printf("Error: %s\n",dlsym_error);
			dlclose(temp->handle);
			return 0;
		}
		strcpy(temp->name,filename);
		temp->plug = retPlug();
		temp->next = plugins;
		printf("Loaded: %s\n",filename);
		plugins = temp;
	}
	return 1;
}	

int removePlugs(char *name)
{
	plugList *temp = plugins;
	plugList *prev = NULL;
	while(temp!=NULL)
	{
		if(strcmp(temp->name,name)==0)
		{
			if(prev!=NULL)
			{
				if(temp->next!=NULL) // Middle node
				{
					prev->next = temp->next;
					dlclose(temp->handle);
					free(temp->plug);
					free(temp);
					printf("Removed: %s\n",name);
					return 1;
				}else{
					prev->next = NULL; // last node
					dlclose(temp->handle);
					free(temp->plug);
					free(temp);
					printf("Removed: %s\n",name);
					return 1;
				}
			}
			else // First node
			{
				if(temp->next!=NULL)
					plugins = temp->next;
				else
					plugins = NULL;
				dlclose(temp->handle);
				free(temp->plug);
				free(temp);				
				printf("Removed: %s\n",name);
				return 1;
			}
		}
		prev = temp;
		temp = temp->next;
	}
	printf("Didn't remove any plugin!\n");
	return 0;
}

void clearPlugs()
{
	//Run through and close the handles
	plugList *temp = plugins;
	while(temp!=NULL)
	{
		dlclose(temp->handle);
		printf("Closed Plugin :%s\n",temp->name);
		free(temp->plug);
		plugList *temp2 = temp;
		temp = temp->next;
		free(temp2);
	}
	plugins = NULL;
}

void loadPlugins(char *filename)
{
	FILE *plLst = fopen(filename,"r");
	while(!feof(plLst))
	{
		char *fname = malloc(128);
		memset(fname,'\0',128);
		fgets(fname,128,plLst);
		if(strlen(fname)>1)
		{
			fname[strlen(fname)-1] = '\0';
			FILE *plug = fopen(fname,"r");
			if(plug)
			{
				fclose(plug);
				addPlugin(fname);
			}
			else
			{
				printf("Couldn't open plugin %s!\n",fname);
			}
		}
	}	
	fclose(plLst);
}
