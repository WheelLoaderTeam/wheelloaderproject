#include <stdint.h>

typedef struct EBUanalogOut{
	uint16_t channel[24];
} EBUanalogOut;

#define new_EBUanalogOut {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

void setAnalogOut(EBUanalogOut *packet, int channel, float value);
float getAnalogOut(EBUanalogOut *packet, int channel);
