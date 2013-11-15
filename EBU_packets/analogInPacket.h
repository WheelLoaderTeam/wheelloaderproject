#include <stdint.h>

typedef struct EBU_analog_in_packet {
	uint16_t channel[24];
}EBU_analog_in_packet;

//TODO: define initial values of a new EBU_analog_in_packet.

void setAnalogIn(EBU_analog_in_packet *packet, int channel, float value);
float getAnalogIn(EBU_analog_in_packet *packet, int channel);
