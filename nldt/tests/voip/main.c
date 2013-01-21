#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fmodex/fmod.h>

int main(int argc, char **argv)
{
	FMOD_SYSTEM *system  = 0;
	FMOD_SOUND *sound   = 0;
	FMOD_CHANNEL *channel = 0;
	FMOD_RESULT result;
	FMOD_CREATESOUNDEXINFO exinfo;
	
	result = FMOD_System_Create(&system);
	result = FMOD_System_SetOutput(system, FMOD_OUTPUTTYPE_OSS);
	result = FMOD_System_SetDriver(system, 0);
	result = FMOD_System_SetRecordDriver(system, 0);
	
	result = FMOD_System_Init(system, 32, FMOD_INIT_NORMAL, NULL);
	
	memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));

	exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	exinfo.numchannels = 1;
	exinfo.format = FMOD_SOUND_FORMAT_PCM16;
	exinfo.defaultfrequency = 44100;
	exinfo.length = exinfo.defaultfrequency * sizeof(short) * exinfo.numchannels * 1;	
	
	int key = 0;
	do
	{
		result = FMOD_System_CreateSound(system, 0, FMOD_2D | FMOD_SOFTWARE | FMOD_OPENUSER, &exinfo, &sound);
		static FMOD_CHANNEL *channel = 0;
		static int   looping   = 0;
		int          recording = 0;
		int          playing   = 0;
		unsigned int recordpos = 0;
		unsigned int playpos   = 0;
		unsigned int length;
		printf("Recording\n");
		result = FMOD_System_RecordStart(system, sound, looping);
		
		unsigned int ctime = time(NULL);
		while(ctime+5>time(NULL));
		
		result = FMOD_System_RecordStop(system);
		result = FMOD_System_PlaySound(system, FMOD_CHANNEL_REUSE, sound, 0, &channel);
		ctime = time(NULL);
		while(ctime+5>time(NULL));
		FMOD_Channel_Stop(channel);
		
	} while (1);
	
	//Shut down
	result = FMOD_Sound_Release(sound);
	result = FMOD_System_Release(system);
	
	return 0;
}
