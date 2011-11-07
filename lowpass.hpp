#ifndef _LOWPASS
#define _LOWPASS

#include "moog.cpp"

//carry these to another file called Lowpass.hpp
MoogFilter mf;
float lowpassfreq = 600.; //between 100 and 8000
float lowpassres = 0.;
float appliedlpfreq = lowpassfreq;
float appliedlpres = lowpassres;
Lowpass lp(600,0.);
bool lpenabled=false;

void setLP(float freq, float res){
	lowpassfreq = freq;
	lowpassres = res;
	
	mf.setCutoff(lowpassfreq);
	mf.setRes(lowpassres);
}

#endif
