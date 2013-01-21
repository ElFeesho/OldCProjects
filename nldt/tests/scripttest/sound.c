#include <lua.h>
#include <lauxlib.h>
#include <malloc.h>
#include <memory.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include "gizmos.h"

ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };

void sound_open(char *file, ALenum *format, ALvoid **data, ALsizei *size, ALsizei *freq, ALboolean *loop)
{
	OggVorbis_File vf;
	int eof = 0;
	int current_section;
	FILE *file_h = fopen(file,"rb");
	if(file_h == NULL)
	{
		fprintf(stderr,"failed to open %s for sound\n",file);
		return;
	}
	if(ov_open(file_h,&vf,0,0) < 0) 
	{
		fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
		return;
	}
	
	vorbis_info *vi=ov_info(&vf,-1);
	if(vi->channels == 2)
		*format = AL_FORMAT_STEREO16;
	else
		*format = AL_FORMAT_MONO16;
	*freq = vi->rate;
	*size = ov_pcm_total(&vf,-1)*(2*vi->channels); // 16 bit mono, 
	*loop = AL_TRUE;
	*data = malloc((*size));
	int pos = 0;
	char pcmout[4096];
	do
	{
		long ret=ov_read(&vf,pcmout,sizeof(pcmout),0,2,1,&current_section);
		if (ret == 0) {
			/* EOF */
			eof=1;
		} 
		else if (ret < 0) 
		{
			fprintf(stderr,"Read error?\n");
		}
		else
		{
			memcpy((*data)+pos,pcmout,ret);
			pos+=ret;
		}
	}while(!eof);
}

void sound_init()
{
	alutInit(NULL, NULL);
	alListenerfv(AL_POSITION, ListenerPos);
	alListenerfv(AL_ORIENTATION, ListenerOri);
	alListenerfv(AL_VELOCITY, ListenerPos);
}

void sound_deinit()
{
	alutExit();
}

int nldt_sound_load(struct lua_State *L)
{
	if(lua_gettop(L)!=1)
	{
		lua_pushstring(L,"ERROR: sound.load usage \"filename\"\n");
		lua_error(L);	
		return 0;
	}
	char *sound_file = (char*)lua_tostring(L,1);
	ALuint buffer;
	alGenBuffers(1, &buffer);
	ALenum format;
	ALsizei size;
	ALvoid *data;
	ALsizei freq;
	ALboolean loop;
	sound_open(sound_file,&format,&data,&size,&freq,&loop);
	alBufferData(buffer, format, data, size, freq);
	lua_newtable(L);
	lua_pushnumber(L,(int)buffer);
	lua_setfield(L,-2,"buf_id");
	lua_pushnumber(L,1.0);
	lua_setfield(L,-2,"pitch");
	lua_pushnumber(L,0);
	lua_setfield(L,-2,"src_id");
	return 1;
}

int nldt_sound_play(struct lua_State *L)
{
	if(lua_gettop(L)!=1)
	{
		lua_pushstring(L,"ERROR: sound.play usage sound_id\n");
		lua_error(L);	
		return 0;	
	}
	
	if(lua_isnil(L,1))
	{
		fprintf(stderr,"ERROR arguements passed are NIL!\n");
		return 0;
	}
	lua_getfield(L,1,"buf_id");
	ALuint buffer = (ALuint)luaL_checkint(L,-1);
	lua_getfield(L,1,"pitch");
	ALfloat pitch = (ALfloat)luaL_checknumber(L,-1);
	ALuint source;
	alGenSources(1,&source);
	lua_pushnumber(L,source);
	lua_setfield(L,1,"src_id");
	alSourcei (source, AL_BUFFER,   buffer   );
	alSourcef (source, AL_PITCH,    pitch     );
	alSourcef (source, AL_GAIN,     1.0f     );
	alSourcefv(source, AL_POSITION, ListenerPos);
	alSourcefv(source, AL_VELOCITY, ListenerPos);
	alSourcei (source, AL_LOOPING,  AL_FALSE);
	alSourcePlay(source);
	return 0;
}

int nldt_sound_stop(struct lua_State *L)
{
	if(lua_gettop(L)!=1)
	{
		lua_pushstring(L,"ERROR: sound.play usage sound_id\n");
		lua_error(L);	
		return 0;	
	}
	
	if(lua_isnil(L,1))
	{
		fprintf(stderr,"ERROR arguements passed are NIL!\n");
		return 0;
	}
	lua_getfield(L,1,"src_id");
	ALuint source = (ALuint)luaL_checkint(L,-1);
	if(source)
		alSourceStop(source);
	return 0;
}

const luaL_reg nldt_sound_methods[] =
{
	{"load", nldt_sound_load },
	{"play", nldt_sound_play },
	{"stop", nldt_sound_stop },
	{0, 0}
};
