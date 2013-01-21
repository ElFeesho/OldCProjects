#include <stdio.h>
#include <sys/stat.h>
#include "scan.h"

int main(int argc, char **argv)
{
	if(argc!=2)
	{
		fprintf(stderr,"Usage:\n\t%s somefile\n",argv[0]);
		return 1;
	}

	struct stat filestat;
	if(stat(argv[1], &filestat)<0)
	{
		fprintf(stderr, "Couldn't open %s.\n", argv[1]);
		return -1;
	}

	printf("File: %s\n", argv[1]);
	printf("Size: %ld bytes\n", filestat.st_size);

	printf("Please enter the value you are looking for: ");
	unsigned short buf = fgetc(stdin); 
	if(buf == EOF)
	{
		fprintf(stderr, "Invalid input\n");
		return -1;
	}

	printf("Scanning...\n");
	scan_file(argv[1],buf);
	return 0;
}
