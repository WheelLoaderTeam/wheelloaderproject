
#include "onBoardControl.h"

#define LONG_TIMEOUT {2, 0}
#define SHORT_TIMEOUT {0, 100000000}

int main(void){
	//variable definitions:
	int bufferEmpty = 1;
	EBUanalogOut buffer;
	uint32_t lastPacketID;
	struct timespec timeOflastPacketArrival;
	
	struct timespec timeOflastPacketSent;
	
	//setup sockets to send packets to the EBU
	//send stop packet
	//send relay packet
	
	//initServerSocket to receve packets from simulator
	//setup filedescriptors for select to wait for.
	//set long timeout
	
	while(1){
		//wait for packet from simulator
		
		//if //timeout
			if(bufferEmpty){
				//send stop packet
				//set long timeout
			} else{ //buffer not empty
				//send packet in buffer
				//clear buffer
				//set long timeout
			}
		//else //incomming packet
			if(bufferEmpty){
				//if (time since last packet sent > short timeout)
					//send packet imediatly
					//set long timeout
				//else (time since last packet sent < short timeout)
					//put packet in buffer
					//set remaining time of short timeout
			} else{ //buffer not empty
				//determine witch packet should remain in buffer
				//set remaining time of short timeout
			}
	}
	return 1;
}


int commandPacket2EBUpacket(CommandPacket* commandPacket, EBUanalogOut* analogEBUpacket){
	
	
	
	return 1;
}
