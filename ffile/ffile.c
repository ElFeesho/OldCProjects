#include <stdio.h>
#include <string.h>
#include "ffile.h"

/* 
	Helper string funcs
*/

static int ffile_find_instr(const char *string, char token)
{
	int i = 0;
	for(i=0;i<strlen(string);i++)
		if(string[i] == token)
			return i;
   return -1;
}

static void ffile_insert_into_file(FILE *file, void *data, int length)
{
	int cur_pos = ftell(file);
	fseek(file,0,SEEK_END);
	int clength = ftell(file)-cur_pos;
	printf("Excess length = %d\n",clength);
	fseek(file, cur_pos, SEEK_SET);
	if(clength <= 0)
	{
		//At end of file - HURRAY!
		fwrite(data,1,length,file);
		return;
	}
	printf("About to alloc data for file insertions\nvoid *fdata = malloc(%d);\n",clength);
	void *fdata = malloc(clength);
	fread(fdata, 1, clength, file);
	fseek(file,cur_pos,SEEK_SET);
	fwrite(data,1,length,file);
	fwrite(fdata,1,clength,file);
}

static void ffile_overwrite_into_file(FILE *file, void *data, int from, int length, int oldlength)
{
	fseek(file,from+length,SEEK_SET);
	fseek(file,0,SEEK_END);
	int file_length = ftell(file)-from+oldlength;
	fseek(file, from+oldlength, SEEK_SET);
	void *fdata = malloc(file_length);
	fread(fdata, 1, file_length, file);
	fseek(file,from,SEEK_SET);
	fwrite(data,1,length,file);
	fwrite(fdata,1,file_length,file);
}


static char **ffile_tokenize_string(const char *string, int *count)
{
	char **return_val = malloc(strlen(string)/2*(sizeof(char*)));
   char delim = '/';
	int pos = 0;
	*count = 0;
	if(ffile_find_instr(string,delim)==-1)
	{
		return_val[*count] = malloc(strlen(string));
		strcpy(return_val[*count],string);
		*count=*count+1;
		return return_val;
	}
	while((pos = ffile_find_instr(string,delim))!=-1)
	{
		char *buffer = malloc(pos);
		strncpy(buffer,string,pos);
		if(buffer!=NULL)
		{
			return_val[*count] = malloc(pos);
			strncpy(return_val[*count],string,pos);
			*count=*count+1;
			if(pos != strlen(string)-1)
			{
				string+=pos+1;
			}
			else
			{
				return return_val;
			}
		}
		else
		{
			string+=pos+1;
		}
		free(buffer);
	}
	char *buffer = malloc(strlen(string));
	strncpy(buffer,string,strlen(string));
	if(buffer!=NULL)
	{
		return_val[*count] = malloc(strlen(string));
		strncpy(return_val[*count],string,strlen(string));
		*count=*count+1;
	}
	free(buffer);	
	return return_val;
}

/*
	Object Creators
*/

ffile_handle_t *ffile_new(char *filename)
{
	ffile_handle_t *ffile = malloc(sizeof(ffile_handle_t));
	ffile->file = fopen(filename,"r+");
	if(!ffile->file)
	{
		ffile->file = fopen(filename,"w+");
		if(!ffile->file)
			return NULL;
	}
	return ffile;
}

void ffile_close(ffile_handle_t *ffile)
{
	fclose(ffile->file);
}

void *ffile_header_new()
{
	ffile_header_t *header = malloc(sizeof(ffile_header_t));
	strcpy(header->magic_num, "MAGICAST");
	header->version = 0x00010001;
	memset(header->checksum,'\0',32);
	header->start = sizeof(ffile_header_t);
	
	return header;
}

void *ffile_file_new(char *name)
{
	printf("Creating file %s\nffile_ent_t *file = malloc(%d);",name,sizeof(ffile_ent_t));
	ffile_ent_t *file = malloc(sizeof(ffile_ent_t));
	printf("Created\n");
	file->type = FFILE_FILE_TYPE;
	if(strlen(name)>64)
		return NULL;
	
	strcpy(file->name,name);	
	return file;	
}

void *ffile_folder_new(char *name)
{
	printf("Creating folder %s\nffile_ent_t *folder = malloc(%d);",name,sizeof(ffile_ent_t));
	ffile_ent_t *folder = malloc(sizeof(ffile_ent_t));
	printf("Created\n");
	
	folder->type = FFILE_FOLDER_TYPE;
	if(strlen(name)>64)
		return NULL;
	
	strcpy(folder->name,name);
	return folder;
}

void *ffile_footer_new()
{
	ffile_footer_t *footer = malloc(sizeof(ffile_footer_t));
	memset(footer->checksum,'\0',32);
	return footer;
}

/* 
	End of Object Creators
*/

void ffile_insert_header(ffile_handle_t *ffile, ffile_header_t *header)
{
	fwrite(header,1,sizeof(ffile_header_t),ffile->file);
	fflush(ffile->file);
}

void ffile_insert_folder(ffile_handle_t *ffile, ffile_ent_t *folder, char *dest)
{
	if(folder->type!=FFILE_FOLDER_TYPE)
	{
		fprintf(stderr, "ASSERTION: Folder == Folder FAILED\n");
		return;
	}

	int count = 0;
	if(dest!=NULL)
	{
		char **dirs = ffile_tokenize_string(dest,&count);
		int32_t folder_pos[count]; //Folder positions!
		ffile_ent_t *temp_folders[count];
		int found = 0;
		fseek(ffile->file,sizeof(ffile_header_t),SEEK_SET);
		while(found != count && !feof(ffile->file))
		{
			ffile_ent_t temp;
			if(fread(&temp,1,sizeof(ffile_ent_t),ffile->file)<=0)
				break;
			if(temp.type == FFILE_FOLDER_TYPE)
			{
				if(strcmp(temp.name,dirs[found])==0)
				{
					folder_pos[found] = ftell(ffile->file)-sizeof(ffile_ent_t);
					temp_folders[found] = ffile_folder_new(temp.name);
					temp_folders[found]->length = temp.length+sizeof(ffile_ent_t);
					found++;
				}
				else
				{
					//If it's not the folder we want, let's jump it!
					fseek(ffile->file,temp.length,SEEK_CUR);
				}	
			}
			else
			{
				//If it's not a folder, let's skip past it!
				fseek(ffile->file,temp.length,SEEK_CUR);
			}
		}
		if(found!=count)
		{
			fprintf(stderr,"Failed locating sub folders for inserting the folder!\n");
			return;
		}else{		
			//If we have specified a place for the folder to go I.E, it's not on the base level, then we should be at the right
			//position to let rip and insert out folder!
	
			ffile_insert_into_file(ffile->file, folder, sizeof(ffile_ent_t));
	
			//Now we just need to update the current folders by updating their length to reflect the new folder they house!
			int i;
			for(i=0;i<count;i++)
				ffile_overwrite_into_file(ffile->file,temp_folders[i],folder_pos[i],sizeof(ffile_ent_t),sizeof(ffile_ent_t));	
		}
	}
	else
	{
		fseek(ffile->file, sizeof(ffile_header_t), SEEK_SET);
		ffile_insert_into_file(ffile->file, folder, sizeof(ffile_ent_t));
	}	
	fflush(ffile->file);
}

void ffile_insert_file(ffile_handle_t *ffile, ffile_ent_t *file, char *filename, char *dest)
{
	printf("Inserting file %s\n",filename);
	if(file->type!=FFILE_FILE_TYPE)
	{
		fprintf(stderr, "ASSERTION: File == File FAILED\n");
		return;
	}
	
	FILE *file_data = fopen(filename,"rb");
	char *file_data_bytes = NULL;
	int file_length = 0;
	if(!file_data)
	{
		fprintf(stderr,"FFILE: Couldn't open file for file data\n");
		return;
	}
	
	fseek(file_data,0,SEEK_END);
	file_length = ftell(file_data);
	fseek(file_data,0,SEEK_SET);
	file_data_bytes = malloc(file_length);
	fread(file_data_bytes,1, file_length, file_data);
	fclose(file_data);
	int count = 0;
	if(dest!=NULL)
	{
		char **dirs = ffile_tokenize_string(dest,&count);
		printf("Found %d directories in string\n",count);
		int32_t folder_pos[count]; //Folder positions!
		ffile_ent_t *temp_folders[count];
		int found = 0;
		fseek(ffile->file,sizeof(ffile_header_t),SEEK_SET);
		while(found != count && !feof(ffile->file))
		{
			ffile_ent_t temp;
			if(fread(&temp,1,sizeof(ffile_ent_t),ffile->file)<=0)
				break;
			if(temp.type == FFILE_FOLDER_TYPE)
			{
				if(strcmp(temp.name,dirs[found])==0)
				{
					printf("Found folder, adding it to temp list\n");
					folder_pos[found] = ftell(ffile->file)-sizeof(ffile_ent_t);
					temp_folders[found] = ffile_folder_new(temp.name);
					temp_folders[found]->length = temp.length+file_length+sizeof(ffile_ent_t);
					found++;
				}
				else
				{
					//If it's not the folder we want, let's jump it!
					fseek(ffile->file,temp.length,SEEK_CUR);
				}	
			}
			else
			{
				//If it's not a folder, let's skip past it!
				fseek(ffile->file,temp.length,SEEK_CUR);
			}
		}
		if(found!=count)
		{
			fprintf(stderr,"Failed locating sub folders for inserting the file!\n");
			return;
		}
		else
		{
			//If we have specified a place for the folder to go I.E, it's not on the base level, then we should be at the right
			//position to let rip and insert out folder!
			file->length = file_length;
			ffile_insert_into_file(ffile->file, file, sizeof(ffile_ent_t));
			ffile_insert_into_file(ffile->file, file_data_bytes, file_length);
	
			//Now we just need to update the current folders by updating their length to reflect the new folder they house!
			int i;
			for(i=0;i<count;i++)
				ffile_overwrite_into_file(ffile->file,temp_folders[i],folder_pos[i],sizeof(ffile_ent_t),sizeof(ffile_ent_t));			
		}
	}
	else
	{
		printf("Setting file location\n");
		fseek(ffile->file, sizeof(ffile_header_t), SEEK_SET);
		file->length = file_length;
		printf("Inserting file!\n");
		ffile_insert_into_file(ffile->file, file, sizeof(ffile_ent_t));
		printf("Inserting file data\n");
		ffile_insert_into_file(ffile->file, file_data_bytes, file_length);
	}
	fflush(ffile->file);
}
