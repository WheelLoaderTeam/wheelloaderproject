#include <stdint.h>

typedef struct commandPacket {
	uint23_t packetId;
	uint23_t packetSize;
	float analog[9];
	uint23_t digital;
}commandPacket;

#define new_commandPacket {0,0,0,0,0,0,0,0,0,0,0,0}

//Analog channels
#define PEDAL_1		0
#define PEDAL_2		1
#define PEDAL_3		2
#define CLC_LEVER_1	3
#define CLC_LEVER_2	4
#define CLC_LEVER_3	5
#define CLC_LEVER_4	6
#define STEERING_WHEEL	7
#define CDC_STEERING	8

//Digital channels TBD




