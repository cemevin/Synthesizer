//TODO!!
/**
 * oscillator u iki kanala ayir.
 * iki farkli nota iki farkli step tutulacak. (notalar pitch shifte göre degisecek.)
 * LFOda volume ve pitche uygulanan oscillation ı ayrı ayrı parametrize et. (toplam 4 parametre olacak)
 * ucgen sawtooth sinus pulse ekle! (pulse ı 0.2 ye 0.8 lik kare olarak düşünebilirsin)
 * 
 * 
 * */


#include <fstream>
#include <iostream>

#include <pthread.h>
#include <stdio.h>

/* required for open() */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cmath>

#include "myaudio.h"


using namespace std;

/********** variables **********/


/********** END variables **********/

/********** system vars **********/



pthread_t		_soundThread;
bool			_QUIT = false;
unsigned char	_soundBuffer[2][BUFFER_LENGTH];
unsigned char	_soundBuffer2[FLUSH_BUFFER_LENGTH];
int				_currentBuffer = 0;
int				_bufferIndex = 0;

ofstream		ofs("output123.raw");

int _notes[] = {C4,CS4,D4,DS4,E4,F4,FS4,G4,GS4,A4,AS4,B4,C5};

double PULSE_WIDTH = 0.25;

/********** END system vars **********/




void osc(int note, unsigned char * buffer, int length);
void flush_buffer(ofstream & of, unsigned char * buffer, int length);
midi_input handleInput(int fileDes, unsigned char * buf, int length);
void writeToBuffer(unsigned char value);

void envelope_stop();
void envelope_attack();
void envelope_decay();
void envelope_sustain();
void envelope_release();
void envelope();

void LFO(){
	APPLIED_NOTE = NOTE;
	APPLIED_NOTE2 = NOTE2;
	APPLIED_VOLUME = VOLUME;
	PULSE_WIDTH = 0.25;
	
	double lfoLevel = sin(((double)LFO_STEP*LFO_FREQ)/SAMPLE*2.0*PI);
	
	
	//cout<<pow(2.0,lfoLevel)<<endl;
	if(LFO_FX[LFO_FX_PITCH] == true){
		APPLIED_NOTE = NOTE*pow(1.01,lfoLevel);
		APPLIED_NOTE2 = NOTE2*pow(1.01,lfoLevel);
	}
	if(LFO_FX[LFO_FX_VOLUME] == true){
		APPLIED_VOLUME = MAX(0,MIN(1,VOLUME*pow(1.1,lfoLevel)));
	}
	if(LFO_FX[LFO_FX_PULSE] == true){
		PULSE_WIDTH = (lfoLevel*0.25)+.5;
	}
	LFO_STEP++;
	LFO_STEP%=(int)(SAMPLE/LFO_FREQ);
}

unsigned char square(int note, int step);
void * soundThreadFunction (void * args);

int mapKeyToNote(unsigned char key){
	return _notes[key - 48];
}

int main()
{
	openSoundCard(SAMPLE);
	
	// open raw input for keyboard
	unsigned char buf[3];
	int fd = open("/dev/snd/midiC1D0",O_RDONLY);
	
	if(fd==-1){
		cerr<<"file cannot be read"<<endl;
		return 1;
	}
	
	// start the sound thread
	pthread_create(&_soundThread, NULL, soundThreadFunction, NULL);
    
    /*int pressLength = (int)SAMPLE/2;
    int releaseLength = 1;
    int noteLength = pressLength + releaseLength;*/
    
    setEnvelopeSettings(ISAMP*0.2, ISAMP, 0.8, ISAMP*0.5);
    setLFOSettings(1,true,true,true);
    
    midi_input tempInput;
    
    midi_input input1,input2;
    
    input1.key = 0;
    input2.key = 0;
    
    while(!_QUIT){
		
		//adamin yaptigi input algoritmasi:
		/*
		 * tus1 tus2 tut. 
		 * 
		 * bir tusa basildiginda:
		 * tus1 bossa tus1 = basilan; NOTE = basilannota;
		 * tus1 dolu tus2 bossa tus2 = basilan; NOTE = basilannota;
		 * ikisi de doluysa inputu umursama.
		 * 
		 * bir tustan el çekildiginde:
		 * tus1den çekildiyse:
		 * tus2 doluysa, NOTE=tus2;
		 * tus2 bossa, KEY_STATE = KEY_STATE_RELEASED
		 * 
		 * tus2den çekildiyse:
		 * NOTE = tus1;
		 * 
		 * else: umursama.
		 * 
		 * TODO: 2 taneliden stacke çevir!
		 *
		 * 
		 * 
		 * */
		
		tempInput = handleInput(fd,buf,3);
		
		if(tempInput.key == 1){
			_QUIT = true;
		}
		else{
			
			if(tempInput.pressed == KEY_STATE_PRESSED){
				if(input1.key == 0){
					input1 = tempInput;
					NOTE = mapKeyToNote(tempInput.key);
					KEY_STATE = KEY_STATE_PRESSED;
				}
				else if(input2.key == 0){
					input2 = tempInput;
					NOTE = mapKeyToNote(tempInput.key);
					KEY_STATE = KEY_STATE_PRESSED;
				}
			}
			else if(tempInput.pressed == KEY_STATE_RELEASED){
				if(input1.key == tempInput.key){
					input1.key = 0;
					
					if(input2.key == 0){
						KEY_STATE = KEY_STATE_RELEASED;
					}
					else{
						// shift input
						input1 = input2;
						input2.key = 0;
					}
				}
				else if(input2.key == tempInput.key){
					input2.key = 0;
					NOTE = mapKeyToNote(input1.key);
				}
			}
			
			NOTE2 = NOTE*2;
			/*
			if(tempInput.pressed == KEY_STATE_RELEASED){
				if(mapKeyToNote(tempInput.key) != NOTE){
					// i had already pressed another button before, so this release should have no effect
					// do nothing	
				}
				else{
					KEY_STATE = tempInput.pressed;
					NOTE = mapKeyToNote(tempInput.key);
				}
			}
			else{
				KEY_STATE = tempInput.pressed;
				NOTE = mapKeyToNote(tempInput.key);
			}*/
		}
	}
	
	// end the sound thread
	pthread_join(_soundThread, NULL);
	
	close(fd);
	ofs.close();
	
	closeSoundCard();
}

/*
void osc(int note, unsigned char * buffer, int length){
    int note2 = note/2;
    
    for(int i=0; i<length; i++)
    {
        if(i%note < note2){
            buffer[i] = 0xff;
        }
        else{
            buffer[i] = 0x00;
        }
    }
}*/

void flush_buffer(ofstream & of, unsigned char * buffer, int length){
    for(int j=0;j<length;j++){
        of<<(unsigned char)buffer[j];
    }
}



void * soundThreadFunction (void * args){
	int step,step2;
	unsigned char value;
	
	while(!_QUIT){
		/*if(IS_KEY_RELEASED && VOLUME == 0){
			continue;
		}*/
		value = (unsigned char)(0.4*pulse(APPLIED_NOTE, step++) + 0.6*pulse(APPLIED_NOTE2, step2++));
		
		if(step>=APPLIED_NOTE)step-=APPLIED_NOTE;
		if(step2>=APPLIED_NOTE2)step2-=APPLIED_NOTE2;
		
		writeToBuffer(value);
	}
	
	return NULL;
}

void writeToBuffer(unsigned char value){
	// step the envelope
	LFO();
	envelope();
	/*
	// change current buffer if the counter is end of the other one
	if(_bufferIndex == BUFFER_LENGTH){
		_bufferIndex = 0;
		_currentBuffer = 1 - _currentBuffer;
	}
	
	// put the note into the buffer
	_soundBuffer[_currentBuffer][_bufferIndex++] = (unsigned char)(value * VOLUME);
	*/
	
	_soundBuffer2[_bufferIndex++] = (unsigned char)(value * APPLIED_VOLUME);
	
	if(_bufferIndex >= FLUSH_BUFFER_LENGTH){
		
		soundCardPlay(_soundBuffer2, _bufferIndex);
		
		
		_bufferIndex = 0;
		
		/*for(int i = 0;i<FLUSH_BUFFER_LENGTH;i++){
			ofs<<_soundBuffer2[i];
		}*/
	}
	
	//cout<<(unsigned char)(value * VOLUME);
}



