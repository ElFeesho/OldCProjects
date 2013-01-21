#include <stdio.h>
#include <setjmp.h>

void jump_func()
{
	jmp_buf env;
	int i = 0;
	setjmp(env);
	printf("Here\n");
	if(i == 0)
	{
		i=1;
		longjmp(env,0);
	}
}

int main(int argc, char **argv)
{
	jump_func();

	return 0;
}
