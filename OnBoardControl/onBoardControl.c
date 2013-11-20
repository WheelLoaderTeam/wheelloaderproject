
#include "onBoardControl.h"

int main(void){
//	setupEBU();
	//setup sockets to send packets to the EBU
	
	//initServerSocket to receve packets from simulator
	
	//wait for packet from simulator, indefinate timeout
	
	
	return 1;
}

void setupEBU(){
	struct sockaddr_in relays_socket;
	socklen_t slen = sizeof(struct sockaddr_in);	
	int s_relays;
	
	initClientSocket(25400, &s_relays, "10.0.0.2", &relays_socket);	
	
	//building relay packet
	EBUrelays relays = newEBUrelays();
	setRelay(&relays, R_A9, 1);
	setRelay(&relays, R_A10, 1);
	setRelay(&relays, R_A11, 1);
	setRelay(&relays, R_A12, 1);
	
	//send relay packet
	sendto(s_relays, (char*)&relays, sizeof(EBUrelays), 0, (struct sockaddr*) &relays_socket, slen);
}


