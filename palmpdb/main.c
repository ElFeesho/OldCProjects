/* Terrible Hack for stripping PDBs- You heard me! */

#include <stdio.h>

typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned int UInt32;
typedef unsigned int LocalID;

#define dmDBNameLength 32

typedef struct {
	LocalID nextRecordListID;
	UInt16 numRecords;
	UInt16 firstEntry;
} RecordListType;

typedef struct {
	UInt8 name[dmDBNameLength];
	UInt16 attributes;
	UInt32 creationDate;
	UInt32 modificationDate;
	UInt32 lastBackupDate;
	UInt32 modificationNumber;
	LocalID appInfoID;
	LocalID sortInfoID;
	UInt32 type;
	UInt32 creator;
	UInt32 uniqueIDSeed;
	RecordListType recordList;
} DatabaseHdrType;

void read_pdb(char *filename)
{
	if(!filename)
		return;

	unsigned char img_data[35509];

	FILE *pdb_file = fopen(filename,"rb");
	
	DatabaseHdrType header;
	fread(&header,sizeof(DatabaseHdrType),1,pdb_file);
	fread(img_data,1,35509,pdb_file);
	fclose(pdb_file);

	FILE *dump_file = fopen(strcat(filename,".raw"),"w");
	fwrite(img_data,1,35509,dump_file);
	fclose(dump_file);
}

int main(int argc, char ** argv)
{
	read_pdb(argv[1]);
	return 0;
}
