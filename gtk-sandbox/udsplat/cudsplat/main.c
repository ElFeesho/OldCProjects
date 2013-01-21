#include <form.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

#define INPUT_BOX 0
#define DISP_BOX 1

/* global vars */
int udp_sock;
struct sockaddr_in udp_sock_addr;
struct sockaddr_in udp_sock_out;
int maxx,maxy;
char *dbuf;

/*       ___|^|__
  subs +(________) 
*/



void exit_error(char *reason)
{
	endwin();
	fprintf(stderr, reason);
	exit(0);
}

char *trimtail(char *inp)
{
	int i;
	for (i = strlen(inp) -1 ; i > 0; i-- )
	{
		if (( inp[i] == ' ')||(inp[i] == 0)) inp[i]=0;
		else break;
	}
	return(inp);
}

void add_message(FIELD *fld, char *name, char *message)
{
	int i;
	message = trimtail(message);
	char *formated_message = malloc( strlen(name) + strlen(message) + 10 );
	memset(formated_message, 0,  strlen(name) + strlen(message) + 10);
	
	sprintf(formated_message, "<%s> %s", name, message);
	
	strcat(dbuf,formated_message);

	char *pad = malloc( maxx - (strlen(formated_message) % maxx)+ 1);
	memset(pad, 32,  maxx - (strlen(formated_message) % maxx) + 1 );
	pad[  maxx - (strlen(formated_message) % maxx)  ] = 0;

	strcat(dbuf, pad);
	
	if (strlen(dbuf) > maxx * (maxy-1)){ 
		for (i=0; i < strlen(dbuf)- strlen(formated_message) - strlen(pad) ; i++)
			dbuf[i] = dbuf[strlen(formated_message) + strlen(pad) + i];
	}
	memset(&dbuf[ maxx * (maxy-1) ], 0, 600);
	set_field_buffer(fld, 0, dbuf);

	free(formated_message);
	free(pad);
}

void setup_socket(void)
{
	udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
	int broadcast = 1;
	if (setsockopt(udp_sock, SOL_SOCKET, SO_BROADCAST, &broadcast,
		sizeof broadcast) == -1) {
		perror("setsockopt (SO_BROADCAST)");
		exit(1);
	}

	udp_sock_addr.sin_family = AF_INET;
	udp_sock_addr.sin_port = htons(9348);
	memset(udp_sock_addr.sin_zero, 0, 8);
	inet_aton("255.255.255.255", &(udp_sock_addr.sin_addr));
	
	udp_sock_out.sin_family = AF_INET;
	udp_sock_out.sin_port = htons(18010);
	udp_sock_out.sin_addr.s_addr = INADDR_ANY;
	
	bind(udp_sock, (struct sockaddr*)&udp_sock_out, sizeof(udp_sock_out));
}

void send_message(char *name, char *message)
{
	/* do this prior to calling this function */
	sendto(udp_sock,"\x10",1,0,(struct sockaddr*)&udp_sock_addr, sizeof(struct sockaddr_in));
	sendto(udp_sock,name,16,0,(struct sockaddr*)&udp_sock_addr, sizeof(struct sockaddr_in));
	short len = htons(strlen(message));
	sendto(udp_sock,&len,2,0,(struct sockaddr*)&udp_sock_addr, sizeof(struct sockaddr_in));
	sendto(udp_sock,message,strlen(message),0,(struct sockaddr*)&udp_sock_addr, sizeof(struct sockaddr_in));

}


int main(void)
{
	initscr();
	cbreak();
	nodelay(stdscr, TRUE);
	noecho();
	keypad(stdscr, TRUE);
	raw();

	getmaxyx(stdscr, maxy,maxx);
	char name[16];
	memset(name, 0, 16);
	strncpy(name, getenv("USER"), 15);
	char bcast[16] = "255.255.255.255";

	fd_set rfds;
	struct timeval tv;
	int retval;
	
	setup_socket();
	
   tv.tv_sec = 0;
   tv.tv_usec = 10;
                      
	FIELD *pfield[2];
	pfield[INPUT_BOX] = new_field(1, maxx, maxy - 1, 0, 0, 1);
	pfield[DISP_BOX] = new_field(maxy - 1, maxx, 0, 0, 0, 1);
	pfield[2] = NULL;
	if ( ( pfield[INPUT_BOX] == NULL ) || ( pfield[DISP_BOX] == NULL ) ) exit_error("Could not create a field.");
	
	field_opts_on(pfield[INPUT_BOX], O_STATIC);
	field_opts_off(pfield[INPUT_BOX], O_AUTOSKIP);
	
	FORM *pform = new_form( pfield );
	
	if ( post_form( pform ) != E_OK ) exit_error("Could not post form.");
	
	refresh();
	
	int ch;
	dbuf = malloc(strlen(field_buffer(pfield[DISP_BOX],0))+600);
	memset(dbuf, 0, strlen(field_buffer(pfield[DISP_BOX],0)) +600);
	char *buf = malloc(600);
	memset(buf, 0, 600);
	struct message_in { char *name; char *msg; } msgin;
	for(;;)
	{
		FD_ZERO(&rfds);
		FD_SET(udp_sock, &rfds);

		retval = select(udp_sock+1, &rfds, NULL, NULL, &tv);
		if (retval)
		{
			/* we have a message! */
			/* for reference: 	
			//sendto(udp_sock,"\x10",1,0,(struct sockaddr*)&udp_sock_addr, sizeof(struct sockaddr_in));
			*/
			char header = 0;
			recv(udp_sock, &header,1,0);
			/* It's all good! The header is 16! */
			if(header == 16)
			{
				char nick[17] = { '\0' };
				recv(udp_sock, nick, 16, 0);
				short len = 0;
				recv(udp_sock, &len, 2, 0);
				len = ntohs(len);
				char *message = malloc(len+1);
				memset(message, 0, len+1);
				recv(udp_sock, message, len, 0);
				add_message( pfield[DISP_BOX], nick, message );
			}
		}
		
		ch = getch();
		if ( ch != ERR ) 
		{
			switch ( ch )
			{
				case KEY_ENTER:
				case 10:
					form_driver(pform, REQ_VALIDATION);
					if ( ! strncmp(field_buffer(pfield[INPUT_BOX],0),"/quit",5) ) exit_error("User quit.\n"); 
					if ( ! strncmp(field_buffer(pfield[INPUT_BOX],0),"/name",5) ) 
					{
						strncpy(name, trimtail(&(field_buffer(pfield[INPUT_BOX],0))[6]), 15);
						set_field_buffer( pfield[INPUT_BOX], 0, "");
						continue;
					}
					if ( ! strncmp(field_buffer(pfield[INPUT_BOX],0),"/bcast",5) ) 
					{
						strncpy(bcast, trimtail(&(field_buffer(pfield[INPUT_BOX],0))[6]), 15);
						set_field_buffer( pfield[INPUT_BOX], 0, "");
						continue;
					}

					send_message (name, trimtail(field_buffer(pfield[INPUT_BOX],0)));
					/* add_message( pfield[DISP_BOX], name, field_buffer(pfield[INPUT_BOX],0) ); Removed for great justice take off every zig*/
					set_field_buffer( pfield[INPUT_BOX], 0, "");
					form_driver(pform, REQ_SCR_FLINE);
				break;
				case KEY_LEFT:
					form_driver(pform, REQ_LEFT_CHAR);
				break;
				case KEY_RIGHT:
					form_driver(pform, REQ_RIGHT_CHAR);
				break;
				case KEY_BACKSPACE:
				case 0x08:		/* yeah, it's ugly, but it works. */
				case 0x7f:
					form_driver(pform, REQ_LEFT_CHAR);
					form_driver(pform, REQ_DEL_CHAR);
				break;
				default:
					form_driver(pform, ch);
				break;
			}
		}
		refresh();
		usleep(10000);
	}
	

	endwin();
	return(0);
}


