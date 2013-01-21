#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void convert_to_dec(char *binary_str);

int main(int argc, char **argv)
{
	if(argc<2)
	{
		return 0;
	}
	convert_to_dec(argv[1]);
	return 0;
}

void convert_to_dec(char *binary_str)
{
	int res = 0;
	int binary_len = strlen(binary_str);

	int i = 0;
	for(i = 0; i<binary_len; i++)
	{
		res += (binary_str[binary_len-(i+1)]=='0')?(0):(1<<i);
	}
	printf("Binary: %s - Dec: %d\n",binary_str, res);
}
