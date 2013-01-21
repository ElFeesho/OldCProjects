#include <stdio.h>
#include <curses.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

static struct sockaddr_in udp_sock_dst;

static void terminate(int signumber)
{
	/* Only really catch SIGINT */
	endwin();
	exit(0);
}

static int net_init()
{
	int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
	/* 
		setsockopt takes in an address to any useful data based on
		what we are settings, in this instance we are setting a
		flag to true, so we are passing in an int.
	*/

	int bc = 1;
	if (setsockopt(udp_sock, SOL_SOCKET, SO_BROADCAST, &bc, sizeof bc) == -1) 
	{
		mvprintw(LINES/2-1,COLS/2-strlen("Failed to initialise Network"),"Failed to initialise network");
		return -1;
	}

	udp_sock_dst.sin_family = AF_INET;
	udp_sock_dst.sin_port = htons(9348);
	inet_aton("255.255.255.255",&(udp_sock_dst.sin_addr));

	return udp_sock;
}

void print_header(bool modified)
{
	char *print_blank = malloc(6); /* "%XXXs\0" */
	sprintf(print_blank,"%%%ds",COLS);
	attron(A_REVERSE);
	mvprintw(0,0,print_blank," ");
	mvprintw(0,2,"GMU mamo 2.0.6");
	mvprintw(0,(COLS-16)/2,"File: UDsPlat");
	if(modified==TRUE)
	{
		mvprintw(0,COLS-10, "Modified");
	}
	attroff(A_REVERSE);
	move(1,0);
}

int main(int argc, char **argv)
{
	/*
		Hookup CTRL+C to 'terminate()', so the terminal's
		original settings are restored.
	*/

	signal(SIGINT, terminate);
	
	/* Initialise the curses lib */
	initscr();

	/* Catch keyboard activity on standard screen */
	keypad(stdscr, TRUE);

	/* no newlines, no line buffering and don't echo input! */
	nonl();
	cbreak();
	noecho();

	print_header(TRUE);
	int sock = net_init();
	if(sock==-1)
	{
		getch();
		terminate(0);
	}
	for(;;)
	{
		/* Get a character from strscr */
		int c = getch();
		print_header(TRUE);
		refresh();
		if(c == 65)
			break;
	}

	terminate(0);

	return 0;
}
