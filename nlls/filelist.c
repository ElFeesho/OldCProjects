#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <math.h>

#include "filelist.h"
#include "format.h"

#define max(a,b) (a>b)?(a):(b)

static filelist_node_t *file_list = NULL;

static filelist_user_len = 0;		//The longest username in the file list (so it's all nicely aligned)
static filelist_group_len = 0;	//The longest groupname in the file list.

extern void filelist_add(char *name, char *root)
{
	char *actual_file = malloc(256);
	memset(actual_file,'\0',256); //root+name
	strcpy(actual_file,root);
	strcat(actual_file,"/");
	strcat(actual_file,name);
	struct stat *file_stat = malloc(sizeof(struct stat));
	if(lstat(actual_file,file_stat)==-1)
	{
		fprintf(stderr,"Couldn't stat %s\n",actual_file);
		exit(1);
	}
	
	if(!file_list)
	{
		file_list = malloc(sizeof(filelist_node_t));
		file_list->name = fname_format(name,root,file_stat->st_mode,0);
		file_list->user = user_format(file_stat->st_uid);
		file_list->group = group_format(file_stat->st_gid);
		file_list->size = size_format(file_stat->st_size);
		file_list->mode = mode_format(file_stat->st_mode);
		file_list->time = time_format(&file_stat->st_mtime);
		file_list->nested = nested_format(actual_file);
		file_list->next = NULL;
		filelist_user_len = max(filelist_user_len,strlen(file_list->user));
		filelist_group_len = max(filelist_group_len,strlen(file_list->group));
	}
	else
	{
		filelist_node_t *temp = malloc(sizeof(filelist_node_t));		
		temp->name = fname_format(name,root,file_stat->st_mode,0);
		temp->user = user_format(file_stat->st_uid);
		temp->group = group_format(file_stat->st_gid);
		temp->size = size_format(file_stat->st_size);
		temp->mode = mode_format(file_stat->st_mode);
		temp->time = time_format(&file_stat->st_mtime);
		temp->nested = nested_format(actual_file);
		temp->next = file_list;
		file_list = temp;
		filelist_user_len = max(filelist_user_len,strlen(file_list->user));
		filelist_group_len = max(filelist_group_len,strlen(file_list->group));
	}
	free(actual_file);
}

extern void filelist_print()
{
	char *format = malloc(32);
	memset(format,'\0',32);
	//              mod user  group Siz Tim File
	sprintf(format,"%%s %%%ds %%-%ds %%4s %%s %%s\x1b[33;0m %%s\n",filelist_user_len, filelist_group_len);
	filelist_node_t *temp = file_list;
	while(temp!=NULL)
	{
		printf(format,temp->mode, temp->user, temp->group, temp->size, temp->time, temp->name, temp->nested);
		temp = temp->next;
	}
}
