/*
	This was written by feesh.
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int pc = -1;
	int mp = 0;
	int ldepth = 0; /* loop depth */
	int stack[128]; /* Possibily 128 subloops */

	char cells[30000] = { '\0' };
	char *bf_prog = NULL;
	FILE *bf_file = fopen(argv[1],"r");
	fseek(bf_file,0,SEEK_END);	
	int len = ftell(bf_file);
	bf_prog = malloc(len);
	rewind(bf_file);
	fread(bf_prog,1,len,bf_file);
	fclose(bf_file);

	while(pc!=len)
	{
		switch(bf_prog[++pc])
		{
			case '+':
				cells[mp]++;
				break;
			case '-':
				cells[mp]--;
				break;
			case ',':
				cells[mp] = fgetc(stdin);
				break;
			case '[':
				if(!cells[mp])
				{
					int match_count = 1;
					while(match_count>0)
					{
						pc++;
						if(bf_prog[pc]=='[')
							match_count++;
						if(bf_prog[pc]==']')
							match_count--;
					}
				}
				else
				{
					stack[ldepth++] = pc;
				}
				break;
			case ']':
				if(ldepth>0)
				{
					ldepth--;
					pc = stack[ldepth]-1;
				}
				break;
			case '.':
				printf("%c",cells[mp]);
				break;
			case '>':
				if(mp<29999)
					mp++;
				else
					mp=0;
				break;
			case '<':
				if(mp>0)
					mp--;
				else
					mp = 29999;
				break;
			default:
				break;
		}
	}
	printf("\n");
	return 0;
}
