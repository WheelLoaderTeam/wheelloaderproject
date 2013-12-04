#include <stdint.h>

typedef struct EBUanalogOut{
	uint16_t channel[24];
} EBUanalogOut;

#define new_EBUanalogOut {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

void setAnalogOut(EBUanalogOut *packet, int channel, float value);
float getAnalogOut(EBUanalogOut *packet, int channel);

#define AO_1	0
#define AO_2	1
#define AO_3	2
#define AO_4	3
#define AO_5	4
#define AO_6	5
#define AO_7	6
#define AO_8	7
#define AO_9	8
#define AO_10	9
#define AO_11	10
#define AO_12	11
#define AO_13	12
#define AO_14	13
#define AO_15	14
#define AO_16	15
#define AO_17	16
#define AO_18	17
#define AO_19	18
#define AO_20	19
#define AO_21	20
#define AO_22	21
#define AO_23	22
#define AO_24	23
