#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "sha.h"

static char *encrypted_file = "files.dat";
static char *decrypted_buf = NULL;
static FILE *tmp_file = NULL;
static SHA_CTX sha_ctx;
static int verbose = 0;
static int updates = 0;

static char *encrypt_nl(char *text, int length)
{
	int i = 0;
	char *return_str = malloc(length+1);

	memset(return_str, '\0', length+1);
	memcpy(return_str, text, length);

	for(i=0;i<length;i++)
	{
		return_str[i] ^= 'N';
	}
	return return_str;
}

static char *sha1_file(char *filename)
{
	FILE *subject;
	unsigned char *sha1_digest = malloc(21);
	struct stat file_mode;
	char *return_digest = malloc(41);
	char *file_contents;
	int i, j;

	memset(sha1_digest, '\0', 21);
	memset(return_digest, '\0', 41);

	if(lstat(filename, &file_mode)==-1)
	{
		return NULL;
	}

	subject = fopen(filename, "r");

	if(!subject)
	{
		return NULL;
	}

	file_contents = malloc(file_mode.st_size+1);
	fread(file_contents, 1, file_mode.st_size, subject);
	fclose(subject);
	SHA1_Init(&sha_ctx);
	SHA1_Update(&sha_ctx, (sha1_byte*)file_contents, file_mode.st_size);
	SHA1_Final(sha1_digest, &sha_ctx);

	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 4; j++)
		{
			sprintf(return_digest,"%s%02x", return_digest, sha1_digest[i*4+j]);
		}
   }
	free(file_contents);
	free(sha1_digest);
	return return_digest;
}

static void scan_files()
{
	/* Check file changes */
	FILE *tmp_output = fopen("/tmp/.nlidout","w");
	FILE *data_file = NULL;
	struct stat file_mode;
	char *enc_buf = NULL;
	char *dec_buf = NULL;

	if(verbose)
		printf("Scanning\n");

	if(lstat(encrypted_file, &file_mode)==-1)
	{
		fprintf(stderr, "Failed opening %s for scan procedure\n",encrypted_file);
		return;
	}

	data_file = fopen(encrypted_file,"r");
	if(!data_file)
	{
		fprintf(stderr, "Failed opening %s for scan procedure\n",encrypted_file);
		return;
	}


	enc_buf = malloc(file_mode.st_size+1);
	fread(enc_buf, 1, file_mode.st_size, data_file);

	fclose(data_file);

	dec_buf = encrypt_nl(enc_buf, file_mode.st_size);
	fwrite(dec_buf, 1, file_mode.st_size, tmp_output);
	fclose(tmp_output);

	data_file = fopen("/tmp/.nlidout","r");
	while(!feof(data_file))
	{
		char *line_buf = malloc(1024);
		char *filename = NULL;
		char *last_space = NULL;
		char *thash = NULL;
		char *hash = NULL;

		if(!fgets(line_buf, 1024, data_file)) /* Possible newline at the end of the file */
		{
			free(line_buf);
			break;
		}

		filename = line_buf;
		last_space = strstr(line_buf, " ");

		if(!last_space)
		{
			fprintf(stderr, "Failed scanning file, possibly corrupted\n");
			free(line_buf);
			return;
		}

		while(!hash)
		{
			char *buf = NULL;
			if((buf = strstr(last_space," "))==NULL) /* There are no more spaces so! */
			{
				filename[last_space-filename-1] = '\0';
				hash = last_space;
				hash[strlen(hash)-1] = '\0';
			}
			else
				last_space = buf+1;
		}

		thash = sha1_file(filename);
		if(thash) /* Might be a protected file, or it might not exist! */
		{
			if(verbose)
				printf("Checking %s - %s against %s\n",filename, thash, hash);
			if(strcmp(thash,hash)!=0)
			{
				printf("%s has been modified\n",filename);
			}
			free(thash);
		}
		else
		{
			printf("%s couldn't be open, may of been deleted\n",filename);
		}
		free(line_buf);
		unlink("/tmp/.nlidout");
	}
}

static void add_file(char *file, int recursive)
{
	char buf[16] = { '\0' }; /* Buffing of a different nature! */
	struct stat file_mode;
	char *hash = NULL;
	char *ftwrite = NULL;
	char *encrypted = NULL;
	memset(buf,' ', (recursive-1)*2);

	if((lstat(file, &file_mode)) == -1)
	{
      fprintf(stderr,"%sCouldn't add %s\n",buf,file);	
		return;
	}

	if(S_ISDIR(file_mode.st_mode))
	{
		struct dirent *dir = NULL;
		DIR *c_dir = NULL;

		if(recursive>=16)  /* 16 levels of recursion */
		{
			fprintf(stderr,"%sHit 16 levels of recursion- stopping here\n",buf);
			return;
		}

		if(verbose)
			printf("%sInside %s\n",buf,file);
		c_dir = opendir(file);
		if(!c_dir) /* Empty dir... I hope */
			return;

		while((dir = readdir(c_dir))!=NULL)
		{
			if(strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0)
			{
				char *fname = malloc(strlen(file)+strlen(dir->d_name)+2); /* \0 and / */
				if(file[strlen(file)-1]!='/')
					sprintf(fname, "%s/%s",file, dir->d_name);
				else
					sprintf(fname, "%s%s",file, dir->d_name);
				add_file(fname, recursive+1);
				free(fname);
			}
		}
		return;
	}
	else if(!S_ISREG(file_mode.st_mode))
	{
		fprintf(stderr, "Not adding unregular file %s\n", file);
		return;
	}
	if(tmp_file==NULL)
	{
		struct stat tmp_file_mode;
		tmp_file = fopen(encrypted_file,"a+");

		if(!tmp_file)
		{
			fprintf(stderr, "Failed creating temporary file - fatal error\n");
			exit(0);
		}

		if(lstat(encrypted_file,&tmp_file_mode)==-1)
		{
			fprintf(stderr, "Failed statting the encrypted file - fatal error\n");
			exit(0);
		}
		if(tmp_file_mode.st_size>0)
		{
			char *file_contents = NULL;
			fseek(tmp_file,0,SEEK_SET);
			file_contents = malloc(tmp_file_mode.st_size+1);
			fread(file_contents,1,tmp_file_mode.st_size,tmp_file);
			decrypted_buf = encrypt_nl(file_contents, tmp_file_mode.st_size); /* SHOULD BE DECRYPRY!T */
			free(file_contents);
			fclose(tmp_file);
			tmp_file = fopen(encrypted_file, "w");
		}
	}

	hash = sha1_file(file);
	if(!hash)
	{
		fprintf(stderr, "Couldn't add %s - Failed to hash\n",file);
		return;
	}

	/* Check to see if the file is already being looked after */
	if(decrypted_buf)
	{
		char *offset = NULL;
		if((offset = strstr(decrypted_buf, file))!=NULL)
		{
			if(strncmp(offset, file, strlen(file)) == 0)
			{
				/* Now check to make sure that the hash is the same */
				if(strncmp(offset+strlen(file)+1,hash,40)!=0)
				{
					if(!updates)
					{
						fprintf(stderr, "%s the hash isn't the same as the one currently stored. To force an update use the `-u` command arguement.\n",file);	
						free(hash);
						return;
					}
					else
					{
						if(verbose) /* Since we will encrypt and write the decrypted text back to the file, we just memcpy the new hash. */
						{
							printf("Updating %s\n",file);
						}
						memcpy(offset+strlen(file)+1,hash,40);
						free(hash);
					}
					return;
				}
				else
				{
					printf("Hash is the same %s - %s\n",offset+strlen(file)+1, hash);
					free(hash);
					return;
				}
			}
			free(hash);
			return;
		}
	}

	ftwrite = malloc(strlen(file)+strlen(hash)+3);
	sprintf(ftwrite,"%s %s\n",file, hash);
	encrypted = encrypt_nl(ftwrite,strlen(file)+strlen(hash)+2);
	fwrite(encrypted,1,strlen(file)+strlen(hash)+2,tmp_file);
	if(verbose)
		printf("%sAdded %s\n",buf,file);

	free(encrypted);
	free(ftwrite);
	free(hash);
}

static void set_output(char *filename)
{
	if(strcmp(encrypted_file,"files.dat")==0)
	{
		encrypted_file = strdup(filename);
		if(verbose)
			printf("Saving to %s\n",filename);
	}
}

static void usage(char *prog_name)
{
	printf("Nonlogic Intrusion Detection System - Copyright Nonlogic 2008\n\n\tExample: %s -a file1 file2 ... fileN -f output.dat\n\n",prog_name);
	printf("Command Options:\n\n\t-a\tAdd a file. Can take a list of files including directories.\n");
	printf("\t-s\tScan for changes. Scans files from the given file (specified with -f) for changes on the system\n");
	printf("\t-f\tFile to save/load files to/from\n");
	printf("\t-v\tAdd's verboseness\n");
	printf("\t-u\tAllows existing file records to be updated.\n");
	printf("\t-h|-?\tThis help message\n\n");
	exit(0);
}

static void parse_args(int arg_count, char **arg_list)
{
	int i;
	char *root = (char*)get_current_dir_name(); /* Make sure relative names aren't copied */

	/* Primary scan through - Get output file and verbosity */
	int k;
	for(k = 0; k<arg_count;k++)
	{
		if(strcmp(arg_list[k],"-f")==0)
		{
			if(arg_list[k+1]!=NULL) /* They did specify a file */
			{
				set_output(arg_list[k+1]);
			}
		}
		else if(strcmp(arg_list[k],"-v")==0)
		{
			verbose = 1;
		}
		else if(strcmp(arg_list[k],"-u")==0)
		{
			updates = 1;
		}
	}

	for(i = 1;i<arg_count;i++)
	{
		if(arg_list[i][0] == '-')
		{
			switch(arg_list[i][1])
			{
				case 'h':
				case '?':
					usage(arg_list[0]);
					break;
				case 'd': /* Debug */
				{
					struct stat fstatm;
					char *enc_buf = NULL;
					char *dec = NULL;
					FILE *enc_hand = NULL;

					lstat(encrypted_file,&fstatm);
					enc_buf = malloc(fstatm.st_size);
					enc_hand = fopen(encrypted_file,"r");
					fread(enc_buf, 1, fstatm.st_size, enc_hand);
					dec = encrypt_nl(enc_buf, fstatm.st_size);
					printf("DEC:\n%s\n",dec);
					fclose(enc_hand);
					free(enc_buf);
					free(dec);
					}
					break;
				case 's': /* Scan for changes */
					scan_files();
					break;
				case 'a': /* Add files to security watch */
					while(i<arg_count)
					{
						i++;
						if(arg_list[i]!=NULL)
						{
							if(arg_list[i][0]!='-')
							{
								if(arg_list[i][0]!='/')
								{
									char *file_to_add = malloc(strlen(root)+strlen(arg_list[i])+2); /* \0 and / */
									sprintf(file_to_add,"%s/%s",root, arg_list[i]);
									add_file(file_to_add, 1);
									free(file_to_add);
								}
								else
								{
									add_file(arg_list[i], 1);
								}
							}
							else
							{
								i--;
								break;
							}
						}
						else
						{
							break;
						}
					}
					break;
				case 'f': /* Wanting to specify a new file to encrypt data in */
					if(arg_list[i+1]!=NULL)
					{
						set_output(arg_list[i+1]);
					}
					break;
				default:
					/* Invalid usage */
					break;
			}
		}
	}
}

int main(int argc, char **argv)
{
	if(argc!=1)
	{
		parse_args(argc, argv);
	}


	if(tmp_file!=NULL)
	{
		if(decrypted_buf)
		{
			int decrypted_len = strlen(decrypted_buf);
			char *encrypted_txt = encrypt_nl(decrypted_buf,decrypted_len);
			fwrite(encrypted_txt, 1, decrypted_len, tmp_file);	
			free(decrypted_buf);
			free(encrypted_txt);
		}
		fclose(tmp_file);
	}
	return 0;
}
