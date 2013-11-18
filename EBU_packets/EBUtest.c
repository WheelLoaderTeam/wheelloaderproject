#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "EBUrelays.h"
#include "EBUanalogIn.h"
#include "EBUanalogOut.h"
#include "socket.h"


int main(void){
	
	
	//setup sockets
	struct sockaddr_in relays_socket, analog_in_socket, analog_out_socket;
	int slen = sizeof(struct sockaddr_in);	
	int s_relays, s_analog_in, s_analog_out;
	
	initServerSocket(25101, &s_analog_in, &analog_in_socket);
	
	initClientSocket(25400, &s_relays, "10.0.0.2", &relays_socket);
	initClientSocket(25400, &s_analog_out, "10.0.0.2", &analog_out_socket);
	
	
	//building relay packet
	EBUrelays relays = newEBUrelays();
	setRelay(&relays, R_A9, 1);
	setRelay(&relays, R_A10, 1);
	setRelay(&relays, R_A11, 1);
	setRelay(&relays, R_A12, 1);
	
	//send relay packet
	//sendto(s_relays, (char*)&relays, sizeof(EBUrelays), 0, (struct sockaddr*) &relays_socket, slen);
	
	while(1){
		char buf[255];
		EBUanalogIn analogIn = new_EBUanalogIn;
		EBUanalogOut analogOut = new_EBUanalogOut;
		
		//wait for analog in packet
		//recvfrom(s_analog_in, buf, 255, 0, analog_in_socket, slen);
		
		memcpy(buf, &analogIn, sizeof(EBUanalogIn)); //copy data to type
		
		system("clear");
		float value;
		for(int i=0; i<24; i++){
			value = getAnalogIn(&analogIn, i);
			setAnalogOut(&analogOut, i, value );
			printf("A%d= %f\n", (i+1), value);
		}
		
		//send analog out packet
		//sendto(s_analog_out, analogOut, sizeof(EBUrelays), 0, analog_out_socket, slen);
	}	
}
