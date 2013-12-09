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
	
	while(1){
		char buf[255];
		EBUanalogIn analogIn = new_EBUanalogIn;
		
		//wait for analog in packet
		recvfrom(s_analog_in, buf, 255, 0, (struct sockaddr*) &analog_in_socket, &slen);
		memcpy(&analogIn, buf, sizeof(EBUanalogIn)); //copy data to type
		
		system("clear");
		float value;
		for(int i=0; i<24; i++){
			value = getAnalogIn(&analogIn, i);
			printf("A%d= %f\n", (i+1), value);
		}
		
	}
}
