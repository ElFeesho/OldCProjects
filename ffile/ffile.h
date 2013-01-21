#ifndef __FFILE_H__
#define __FFILE_H__

#include <stdint.h>

#define FFILE_FILE_TYPE		0x01
#define FFILE_FOLDER_TYPE	0x02

typedef struct _ffile_handle_t
{
	FILE *file;
} ffile_handle_t;

typedef struct _ffile_header_t
{
	char magic_num[8];
	int32_t version;
	char checksum[32];
	int32_t start;
} ffile_header_t;

typedef struct _ffile_ent_t
{
	int32_t type;
	int32_t length;
	char name[64];
} ffile_ent_t;

typedef struct _ffile_footer_t
{
	char checksum[32];
} ffile_footer_t;

ffile_handle_t *ffile_new(char *filename);
void ffile_close(ffile_handle_t *ffile);
file_header_t *ffile_header_new();
void *ffile_file_new(char *name);
void *ffile_folder_new(char *name);
file_footer_t *ffile_footer_new();
void ffile_insert_header(ffile_handle_t *ffile, ffile_header_t *header);
void ffile_insert_folder(ffile_handle_t *ffile, ffile_ent_t *folder, char *dest);
void ffile_insert_file(ffile_handle_t *ffile, ffile_ent_t *file, char *filename, char *dest);

#endif
