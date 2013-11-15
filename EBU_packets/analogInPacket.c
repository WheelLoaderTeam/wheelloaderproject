#include analogInPacket.h

void setAnalogIn(EBU_analog_in_packet *packet, int channel, float value){
	uint16_t intValue;
	
	if(value > 5){
		intValue = 4095; // Maximum value of a 12 bit integer.
	} else if(value < 0){
		intValue = 0;
	}else{
		intValue = ((value/5) * 4095 + 0.5); //Rounding conversion from float to int.
	}
	
	packet->channel[channel] = intValue;	
}

float getAnalogIn(EBU_analog_in_packet *packet, int channel){
	
	uint16_t intValue = packet->channel[channel];
	
	float value = ((intValue/4095) * 5);
	return value;
}
