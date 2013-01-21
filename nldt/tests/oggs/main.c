#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

char pcmout[4096];

int main(int argc, char ** argv)
{
	OggVorbis_File vf;
	int eof=0;
	int current_section;
	
	if(ov_open_callbacks(stdin, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0) 
	{
		fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
		exit(1);
	}
  
	{
		char **ptr=ov_comment(&vf,-1)->user_comments;
		vorbis_info *vi=ov_info(&vf,-1);
		while(*ptr)
		{
			fprintf(stderr,"%s\n",*ptr);
			++ptr;
		}
		fprintf(stderr,"\nBitstream is %d channel, %ldHz\n",vi->channels,vi->rate);
		fprintf(stderr,"\nDecoded length: %ld samples\n",(long)ov_pcm_total(&vf,-1));
		fprintf(stderr,"Encoded by: %s\n\n",ov_comment(&vf,-1)->vendor);
		fprintf(stderr,"Size of pcmout: %d\n",sizeof(pcmout));
	}
	while(!eof)
	{
		long ret=ov_read(&vf,pcmout,sizeof(pcmout),0,2,1,&current_section);
		if (ret == 0) {
			/* EOF */
			eof=1;
		} 
		else if (ret < 0) 
		{
		}
		else
		{
			fwrite(pcmout,1,ret,stdout);
		}
	}
	return 0;
}