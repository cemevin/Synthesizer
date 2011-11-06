#include <iostream>
#include <fstream>
#include <string.h>

#include "globals.hpp"
#include "input.hpp"
#include "myaudio.hpp"
#include "synth.hpp"
#include "envelope.hpp"

using namespace std;

bool searchArg(char * arg, int argc, char ** argv){
	for(int i=1;i<argc;i++){
		if(strcmp(argv[i],arg)==0)return true;
	}
	return false;
}

int main(int argc, char ** argv)
{
	record = searchArg("-r", argc, argv);

	//set oscillators
	setOscillator(CHANNEL_LEFT, PULSE_NARROW, 1, 0.5);
	setOscillator(CHANNEL_RIGHT, PULSE_NARROW, 2.007, 0.5);

	//envelope settings
	setEnvelope(ISAMP*0.2, ISAMP, 0.8, ISAMP*0.5);
	
	//lfo settings
	setLFO(LFO_FX_VOLUME, true, 4, 1.1);
	setLFO(LFO_FX_PITCH, true, 1, 1.01);
	//cout<<_pitchShift[0]<<" "<<_pitchShift[1]<<endl;
	start();
	
	/*
	openSoundCard(SAMPLE);
	
	// open raw input for keyboard
	unsigned char buf[3];
	int tnote;
	
	int fd = open("/dev/snd/midiC1D0",O_RDONLY);
	
	if(fd==-1){
		cerr<<"file cannot be read"<<endl;
		return 1;
	}
	
	// start the sound thread
	pthread_create(&_soundThread, NULL, soundThreadFunction, NULL);

	//set oscillators
	setOscillator(CHANNEL_LEFT, PULSE_NARROW, 1, 0.5);
	setOscillator(CHANNEL_RIGHT, PULSE_WIDE, 2.007, 0.5);

	//envelope settings
	setEnvelope(ISAMP*0.2, ISAMP, 0.8, ISAMP*0.5);
    
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
		}
	}
	
	// end the sound thread
	pthread_join(_soundThread, NULL);
	
	close(fd);
	
	closeSoundCard();*/
}

