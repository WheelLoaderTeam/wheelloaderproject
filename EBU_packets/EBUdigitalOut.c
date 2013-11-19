#include "EBUdigitalOut.h"

void setDigitalOut(EBUdigitalOut *packet, int channel, int value){
	int byte = channel / 8;
	int offset = channel % 8;
	
	uint8_t mask = 0x01 << offset;
	
	packet->channel[byte] = (value)?(packet->channel[byte] | mask):(packet->channel[byte] & ~mask);
	
}

uint8_t getDigitalOut(EBUdigitalOut *packet, int channel){
	int byte = channel / 8;
	int offset = channel % 8;
	
	uint8_t mask = 0x01 << offset;
	
	return packet->channel[byte] & mask;
}
