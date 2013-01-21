/*
	Copyright (c) 2008 - Christopher Sawczuk

	NOTE: MUSIC BUNDLED WITH OPEN STAGE IS ONLY FOR USE
	WITH OPEN STAGE.  

	If you like the featured music please consider purchasing
	the sound tracks (plus additional none-featured tracks)
	from Core//Mute.

	<GPL>

	</GPL>
*/

/*
	Song Engine
	
	The song format is copied pretty much from Stepmania's .SM file, but it is simplified.
	
	Measures are still used.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "song_eng.h"

static song_note *note_list = NULL;

/* Beats per Minute */
static float bpm = 120.0f;

/* How long (in seconds) it takes for the note to reach the bottom of the screen */
static float speed = 1.0f;

/* How long to wait before reading notes */
static float delay = 0.0f;

static char *artist;
static char *song_name;
static char *song_file;

static void song_parse_details(FILE *song_handle)
{
	char c_line[255];
	int done = 0;
	do
	{
		memset(c_line, 0, 255);
		fgets(c_line, 255, song_handle);
		if(strncmp(c_line,"#artist",7)==0)
		{
			c_line[strlen(c_line)-1] = '\0';
			artist = strdup(c_line+8);
		}
		else if(strncmp(c_line,"#track",6)==0)
		{
			c_line[strlen(c_line)-1] = '\0';
			song_name = strdup(c_line+7);
		}
		else if(strncmp(c_line,"#song",5)==0)
		{
			c_line[strlen(c_line)-1] = '\0';
			song_file = strdup(c_line+6);
		}
		else if(strncmp(c_line,"#bpm",4)==0)
		{
			sscanf(c_line,"#bpm %f\n",&bpm);
		}
		else if(strncmp(c_line,"#delay",6)==0)
		{
			sscanf(c_line, "#delay %f\n", &delay);
		}
		else if(strncmp(c_line,"#speed",6)==0)
		{
			sscanf(c_line, "#speed %f\n", &speed);
		}
		else if(strncmp(c_line,"#start",6)==0)
		{
			done = 1;
		}
	}while(!done);
	
	printf("Read in song data:\n");
	printf("\tArtist: %s\n", artist);
	printf("\tTrack:  %s\n", song_name);
	printf("\tBPM:    %f\n", bpm);
	printf("\tSpeed:  %f\n", speed);
	printf("\tDelay   %f\n", delay);
}

int song_eng_load(char *file_name)
{
	/* The current offset of the notes */
	long long c_off = 0;
	
	FILE *song_file = fopen(file_name, "r");
	if(!song_file)
	{
		fprintf(stderr, "Failed to load song file\n");
		return 1;
	}
	
	/*
		Read in some primary data!
	*/

	song_parse_details(song_file);
	
	/*
		Create the list of notes
	*/
	
	c_off += (delay*1000);
	
	while(!feof(song_file))
	{
		/*
			Start off by reading in a measure
		*/
		int line_count = 0;
		char **buffer = malloc(32*sizeof(char*));
		int done = 0;
		do
		{
			if(feof(song_file))
			{
				break;
			}
			char tmp[7] = { 0 };
			fgets(tmp,7,song_file);
			if(tmp[0] == '\n')
				continue;
			else if(feof(song_file))
				break;
			if(tmp[0]!=',')
			{
				buffer[line_count] = malloc(7);
				strcpy(buffer[line_count],tmp);
				line_count++;
			}
			else
			{
				done = 1;
			}
		}while(!done);
		
		/*
			Read in a measure, lets add the notes into the 
			note list.
		*/
		
		int i = 0;
		int k = 0;
		for(i = 0;i<line_count;i++)
		{
			/* Don't bother processing the line if it's just blank */
			if(strcmp(buffer[i],"00000")==0)
			{
				c_off += (60000/bpm)/(line_count);
				continue;
			}
			for(k = 0;k<5;k++)
			{
				if(buffer[i][k] == '0')
					continue;
				if(!note_list)
				{
					note_list = malloc(sizeof(song_note));
					note_list->colour = k;
					note_list->type = buffer[i][k] - '0';
					note_list->t_off = c_off+5000;
					note_list->played = 0;
					note_list->next = NULL;
				}
				else
				{
					song_note *tmp = note_list;
					while(tmp->next!=NULL)
						tmp = tmp->next;
					tmp->next = malloc(sizeof(song_note));
					tmp = tmp->next;
					tmp->colour = k;
					tmp->type = buffer[i][k] - '0';
					tmp->played = 0;
					tmp->t_off = c_off+5000;
					tmp->next = NULL;
				}
			}
			c_off += 60000/bpm/(line_count);
		}
	}
	fclose(song_file);
	
	/*
		0 Errors :D
	*/
	return 0;
}

song_note *song_eng_get_notes(void)
{
	return note_list;
}

long int song_eng_get_speed(void)
{
	return speed*1000;
}

char *song_eng_get_song(void)
{
	return song_file;
}
