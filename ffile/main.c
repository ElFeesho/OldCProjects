#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ffile.h"

int main(int argc, char **argv)
{
	printf("Flat File Test\n");

	ffile_handle_t *handle = ffile_new("myflatfile.ff");
	if(!handle)
	{
		fprintf(stderr,"Failed opening myflatfile.ff\n");
		perror("IO");
		return 1;
	}
	ffile_insert_header(handle, ffile_header_new());
	ffile_ent_t *main_c = ffile_file_new("main.c");
	ffile_insert_file(handle,main_c,"main.c",NULL);
	free(main_c);
	ffile_ent_t *docs = ffile_folder_new("docs");
	ffile_insert_folder(handle, docs,NULL);

	ffile_ent_t *makefile = ffile_file_new("Makefile");
	ffile_insert_file(handle,makefile,"Makefile","docs");
	free(makefile);

	ffile_close(handle);
	return 0;
}
