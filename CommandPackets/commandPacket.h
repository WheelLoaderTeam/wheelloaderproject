#include <stdint.h>

typedef struct commandPacket {
	uint32_t packetId;
	uint32_t packetSize;
	float analog[9];
	uint32_t digital;
}commandPacket;

#define new_commandPacket {0,0,0,0,0,0,0,0,0,0,0,0}

//Analog channels
#define LEVER_LIFT	2
#define LEVER_TILT	3


//Digital channels TBD




