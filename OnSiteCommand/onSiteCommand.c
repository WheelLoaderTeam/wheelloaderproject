
#include "onSiteCommand.h"

int main(void){
	char rcvBuf[255];
	
	commandPacket comPacket;

	struct sockaddr_in commandInSocket, commandOutSocket;
	socklen_t slen = sizeof(struct sockaddr_in);	
	int s_commandInSocket, s_commandOutSocket;
	
	//setup socket to send to wheel loader
//	initClientSocket(CMDO_PORT, &s_commandOutSocket, WL_IP, &commandOutSocket);
	initClientSocket(CMDO_PORT, &s_commandOutSocket, "127.0.0.1", &commandOutSocket);

	//setup socket to receive packets from simulator
	initServerSocket(CMDI_PORT, &s_commandInSocket, &commandInSocket);
	
	while(1){
		printf("Waiting to receive Packet\n");
		recvfrom(s_commandInSocket, rcvBuf, 255, 0, (struct sockaddr*) &commandInSocket, &slen);
		memcpy(&comPacket, rcvBuf, sizeof(commandPacket));
		printf("Packet %u recevied\n", comPacket.packetId);
		sendto(s_commandOutSocket, (char*)&comPacket, sizeof(commandPacket), 0, (struct sockaddr*) &commandOutSocket, slen);
		printf("Packet %u Sent\n\n", comPacket.packetId);
	}
	return 0;
}

