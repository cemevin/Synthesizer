#ifndef _GLOBALS
#define _GLOBALS

#define PI 3.14159265
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) < (Y) ? (Y) : (X))
#define CLIP(X) MAX(0,MIN(255,(X)))

#define SAMPLE (192000.0)
#define ISAMP ((int)SAMPLE)
//#define BUFFER_LENGTH ((int)(SAMPLE))
#define FLUSH_BUFFER_LENGTH ((int)(SAMPLE/40.0))

#define C4 	(int)(SAMPLE/261.63)
#define CS4 (int)(SAMPLE/277.18)
#define D4 	(int)(SAMPLE/293.66)
#define DS4 (int)(SAMPLE/311.13)
#define E4 	(int)(SAMPLE/329.63)
#define F4 	(int)(SAMPLE/349.23)
#define FS4 (int)(SAMPLE/369.99)
#define G4 	(int)(SAMPLE/392.0)
#define GS4 (int)(SAMPLE/415.30)
#define A4 	(int)(SAMPLE/440.0)
#define AS4 (int)(SAMPLE/466.16)
#define B4 	(int)(SAMPLE/493.88)
#define C5 	C4/2

#define KEY_STATE_PRESSED 144
#define KEY_STATE_RELEASED 128
#define IS_KEY_RELEASED (KEY_STATE == KEY_STATE_RELEASED)
#define IS_KEY_PRESSED (KEY_STATE == KEY_STATE_PRESSED)

#define CHANNEL_LEFT	0
#define CHANNEL_RIGHT	1

#include <pthread.h>

/********** variables **********/

// input
int 			KEY_STATE 		= KEY_STATE_RELEASED;
int				NOTE[2]			= {C4, C4};
int 			APPLIED_NOTE[2]	= {C4, C4};

bool			_QUIT 			= false;
int				_bufferIndex 	= 0;
int 			_notes[] 		= {	C4*2,CS4*2,D4*2,DS4*2,E4*2,F4*2,FS4*2,G4*2,GS4*2,A4*2,AS4*2,B4*2,
									C5*2,CS4,D4,DS4,E4,F4,FS4,G4,GS4,A4,AS4,B4,C5};

pthread_t		_soundThread;
unsigned char	_soundBuffer[FLUSH_BUFFER_LENGTH];

bool			record			= false;

/********** END variables **********/

int mapKeyToNote(unsigned char key){
	return _notes[key - 48];
}

#endif




