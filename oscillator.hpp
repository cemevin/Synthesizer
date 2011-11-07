#ifndef _OSCILLATOR
#define _OSCILLATOR

#define SAWTOOTH 		0
#define SQUARE 			1
#define PULSE_NARROW	2
#define PULSE_WIDE		3
#define TRIANGLE		4	
#define SINE			5
#define NOISE			6
#define WAVETYPE_COUNT	7

//internal define
#define PULSE_WIDTH_NARROW 	0.25
#define PULSE_WIDTH_WIDE	0.75

#include <iostream>
#include <cmath>
#include <cstdlib>

/********** variables **********/

int 	_waveType[2] 	= {SAWTOOTH, TRIANGLE};
float 	_pitchShift[2]	= {1, 2};
float 	_mix[2]			= {0.5, 0.5}; //sum must not be greater than 1

/********** END variables **********/

void setOscillator(int channel, int wt, float ps, float m){
	if(channel>1 || wt>WAVETYPE_COUNT || m>1){
		std::cerr<<"OSCILLATOR: invalid params"<<std::endl;
		return;
	}

	_waveType[channel] = wt;
	_pitchShift[channel] = ps;
	_mix[channel] = m;
}

unsigned char square(int note, int step){
    int note2 = note*0.5;
    
    if(step%note < note2){
		return 0xff;
	}
	else{
		return 0x00;
	}
}



unsigned char noise(int note, int step){
    return 0xff * random();
}

unsigned char triangle(int note, int step){
    int note2 = note*0.5;
    
    if(step%note < note2){
		return 0xff*((float)step/note2);
	}
	else{
		return 0xff + 0xff*(1.0f - (float)(step)/note2);
	}
}

unsigned char sine(int note, int step){
	return 0x7f + 0x7f*sin(((double)step)/((double)note)*PI*2);
}

unsigned char pulse_narrow(int note, int step){
    int note22 = note*PULSE_WIDTH_NARROW;
    
    if(step%note < note22){
		return 0xff;
	}
	else{
		return 0x00;
	}
}

unsigned char pulse_wide(int note, int step){
    int note2 = note*PULSE_WIDTH_WIDE;
    
    if(step%note < note2){
		return 0xff;
	}
	else{
		return 0x00;
	}
}

unsigned char osc_step(int channel, int note, int step){
	switch(_waveType[channel]){
		case SQUARE:
			return square(note, step);
		case PULSE_NARROW:
			return pulse_narrow(note, step);	
		case PULSE_WIDE:
			return pulse_wide(note, step);
		case SINE:
			return sine(note, step);
		case TRIANGLE:
			return triangle(note, step);
		case NOISE:
			return noise(note, step);
		default:
			return square(note, step);
	}
}

#endif
