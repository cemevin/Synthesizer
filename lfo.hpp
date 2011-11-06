#ifndef _LFO
#define _LFO

#define LFO_FX_VOLUME	0
#define LFO_FX_PITCH	1
//#define LFO_FX_PULSE	2
#define LFO_FX_COUNT	2

#include <iostream>
#include <cmath>
#include "globals.hpp"
#include "envelope.hpp"
/********** variables **********/

// array elements represent settings for {VOLUME, PITCH} respectively
bool	LFO_ENABLED[3]		= {false, false}; 
float	LFO_FREQ[3]			= {20,20};
float 	LFO_RANGE[3] 		= {1,1};

//internal
int	LFO_STEP[3]	= {0,0};

/********** END variables **********/

void setLFO(int setting, bool enabled, float freq, float range){
	if(setting>=LFO_FX_COUNT){
		std::cerr<<"LFO: invalid setting"<<std::endl;
	}
	
	LFO_ENABLED[setting] = enabled;
	LFO_FREQ[setting] = freq;
	LFO_RANGE[setting] = range;
}

void LFO(){
	float lfoLevel;
	
	APPLIED_NOTE[0] = NOTE[0];
	APPLIED_NOTE[1] = NOTE[1];
	APPLIED_VOLUME = VOLUME;
	
	if(LFO_ENABLED[LFO_FX_VOLUME] == true){
		lfoLevel = sin(((float)LFO_STEP[0]*LFO_FREQ[0])/SAMPLE*2.0*PI);
		APPLIED_VOLUME = MAX(0,MIN(1,VOLUME*pow(LFO_RANGE[0],lfoLevel)));
		
		LFO_STEP[0]++;
		LFO_STEP[0]%=(int)(SAMPLE/LFO_FREQ[0]);
	}
	if(LFO_ENABLED[LFO_FX_PITCH] == true){
		lfoLevel = sin(((float)LFO_STEP[1]*LFO_FREQ[1])/SAMPLE*2.0*PI);
		APPLIED_NOTE[0] = NOTE[0]*pow(LFO_RANGE[1],lfoLevel);
		APPLIED_NOTE[1] = NOTE[1]*pow(LFO_RANGE[1],lfoLevel);
		
		LFO_STEP[1]++;
		LFO_STEP[1]%=(int)(SAMPLE/LFO_FREQ[1]);
		
		//std::cout<<lfoLevel<<" "<<pow(LFO_RANGE[1],lfoLevel)<<std::endl;
	}
	
	
	
	/*
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
	LFO_STEP%=(int)(SAMPLE/LFO_FREQ);*/
}

#endif
