#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "relayPacket.h"

typedef struct testType{
	uint8_t testchannel[8];
	uint32_t testnumber;
} testType;

int main(void){
	
	char buf[255];
	
	EBU_relay_packet packet = new_EBU_relay_packet();
	setRelay(&packet, R_S12, 1);
	
	int r = getRelay(&packet, R_S12);
	printf("\n\nresult: %x\n\n", r);
	
	memcpy(buf, &packet, sizeof(EBU_relay_packet));
	
	printf("buf[0]: %x\n", buf[0]);
	printf("buf[1]: %x\n", buf[1]);
	printf("buf[2]: %x\n", buf[2]);
	printf("buf[3]: %x\n", buf[3]);
	printf("buf[4]: %x\n", buf[4]);
	printf("buf[5]: %x\n", buf[5]);
	printf("buf[6]: %x\n", buf[6]);
	printf("buf[7]: %x\n", buf[7]);
	printf("buf[8]: %x\n", buf[8]);
	printf("buf[9]: %x\n", buf[9]);
	printf("buf[10]: %x\n", buf[10]);
	printf("buf[11]: %x\n", buf[11]);
	printf("buf[12]: %x\n", buf[12]);
	printf("buf[13]: %x\n", buf[13]);
	
	
	
}
