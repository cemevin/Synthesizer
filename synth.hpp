#ifndef _SYNTH
#define _SYNTH

#include "globals.hpp"
#include "oscillator.hpp"
#include "lfo.hpp"
#include "envelope.hpp"
#include "myaudio.hpp"
#include "globals.hpp"
#include "moog.cpp"
#include <iostream>
#include <stdlib.h>



using namespace std;

enum filterType {LOW_PASS, HIGH_PASS, BAND_PASS, BAND_STOP};

double *create1TransSinc(int windowLength, double transFreq, double sampFreq, enum filterType type)
{
	int n;

	// Allocate memory for the window
	double *window = (double *) malloc(windowLength * sizeof(double));
	if (window == NULL) {
		fprintf(stderr, "create1TransSinc: Could not allocate memory for window\n");
		return NULL;
	}

	if (type != LOW_PASS && type != HIGH_PASS) {
		fprintf(stderr, "create1TransSinc: Bad filter type, should be either LOW_PASS of HIGH_PASS\n");
		return NULL;
	}

	// Calculate the normalised transistion frequency. As transFreq should be
	// less than or equal to sampFreq / 2, ft should be less than 0.5
	double ft = transFreq / sampFreq;

	double m_2 = 0.5 * (windowLength-1);
	int halfLength = windowLength / 2;

	// Set centre tap, if present
	// This avoids a divide by zero
	if (2*halfLength != windowLength) {
		double val = 2.0 * ft;

		// If we want a high pass filter, subtract sinc function from a dirac pulse
		if (type == HIGH_PASS) val = 1.0 - val;

		window[halfLength] = val;
	}
	else if (type == HIGH_PASS) {
		fprintf(stderr, "create1TransSinc: For high pass filter, window length must be odd\n");
		return NULL;
	}

	// This has the effect of inverting all weight values
	if (type == HIGH_PASS) ft = -ft;

	// Calculate taps
	// Due to symmetry, only need to calculate half the window
	for (n=0 ; n<halfLength ; n++) {
		double val = sin(2.0 * PI * ft * (n-m_2)) / (PI * (n-m_2));

		window[n] = val;
		window[windowLength-n-1] = val;
	}

	return window;
}


float lopass(unsigned char input, float cutoff, unsigned char * output){
	unsigned char lo_pass_output = CLIP(output[0] + (cutoff*(input-output[0])));
	output[0] = lo_pass_output;
	return lo_pass_output;
}


double * h = create1TransSinc(FLUSH_BUFFER_LENGTH, 460, SAMPLE, LOW_PASS);

unsigned char * lopass2(){
	unsigned char * x = _soundBuffer;
	unsigned char * y = (unsigned char *)malloc(FLUSH_BUFFER_LENGTH * sizeof(unsigned char));
	
	int i,j,k,N = FLUSH_BUFFER_LENGTH;
	
	y[0] = x[0];
	
	for(i=1;i<N;i++){
		y[i]=0;
		for(j=0;j<i;j++){
			y[i] = CLIP(y[i] + h[N-j]*x[j]);
		}
	}
	
	return y;
}



unsigned char result[FLUSH_BUFFER_LENGTH];

void writeToBuffer(unsigned char value){
	_soundBuffer[_bufferIndex++] = (unsigned char)(value);
	
	int i,j;
	
	if(_bufferIndex >= FLUSH_BUFFER_LENGTH){
		/*mf.setCutoff(600.0);
		mf.setRes(0.);
		for(i=0;i<FLUSH_BUFFER_LENGTH;i++){
			result[i] = (unsigned char)CLIP(255.0 * mf.process(_soundBuffer[i]/255.0));
		}*/
		
		//soundCardPlay(result, _bufferIndex);
		soundCardPlay(_soundBuffer, _bufferIndex);
		_bufferIndex = 0;
	}
	
	if(record)cout<<value;
}



void * soundThreadFunction (void * args){
	int step,step2,note1,note2;
	unsigned char value, value1, value2;
	
	mf.setCutoff(600.0);
	mf.setRes(0.);
	
	
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
		
		if(lpenabled)
			//value = CLIP(127.0 + 127.0 * lp.process((value-127.0)/127.0));
			value = CLIP(127. + 127.0 * mf.process((value-127.)/127.0));
		
		
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
