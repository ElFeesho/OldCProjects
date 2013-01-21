#ifndef __SONG_ENG_H__
#define __SONG_ENG_H__

typedef struct
{
	/* Which colour note is it */
	char colour;
	
	/* How long through the song does the note appear */
	long long t_off;
	
	/* What type of note is it? 1 normal, 2 hold down, 3 release hold, 4 tap note */
	char type;
	
	/*Has the note been hit? or even missed? */
	char played;
	/* Linked list pointer to next node */
	void *next;
} song_note;


int song_eng_load(char *file_name);
song_note *song_eng_get_notes(void);
long int song_eng_get_speed(void);
char *song_eng_get_song(void);

#endif
