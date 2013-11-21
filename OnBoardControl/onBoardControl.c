
#include "onBoardControl.h"

#define LONG_TIMEOUT {2, 0}
#define SHORT_TIMEOUT {0, 100000000}

int main(void){
	//setup sockets to send packets to the EBU
	//send stop packet
	//send relay packet
	
	//initServerSocket to receve packets from simulator
	//set long timeout
	
	while(1){
		//wait for packet from simulator
		
		//if timeout
			//if buffer empty
				//send stop packet
				//set long timeout
			//else buffer not empty
				//send packet in buffer
				//clear buffer
				//set long timeout
		//else if incomming packet
			//if buffer empty
				//if (time since last packet sent > short timeout)
					//send packet imediatly
					//set long timeout
				//else (time since last packet sent < short timeout)
					//put packet in buffer
					//set remaining time of short timeout
			//else buffer not empty
				//determine witch packet should remain in buffer
				//set remaining time of short timeout
	}
	return 1;
}

int commandPacket2EBUpacket(CommandPacket* commandPacket, EBUanalogOut* analogEBUpacket){
	
	
	
	return 1;
}
