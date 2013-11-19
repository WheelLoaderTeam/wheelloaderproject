#include "EBUrelays.h"

void setRelay(EBUrelays *packet, int relay, int value){
	int byte = relay / 8;
	int offset = relay % 8;
	
	uint16_t mask = 0x01 << offset;
	
	packet->channel[byte] = (value)?(packet->channel[byte] | mask):(packet->channel[byte] & ~mask);
	
}

uint8_t getRelay(EBUrelays *packet, int relay){
	int byte = relay / 8;
	int offset = relay % 8;
	
	uint8_t mask = 0x01 << offset;
	
	return packet->channel[byte] & mask;
}
