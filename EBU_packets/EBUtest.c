#define _XOPEN_SOURCE 600

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/time.h>
#include <time.h>

#include "EBUrelays.h"
#include "EBUanalogIn.h"
#include "EBUanalogOut.h"
#include "socket.h"



int main(void){
	struct timespec t;
	t.tv_sec = 0;
	t.tv_nsec = 30000000;
	
	//setup sockets
	struct sockaddr_in relays_socket, analog_in_socket, analog_out_socket, digital_in_socket, digital_out_socket;
	socklen_t slen = sizeof(struct sockaddr_in);	
	int s_relays, s_analog_in, s_analog_out, s_digital_in, s_digital_out;
	
	initServerSocket(25101, &s_analog_in, &analog_in_socket);
	initServerSocket(25301, &s_digital_in, &digital_in_socket);
	
	initClientSocket(25400, &s_relays, "10.0.0.2", &relays_socket);
	initClientSocket(25200, &s_analog_out, "10.0.0.2", &analog_out_socket);
	
	
	//building relay packet
	EBUrelays relays = newEBUrelays();
	setRelay(&relays, R_A9, 1);
	setRelay(&relays, R_A10, 1);
	setRelay(&relays, R_A11, 1);
	setRelay(&relays, R_A12, 1);
	
	
	//wait before sending. this is a workaround to a bug in the EBU
	//TODO: Remove nanosleep and timespec t when the EBU bug is fixed.
	nanosleep(&t, NULL);
	//send relay packet
	sendto(s_relays, (char*)&relays, sizeof(EBUrelays), 0, (struct sockaddr*) &relays_socket, slen);
	
	while(1){
		char buf[255];
		EBUanalogIn analogIn = new_EBUanalogIn;
		EBUanalogOut analogOut = new_EBUanalogOut;
		
		//wait for analog in packet
		recvfrom(s_analog_in, buf, 255, 0, (struct sockaddr*) &analog_in_socket, &slen);
		memcpy(&analogIn, buf, sizeof(EBUanalogIn)); //copy data to type
		
		system("clear");
		float value;
		for(int i=0; i<24; i++){
			value = getAnalogIn(&analogIn, i);
			setAnalogOut(&analogOut, i, value );
			//setAnalogOut(&analogOut, i, 2.5 );
			printf("A%d= %f\n", (i+1), value);
		}
		
		//wait before sending. this is a workaround to a bug in the EBU
		//TODO: Remove nanosleep and timespec t when the EBU bug is fixed.
		nanosleep(&t, NULL);
		//send analog out packet
		sendto(s_analog_out, (char*)&analogOut, sizeof(EBUanalogOut), 0, (struct sockaddr*) &analog_out_socket, slen);
	}
}
