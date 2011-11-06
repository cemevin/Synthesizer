#ifndef _MYAUDIO
#define _MYAUDIO

#include <stdio.h>
#include <alsa/asoundlib.h>

static char *device = "default";
snd_pcm_t *handle = NULL;
snd_pcm_sframes_t frames;

void openSoundCard(int samp){
	int err;
	
	if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		 printf("Playback open error: %s\n", snd_strerror(err));
		 exit(EXIT_FAILURE);
	}
	if ((err = snd_pcm_set_params(handle,
							   SND_PCM_FORMAT_U8 ,
							   SND_PCM_ACCESS_RW_INTERLEAVED,
							   1,
							   samp,
							   1,
							   50000)) < 0) {   /* 0.5sec */
		 printf("Playback open error: %s\n", snd_strerror(err));
		 exit(EXIT_FAILURE);
	}
}



int soundCardPlay(unsigned char * buf, int len){
	//writes soundcardbuffer to sound card
	//returns nr of frames written
	//returns <0 in case of error
	
	if(!handle)return -1;
	
	long int frames = snd_pcm_writei(handle, buf, len);
	
	if (frames < 0){
		snd_pcm_recover(handle, frames, 1);
	}
			
	return frames;
}

void closeSoundCard(){
	if(handle){
		snd_pcm_close(handle);
	}
}

#endif
