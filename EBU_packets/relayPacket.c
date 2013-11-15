#include "relayPacket.h"

void setRelay(EBU_relay_packet *packet, int relay, int relayValue){
	int byte = relay / 8;
	int offset = relay % 8;
	
	uint8_t mask = 0x80 >> offset;
	
	packet->channel[byte] = (relayValue)?(packet->channel[byte] | mask):(packet->channel[byte] & ~mask);
	
}

uint8_t getRelay(EBU_relay_packet *packet, int relay){
	int byte = relay / 8;
	int offset = relay % 8;
	
	uint8_t mask = 0x80 >> offset;
	
	return packet->channel[byte] & mask;
}
