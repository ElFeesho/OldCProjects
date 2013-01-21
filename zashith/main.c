#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>

int main()
{
        struct termios attr;
        tcgetattr(STDIN_FILENO, &attr);
        attr.c_lflag &= ~ECHO; /* definitely disables echo */
		  attr.c_lflag ^= ICANON;
        tcsetattr(STDIN_FILENO, TCSANOW, &attr);

        printf("PASSWORD: ");
        char ch;
        char store=1;
        while ( (ch=getchar()) != '\n' )
        {
                store = ch; /* place holder for an actual action */
                putchar('*');
                fflush(stdout);
                fflush(stdin);
                fflush(stderr);
        }
        putchar('\n');

        attr.c_lflag ^= ECHO; /* toggles echo */
        tcsetattr(STDIN_FILENO, TCSANOW, &attr);

        return(0);
}
