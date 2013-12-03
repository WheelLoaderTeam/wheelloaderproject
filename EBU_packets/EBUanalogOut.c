#include "EBUanalogOut.h"

void setAnalogOut(EBUanalogOut *packet, int channel, float value){
	uint16_t intValue;
	
	if(value > 5){
		intValue = 65535; // Maximum value of a 16 bit integer.
	} else if(value < 0){
		intValue = 0;
	}else{
		intValue = ((value/5) * 65535 + 0.5); //Rounding conversion from float to int.
	}
	
	packet->channel[channel] = intValue;	
}

float getAnalogOut(EBUanalogOut *packet, int channel){
	
	uint16_t intValue = packet->channel[channel];
	
	float value = (((float)intValue/65535) * 5);
	return value;
}
