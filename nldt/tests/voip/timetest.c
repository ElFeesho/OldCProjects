#include <stdio.h>
#include <time.h>

int main(int argc, char **argv)
{
	printf("Timetest\n");
	while(1)
		printf("%d\n",time(NULL));
	return 0;
}
