#include <stdint.h>

typedef struct EBUanalogIn {
	uint16_t channel[24];
}EBUanalogIn;

#define new_EBUanalogIn {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

void setAnalogIn(EBUanalogIn *packet, int channel, float value);
float getAnalogIn(EBUanalogIn *packet, int channel);
