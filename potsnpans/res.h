#ifndef __RES_H__
#define __RES_H__

typedef enum
{
	BASS,
	CYM,
	HIHAT,
	HIHAT2,
	SNARE,
	CRASH,
	HITOM,
	MIDTOM,
	FLOORTOM,
	STICK
} drum_sounds;

// Resource loading thread func
int res_load_resources(void *data);

void res_loaded();

void res_draw_kit();

void res_play(drum_sounds sound, int channel);

#endif
