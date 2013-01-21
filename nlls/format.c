#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <math.h>

extern char *fname_format(char *name, char *root, int mode, int link)
{
	// \x1b[33;0mFILE_NAME\x1b[33;0m
	char *link_dest = malloc(256); //If file type is a link, find out where it goes!
	memset(link_dest,'\0',256);
	name[strlen(name)] = '\0';
	char *rtn_str = malloc(14+strlen(name));
	memset(rtn_str,'\0',14+strlen(name));
	strcpy(rtn_str,"\x1b[33;1m");
	if((mode&S_IFSOCK)==S_IFSOCK)
		rtn_str[3] = '5';
	else if((mode&S_IFLNK)==S_IFLNK)
	{
		rtn_str[3] = '6';
		if(!link)
		{
			char *link_loc = malloc(256);
			memset(link_loc,'\0',256);
			strcpy(link_loc,root);
			strcat(link_loc,"/");
			strcat(link_loc,name);
			readlink(link_loc,link_dest,256);
			//Ot oh, recursion!!!
			memset(link_loc,'\0',256);
			strcpy(link_loc,root);
			strcat(link_loc,"/");
			strcat(link_loc,link_dest);
			struct stat *link_mode = malloc(sizeof(struct stat));
			if(lstat(link_loc,link_mode)!=-1)
			{
				link_dest = fname_format(link_dest,root,link_mode->st_mode,1);
			}
		}
	}
	else if((mode&S_IFREG)==S_IFREG)
		if((mode&S_IXOTH)==S_IXOTH || (mode&S_IXUSR)==S_IXUSR || (mode&S_IXGRP)==S_IXGRP)
			rtn_str[3] = '2';
		else
			rtn_str[5] = '0';
	else if((mode&S_IFBLK)==S_IFBLK)
		rtn_str[3] = '3';
	else if((mode&S_IFDIR)==S_IFDIR)
		rtn_str[3] = '4';
	else if((mode&S_IFCHR)==S_IFCHR)
		rtn_str[3] = '3';
	else if((mode&S_IFIFO)==S_IFIFO)
		rtn_str[5] = '3';
	else
	{
		//Some other kind of funky file (MAKE IT PURPLE!!!)
		rtn_str[3] = '5';
	}
	strcat(rtn_str,name);
	if(strlen(link_dest)>0)
	{
		strcat(rtn_str,"\x1b[33;0m -> ");
		strcat(rtn_str,link_dest);
	}
	return rtn_str;
}

extern char *nested_format(char *fname)
{
	char *rtn_str = malloc(256);
	memset(rtn_str,'\0',256);
	
	struct stat *file_stat = malloc(sizeof(struct stat));
	
	if(lstat(fname,file_stat)!=-1)
	{
		//ONLY!!! Try and read what's inside the folder IF... We are in the same UID of the folder or GID AND it is eXecutable, or... if we're root :D
		if((file_stat->st_mode&S_IFDIR)==S_IFDIR && 
		(((file_stat->st_mode&S_IXUSR)==S_IXUSR && getuid()==file_stat->st_uid) || 
		(((file_stat->st_mode&S_IXGRP)==S_IXGRP && getgid()==file_stat->st_gid)) || getuid() == 0 || (file_stat->st_mode&S_IXOTH)==S_IXOTH))
		{
			//Only scan the contents of a dir (naturally - maybe later tars hahahahaha :( please don't hold me to that
			struct dirent *dent = NULL;
			DIR *dent_dir = opendir(fname);
			dent = readdir(dent_dir);
			int fcount = 0, hfcount = 0;
			errno = 0;
			do
			{
				if(strncmp(dent->d_name,".",1)==0 || strncmp(dent->d_name,"..",2)==0)
					continue;
				if(dent->d_name[0] == '.')
					hfcount++;
				else
					fcount++;
				
			}while(dent=readdir(dent_dir));
			if(errno>0)
			{
				perror("Failed reading dir contents");
				exit(1);
			}
			sprintf(rtn_str,"(F: %d, HF: %d)",fcount, hfcount);
		}
	}
	
	
	return rtn_str;
}

extern char *mode_format(int mode)
{
	char *rtn = malloc(10);
	memset(rtn,'\0',10);
	if((mode&S_IFSOCK)==S_IFSOCK)
		rtn[0] = 's';
	else if((mode&S_IFLNK)==S_IFLNK)
		rtn[0] = 'l';
	else if((mode&S_IFREG)==S_IFREG)
		rtn[0] = '-';
	else if((mode&S_IFBLK)==S_IFBLK)
		rtn[0] = 'b';
	else if((mode&S_IFDIR)==S_IFDIR)
		rtn[0] = 'd';
	else if((mode&S_IFCHR)==S_IFCHR)
		rtn[0] = 'c';
	else if((mode&S_IFIFO)==S_IFIFO)
		rtn[0] = 'p';
		
	if((mode&S_IRUSR)==S_IRUSR)
		rtn[1] = 'r';
	else
		rtn[1] = '-';
	if((mode&S_IWUSR)==S_IWUSR)
		rtn[2] = 'w';
	else
		rtn[2] = '-';
	if((mode&S_IXUSR)==S_IXUSR)
		rtn[3] = 'x';
	else
		rtn[3] = '-';

	if((mode&S_IRGRP)==S_IRGRP)
		rtn[4] = 'r';
	else
		rtn[4] = '-';
	if((mode&S_IWGRP)==S_IWGRP)
		rtn[5] = 'w';
	else
		rtn[5] = '-';
	if((mode&S_IXGRP)==S_IXGRP)
		rtn[6] = 'x';
	else
		rtn[6] = '-';
	
	if((mode&S_IROTH)==S_IROTH)
		rtn[7] = 'r';
	else
		rtn[7] = '-';
	if((mode&S_IWOTH)==S_IWOTH)
		rtn[8] = 'w';
	else
		rtn[8] = '-';
	if((mode&S_IXOTH)==S_IXOTH)
		rtn[9] = 'x';
	else
		rtn[9] = '-';

	return rtn;
}

extern char *time_format(time_t *mod_time)
{
	char *rtn = malloc(16);
	memset(rtn,'\0',16);
	struct tm *timeinf = localtime(mod_time);
	sprintf(rtn,"%d-%02d-%02d %02d:%02d",1900+timeinf->tm_year,timeinf->tm_mon+1,timeinf->tm_mday,timeinf->tm_hour,timeinf->tm_min);
	return rtn;
}

extern char *size_format(unsigned long long size)
{
	char *rtn = malloc(10);
	memset(rtn, '\0', 10);
	if(size<=1024)
		sprintf(rtn,"%d",size);
	else if(size>1024 && size <=1024000)
		if(size<10240)
			sprintf(rtn,"%.1fK",ceil(size/102.4)/10);
		else
			sprintf(rtn,"%.0fK",ceil(size/1024.0));			
	else if(size>1024000 && size<=1073741000)
		if(size<10240000)
			sprintf(rtn,"%.1fM",ceil(size/104857.6)/10);
		else
			sprintf(rtn,"%.0fM",ceil(size/1048576.0));
	else
		sprintf(rtn,"%.1fG",(ceil(size/103757.6))/10240.0);
	return rtn;
}

extern char *user_format(int uid)
{
	struct passwd *c_filepass = getpwuid(uid);
	if(c_filepass!=NULL)
	{
		return strdup(c_filepass->pw_name);
	}
	else
	{
		char *ret_str = malloc(10); //AS IF!
		memset(ret_str,'\0',10);
		sprintf(ret_str,"%d",uid);
		return ret_str;
	}
}

extern char *group_format(int gid)
{
	struct group *c_filegroup = getgrgid(gid);
	if(c_filegroup!=NULL)
	{
		return strdup(c_filegroup->gr_name);
	}
	else
	{
		char *ret_str = malloc(10); //Yeah right...
		memset(ret_str,'\0',10);
		sprintf(ret_str,"%d",gid);
		return ret_str;
	}
}
