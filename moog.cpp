#ifndef _MOOG
#define _MOOG

#include "globals.hpp"
#include <cmath>

class Lowpass{
	public:
		Lowpass(float freq, float reson){
			fc = freq;
			res = reson;
			
			setRate(SAMPLE);
			reset();
		}
		void reset(){
			d1 = d2 = d3 = d4 = 0;
		}
		void setRate(int r){
			rate = (float) r; 
			nyquist = rate/(float)2;
		}
		void setRes(float r){
			res = r;
		}
		void setFreq(float f){
			fc = f;
		}
		
		float process(float val){
			#define SAFE 0.99f // filter is unstable _AT_ PI
			if (fc>(nyquist*SAFE))
				fc=nyquist*SAFE;
			if (fc<10)
				{fc = 10;/*d1=d2=d3=d4=0;*/}
			float w = (fc/(float)rate); // cutoff freq [ 0 <= w <= 0.5 ]
			
			// find final coeff values for end of this buffer
			register double k, k2, bh, a0, a1, a2, b1, b2;
			double r = 2*(1-res);
			if(r==0.0) r = 0.001;
			k=tan(w*PI);
			k2 = k*k;
			bh = 1 + (r*k) + k2;
			a0 = a2 = double(k2/bh);
			a1 = a0 * 2;
			b1 = double(2*(k2-1)/-bh);
			b2 = double((1-(r*k)+k2)/-bh);
			
			float x = val;
			// first 2nd-order unit
			float y = ( a0*x ) + d1;
			d1 = d2 + ( (a1)*x ) + ( (b1)*y );
			d2 = ( (a2)*x ) + ( (b2)*y );
			x=y;
			// and the second
			
			y = ( a0*x ) + d3;
			d3 = d4 + ( a1*x ) + ( b1*y );
			d4 = ( a2*x ) + ( b2*y );
			
			return (float) y;
		}
		
		float fc,res;
		float rate;
		float nyquist;
		double k, k2, bh, a0, a1, a2, b1, b2,d1,d2,d3,d4;
		double r;
};

class MoogFilter
{
public:
	MoogFilter();
	void init();
	void calc();
	float process(float x);
	~MoogFilter();
	float getCutoff();
	void setCutoff(float c);
	float getRes();
	void setRes(float r);
protected:
	float cutoff;
	float res;
	float fs;
	float y1,y2,y3,y4;
	float oldx;
	float oldy1,oldy2,oldy3;
	float x;
	float r;
	float p;
	float k;
};

MoogFilter::MoogFilter()
{
	fs=SAMPLE;
	init();
}

MoogFilter::~MoogFilter()
{
}

void MoogFilter::init()
{
	// initialize values
	y1=y2=y3=y4=oldx=oldy1=oldy2=oldy3=0;
	calc();
};

void MoogFilter::calc()
{
	float f = (cutoff+cutoff) / fs; //[0 - 1]
	p=f*(1.8f-0.8f*f);
	k=p+p-1.f;

	float t=(1.f-p)*1.386249f;
	float t2=12.f+t*t;
	r = res*(t2+6.f*t)/(t2-6.f*t);
};

float MoogFilter::process(float input)
{
	// process input
	x = input - r*y4;

	//Four cascaded onepole filters (bilinear transform)
	y1= x*p +  oldx*p - k*y1;
	y2=y1*p + oldy1*p - k*y2;
	y3=y2*p + oldy2*p - k*y3;
	y4=y3*p + oldy3*p - k*y4;

	//Clipper band limited sigmoid
	y4-=(y4*y4*y4)/6.f;

	oldx = x; oldy1 = y1; oldy2 = y2; oldy3 = y3;
	return y4;
}

float MoogFilter::getCutoff()
{ return cutoff; }

void MoogFilter::setCutoff(float c)
{ cutoff=c; calc(); }

float MoogFilter::getRes()
{ return res; }

void MoogFilter::setRes(float r)
{ res=r; calc(); }

#endif
