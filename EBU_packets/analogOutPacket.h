


typedef struct EBU_analog_out_packet{
	uint16_t channel[24];
} EBU_analog_out_packet;

//TODO: define initial values of EBU_analog_out_packet.

void setAnalogOut(EBU_analog_out_packet *packet, int channel, float value);
float getAnalogOut(EBU_analog_out_packet *packet, int channel);
