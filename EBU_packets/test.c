#include <stdint.h>
#include <stdio.h>

#include "relayPacket.h"

typedef struct testType{
	uint8_t testchannel[8];
	uint32_t testnumber;
} testType;

int main(void){
	
	
	uint8_t channel[8];
	char *ch;
	
	//unsigned char channel[8];
	//unsigned int channel[8];
	
	channel[0] = 8;
	channel[1] = 9;
	channel[2] = 10;
	channel[3] = 231;
	channel[4] = 5;
	channel[5] = 6;
	channel[6] = 7;
	channel[7] = 8;
	
	ch = (char *) channel;
	
	printf("0 \"%x\"\n", *ch);
	printf("1 \"%x\"\n", *(ch + 1));
	printf("2 \"%x\"\n", *(ch + 2));
	printf("3 \"%x\"\n", *(ch + 3));
	
	printf("\n");
	
	printf("0 \"%d\"\n", &(*ch));
	printf("0 \"%d\"\n", &(*(ch + 1)));
	
	int len = sizeof(channel);
	printf("sizeof channel = %d\n", len);
	
	len = sizeof(ch);
	printf("sizeof ch = %d\n", len);
	
	len = sizeof(EBU_relay_packet);
	printf("sizeof EBU_relay_packet = %d\n", len);
	
	
	EBU_relay_packet packet = new_EBU_relay_packet();
	setRelay(&packet, R_S12, 1);
	int r = getRelay(&packet, R_S12);
	
	
	printf("\n\nresult: %d\n", r);
	
	
	
}
