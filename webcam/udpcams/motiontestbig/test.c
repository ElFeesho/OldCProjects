#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double e(double accuracy)
{
        double retx = 0.0;
        double n;
        for ( n = 0; n < accuracy; n++)
        {
                retx += pow( 1 + (1/n), n);
        }
        return retx;
}

int main(int argc, char **argv)
{
	printf("This just in %f\n",e(1111212048.0));
	return 0;
}
