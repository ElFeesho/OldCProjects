#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

#define THREADS 256

void *thread_entry(void *data)
{
	unsigned long int i;
	for(i=1000000000;i<15000000000;i++)
	{
		double res = sqrt(i);
	}
	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t threads[THREADS];
	int i;
	for(i = 0;i<THREADS;i++)
		pthread_create(&threads[i],NULL,thread_entry,NULL);
	for (i=0; i<THREADS; i++)
	{
		// This is stupid. Threads won't finish in a specific order
		pthread_join(threads[i], NULL);
		printf("Thread: %d done!\n",i);
	}
	return 0;
}

/*
real	10m32.482s - The Feesh! - 256 threads - Q9550
user	42m6.337s
sys	0m2.562s

real	15m14.051s -- The Feesh! - 256 threads - Q6600
user	60m44.184s
sys	0m1.575s
*/
