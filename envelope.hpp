#ifndef _ENVELOPE
#define _ENVELOPE

#define ENVELOPE_STATE_STOP 0
#define ENVELOPE_STATE_ATTACK 1
#define ENVELOPE_STATE_DECAY 2
#define ENVELOPE_STATE_SUSTAIN 3
#define ENVELOPE_STATE_RELEASE 4

#include "globals.hpp"

/********** variables **********/

float 	VOLUME 				= 0.;
float 	APPLIED_VOLUME 		= VOLUME;
int     ENVELOPE_ATTACK     = (int)SAMPLE/2;
int     ENVELOPE_DECAY      = 1;
float   ENVELOPE_SUSTAIN    = 0.;
int     ENVELOPE_RELEASE    = 1;
int	    ENVELOPE_STATE      = ENVELOPE_STATE_STOP;

/********** END variables **********/

void envelope_stop();
void envelope_attack();
void envelope_decay();
void envelope_sustain();
void envelope_release();
void envelope();

void setEnvelope(int attack, int decay, float sustain, int release, float volume = 0., int envelopestate = ENVELOPE_STATE_STOP){
	ENVELOPE_ATTACK = attack;
	ENVELOPE_DECAY = decay;
	ENVELOPE_SUSTAIN = sustain;
	ENVELOPE_RELEASE = release;
	VOLUME = volume;
	ENVELOPE_STATE = envelopestate;
}

void shiftEnvelopeState(){
    ENVELOPE_STATE++;
    ENVELOPE_STATE %= 5;
}

void setEnvelopeState(char state){
    ENVELOPE_STATE = state;
}

void envelope_stop(){
    if(KEY_STATE == KEY_STATE_PRESSED){
        shiftEnvelopeState();
        envelope_attack();
    }
}
void envelope_attack(){
	if(KEY_STATE == KEY_STATE_PRESSED){
		if(VOLUME == 1.){
			shiftEnvelopeState();
			envelope_decay();
		}
		else{
			VOLUME = MIN(1., VOLUME + (1.0)/ENVELOPE_ATTACK);
		}
	}
	else{
		setEnvelopeState(ENVELOPE_STATE_RELEASE);
        if(VOLUME == 0.){
            shiftEnvelopeState();
        }
        else{
            VOLUME = MAX(0., VOLUME - (1.0)/ENVELOPE_RELEASE);
        }
	}
}
void envelope_decay(){
	if(KEY_STATE == KEY_STATE_PRESSED){
		if(VOLUME == ENVELOPE_SUSTAIN){
			shiftEnvelopeState();
			envelope_sustain();
		}
		else{
			VOLUME = MAX(ENVELOPE_SUSTAIN, (VOLUME - (1.0)/ENVELOPE_DECAY));
		}
	}
	else{
		setEnvelopeState(ENVELOPE_STATE_RELEASE);
        if(VOLUME == 0.){
            shiftEnvelopeState();
        }
        else{
            VOLUME = MAX(0., VOLUME - (1.0)/ENVELOPE_RELEASE);
        }
	}
}

void envelope_sustain(){
	if(KEY_STATE == KEY_STATE_PRESSED){
		VOLUME = ENVELOPE_SUSTAIN;
	}
	else{
		setEnvelopeState(ENVELOPE_STATE_RELEASE);
        if(VOLUME == 0.){
            shiftEnvelopeState();
        }
        else{
            VOLUME = MAX(0., VOLUME - (1.0)/ENVELOPE_RELEASE);
        }
	}
}
void envelope_release(){
	if(KEY_STATE == KEY_STATE_PRESSED){
		shiftEnvelopeState();
		envelope_stop();
	}
	else{
		if(VOLUME == 0.){
            shiftEnvelopeState();
        }
        else{
            VOLUME = MAX(0., VOLUME - (1.0)/ENVELOPE_RELEASE);
        }
	}
}

void envelope(){
    switch(ENVELOPE_STATE){
        case ENVELOPE_STATE_STOP:
			envelope_stop();
            break;
        case ENVELOPE_STATE_ATTACK:
			envelope_attack();
            break;
        case ENVELOPE_STATE_DECAY:
            envelope_decay();
            break;
        case ENVELOPE_STATE_SUSTAIN:
			envelope_sustain();
            break;
        case ENVELOPE_STATE_RELEASE:
			envelope_release();
            break;
    }
}

#endif
