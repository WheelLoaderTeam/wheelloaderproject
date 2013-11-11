#include relayPacket.h

void setRelay(EBU_relay_packet *packet, int relay, int relayValue){
	int byte = relay / 8;
	int offset = relay % 8;
	
	uint8_t mask = 0x80 >> offset;
	
	uint8_t value = 0;
	if  relayValue {
		value = 0xFF;
	}
	
	packet->channel[byte] = (packet->channel[byte] & ~mask) | (value & mask);
	
}

int getRelay(EBU_relay_packet *packet, int relay){
	int byte = relay / 8;
	int offset = relay % 8;
	
	uint8_t mask = 0x80 >> offset;
	
	if (packet->channel[byte] & mask) {
		return 1;
	} else {
		return 0;
	}
}