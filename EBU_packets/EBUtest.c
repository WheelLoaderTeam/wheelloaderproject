#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "EBUrelays.h"
#include "EBUanalogIn.h"
#include "EBUanalogOut.h"


int main(void){
	
	//TODO: setup sockets
	
	//building relay packet
	EBUrelays relays = newEBUrelays();
	setRelay(&relays, R_A9, 1);
	setRelay(&relays, R_A10, 1);
	setRelay(&relays, R_A11, 1);
	setRelay(&relays, R_A12, 1);
	
	//TODO: send relay packet
	
	while(1){
		char buf[255];
		//TODO: wait for analog in packet
		EBUanalogIn analogIn = new_EBUanalogIn;
		//memcpy(buf, &analogIn, len); //copy data to type
		
		EBUanalogOut analogOut = new_EBUanalogOut;
		
		
		system("clear");
		float value;
		for(int i=0; i<24; i++){
			value = getAnalogIn(&analogIn, i);
			setAnalogOut(&analogOut, i, value );
			printf("A%d= %f\n", (i+1), value);
		}
		
		//TODO: send analog out packet
	}	
}
