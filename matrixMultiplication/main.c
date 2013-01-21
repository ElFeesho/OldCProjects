#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *generate_question()
{
	char *answer = NULL;
	int matrix_a[2], matrix_b[2];
	matrix_a[0] = (rand()%4)+1; //rows
	matrix_a[1] = (rand()%4)+1; //columns

	matrix_b[0] = (rand()%4)+1;
	matrix_b[1] = (rand()%4)+1;

	int matrix_a_els[matrix_a[0]][matrix_a[1]];
	int matrix_b_els[matrix_b[0]][matrix_b[1]];
	int i = 0;
	int j = 0;
	int k = 0;

	for(i = 0;i<matrix_a[0];i++)
	{
		for(j = 0;j<matrix_a[1];j++)
		{
			matrix_a_els[i][j] = rand()%10;
		}
	}

	for(i = 0;i<matrix_b[0];i++)
	{
		for(j = 0;j<matrix_b[1];j++)
		{
			matrix_b_els[i][j] = rand()%10;
		}
	}

	int max = (matrix_a[0]<=matrix_b[0])?(matrix_b[0]):(matrix_a[0]);
	int chars_moved = 2;
	int cchars_moved = 2;
	for(i = 0;i<max;i++)
	{
		if(i<matrix_a[0])
		{
			printf("[ ");
			for(j=0;j<matrix_a[1];j++)
			{
				printf("%d ",matrix_a_els[i][j]);
				cchars_moved += 2;
			}
			printf("] ");
			cchars_moved += 2;
			if(cchars_moved>chars_moved)
			{
				chars_moved = cchars_moved;
			}
		}
		else
		{
			for(k=0;k<chars_moved;k++,printf(" "));
		}
		if(i<matrix_b[0])
		{
			printf("[ ");

			for(j=0;j<matrix_b[1];j++)
			{
				printf("%d ",matrix_b_els[i][j]);
			}
			printf("]");
		}
		printf("\n");
		cchars_moved = 0;
	}

	while(fgetc(stdin)<0);

	printf("\nAnswer:");

	if(matrix_a[0]!=matrix_b[1])
	{
		printf("\nImpossible to multiply matrices, matrix A doesn't have the same amount of columns as matrix B has rows\n");
	}
	else
	{
		printf("Answer matrix size %dx%d\n\n",matrix_a[1],matrix_b[0]);
		// The fun part :D
		/*
			As Feesh understands it. You multiply rows by colums.. TADA

			But seriously if you have
			
			[ 1  2 ]  [ 4 5 7 ]  The answer will be [ 8  17 23 ] 
			[ 3  4 ]  [ 2 6 8 ]                     [ 20 39 53 ]

			Simple!.. Isn'tit!

			Makes it possible to just loop for the amount of rows in 
			matrix A whilst looping the amount of columns in matrix B, 
			whilst then multiplying and adding the end result and stashing
			it in a matrix :D
		*/

		int answer_matrix[matrix_a[0]][matrix_b[1]];
		for(i = 0;i<matrix_a[0];i++)
		{
			for(j=0;j<matrix_b[0];j++)
			{
				int ans = 0;
				for(k = 0; k<matrix_a[1];k++)
				{
					ans += matrix_a_els[i][k]*matrix_b_els[k][j];
				}
				answer_matrix[i][j] = ans;
			}
		}
		/* Debug, print the aswer out */
		answer = malloc(1024);
		memset(answer, '\0', 1024);
		for(i=0;i<matrix_a[0];i++)
		{
			printf("\n[ ");
			for(j=0;j<matrix_b[1];j++)
			{
				printf("%d ",answer_matrix[i][j]);
			}
			printf("]");
		}
		printf("\n");
	}
	return NULL;

}

int main(int argc, char **argv)
{
	printf("Matrix Multiplication Revision - by Feesh!\n\n");
	printf("Let's start- Questions will appear, you will need\n");
	printf("to answer them on a piece of paper, as you cannot\n");
	printf("input your answer directly into the program.\n\n");
	printf("You will receive a question, and then you will\n");
	printf("have to hit the return key to see the answer!\n\n");
	printf("Press CTRL+C to exit the program\n\n");

	srand(time(NULL));
	for(;;)
	{
		generate_question();	
		while(fgetc(stdin)<0);
	}
	return 0;
}
