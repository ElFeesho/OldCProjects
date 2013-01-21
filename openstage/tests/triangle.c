#include <stdio.h>

int main(int argc, char **argv)
{
	int i;
	int j;
	int k;
	/*
		Loop from 1 to 10
	*/
	for(i = 1;i<=10;i++)
	{
		for(j = 1;j<=i;j++)
		{
			printf("%d ",i*j);
		}
		printf("\n");
	}
	return 0;
}
