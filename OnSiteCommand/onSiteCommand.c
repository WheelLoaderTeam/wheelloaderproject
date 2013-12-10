
#include "onSiteCommand.h"

int main(void){
	char rcvBuf[255];
	
	commandPacket comPacket;
	int packetId = 1;
	struct sockaddr_in commandInSocket, commandOutSocket;
	socklen_t slen = sizeof(struct sockaddr_in);	
	int s_commandInSocket, s_commandOutSocket;
	
	//setup socket to send to wheel loader
	initClientSocket(CMDO_PORT, &s_commandOutSocket, WL_WIRELESSIP, &commandOutSocket);
//	initClientSocket(CMDO_PORT, &s_commandOutSocket, "127.0.0.1", &commandOutSocket);

	//setup socket to receive packets from simulator
	initServerSocket(CMDI_PORT, &s_commandInSocket, &commandInSocket);
	
	while(1){
		recvfrom(s_commandInSocket, rcvBuf, 255, 0, (struct sockaddr*) &commandInSocket, &slen);
		memcpy(&comPacket, rcvBuf, sizeof(commandPacket));
		
		comPacket.packetId = packetId++;
		clock_gettime(CLOCK_REALTIME, &comPacket.timeSent);
		printf("sending packet\n");
		sendto(s_commandOutSocket, (char*)&comPacket, sizeof(commandPacket), 0, (struct sockaddr*) &commandOutSocket, slen);
	}
	return 0;
}

