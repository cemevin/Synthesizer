#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct{
	unsigned char key,velocity,pressed;
} midi_input;


midi_input readInput(int fileDes, unsigned char * buf, int length){
	read(fileDes, buf, length);
	midi_input ki = {buf[1],buf[2],buf[0]};
	
	return ki;
}
