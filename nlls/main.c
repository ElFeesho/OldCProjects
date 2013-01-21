/*
	Nonlogic LS!
	~~~~~~~~~~~~~~~~~~~~~~~~
	Mainly for fun, but it is useful
	Lists directories and there contents
*/

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

#include "format.h"
#include "filelist.h"

char *get_folder_contents(char *dir_name)
{
	char *ret_string = malloc(32);
	
}

void print_details(struct dirent *dir, char *root_dir)
{
	struct stat *file_mode = malloc(sizeof(struct stat));
	char *cfile_name = malloc(256);
	memset(cfile_name,'\0',256);
	strcpy(cfile_name,root_dir);
	strcat(cfile_name,dir->d_name);
	lstat(cfile_name, file_mode);
	free(cfile_name);
	char *user = malloc(32);
	char *group = malloc(32);
	
	printf("%10s %s %s %4s %16s \x1b[32;1m%-16s\x1b[34;0m\n",mode_format(file_mode->st_mode),user_format(file_mode->st_uid),group_format(file_mode->st_gid), size_format(file_mode->st_size), time_format(&file_mode->st_mtime), dir->d_name);
}

int main(int argc, char ** argv)
{
	struct dirent *dir;
	DIR *c_dir = NULL;
	char *root;
	if(argc>1)
		root = argv[1];
	else
		root = "./";
	struct stat *file_mode = malloc(sizeof(struct stat));
	if((lstat(root, file_mode)) == -1)
	{
		fprintf(stderr,"Couldn't stat file: %s\n",argv[1]);
		return -1;
	}
	c_dir = opendir(root);

	dir = readdir(c_dir);
	
	do
	{
		if(strncmp(dir->d_name,".",1)==0 || strncmp(dir->d_name,"..",2)==0)
			continue;
		filelist_add(dir->d_name, root);
		//print_details(dir, root);
	}while((dir = readdir(c_dir))!=NULL);
	filelist_print();
	return 0;
}
