#ifndef _SYNTH
#define _SYNTH

#include "globals.hpp"
#include "oscillator.hpp"
#include "lfo.hpp"
#include "envelope.hpp"
#include "myaudio.hpp"

#include <iostream>
using namespace std;



void writeToBuffer(unsigned char value){
	int result;
	
	_soundBuffer[_bufferIndex++] = (unsigned char)(value);
	
	if(_bufferIndex >= FLUSH_BUFFER_LENGTH){
		result = soundCardPlay(_soundBuffer, _bufferIndex);
		_bufferIndex = 0;
	}
	
	if(record)cout<<value;
}

void * soundThreadFunction (void * args){
	int step,step2,note1,note2;
	unsigned char value, value1, value2;
	
	while(!_QUIT){
		/*if(IS_KEY_RELEASED && VOLUME == 0){
			continue;
		}*/
		
		note1 = APPLIED_NOTE[CHANNEL_LEFT];
		note2 = APPLIED_NOTE[CHANNEL_RIGHT];
		
		//cerr<<note1<<" "<<note2<<endl;
		
		value1 = osc_step(CHANNEL_LEFT, note1, step++);
		value2 = osc_step(CHANNEL_RIGHT, note2, step2++);
		
		
		
		value = (unsigned char)MAX(0,MIN(255,(_mix[CHANNEL_LEFT]*value1 + _mix[CHANNEL_RIGHT]*value2)));
		
		
		if(step >= note1)step -= note1;
		if(step2 >= note2)step2 -= note2;
		
		// step the LFO and envelope
		LFO();
		envelope();
		
		writeToBuffer(value*APPLIED_VOLUME);
	}
	
	return NULL;
}

void start(){
	openSoundCard(SAMPLE);
	
	// start the sound thread
	pthread_create(&_soundThread, NULL, soundThreadFunction, NULL);
	
	// open raw input for keyboard
	unsigned char buf[3];
	int tnote;
	
	int fd = open("/dev/snd/midiC1D0",O_RDONLY);
	
	if(fd==-1){
		cerr<<"file cannot be read"<<endl;
		return;
	}
    
    midi_input tempInput;
    midi_input input1,input2;
    
    input1.key = 0;
    input2.key = 0;
    
    while(!_QUIT){
		
		tempInput = readInput(fd,buf,3);
		
		if(tempInput.key == 1){
			_QUIT = true;
		}
		else{
			
			if(tempInput.pressed == KEY_STATE_PRESSED){
				if(input1.key == 0){
					input1 = tempInput;
					tnote = mapKeyToNote(tempInput.key);
					KEY_STATE = KEY_STATE_PRESSED;
				}
				else if(input2.key == 0){
					input2 = tempInput;
					tnote = mapKeyToNote(tempInput.key);
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
					tnote = mapKeyToNote(input1.key);
				}
			}
			
			NOTE[CHANNEL_LEFT] = tnote*_pitchShift[CHANNEL_LEFT];
			NOTE[CHANNEL_RIGHT] = tnote*_pitchShift[CHANNEL_RIGHT];
			
			//cerr<<NOTE[CHANNEL_LEFT]<<" "<<NOTE[CHANNEL_RIGHT]<<endl;
			/*if(input1.key!=0 && input2.key!=0){
				NOTE[CHANNEL_LEFT] = mapKeyToNote(input1.key)*_pitchShift[CHANNEL_LEFT];
				NOTE[CHANNEL_RIGHT] = mapKeyToNote(input2.key)*_pitchShift[CHANNEL_RIGHT];
			}
			else if(input1.key!=0 && input2.key == 0){
				NOTE[CHANNEL_LEFT] = mapKeyToNote(input1.key)*_pitchShift[CHANNEL_LEFT];
				NOTE[CHANNEL_RIGHT] = mapKeyToNote(input1.key)*_pitchShift[CHANNEL_RIGHT];
			}*/
		}
	}
	
	// end the sound thread
	pthread_join(_soundThread, NULL);
	
	close(fd);
	
	closeSoundCard();
}

#endif
