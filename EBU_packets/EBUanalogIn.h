#include <stdint.h>

typedef struct EBUanalogIn {
	uint16_t channel[24];
}EBUanalogIn;

#define new_EBUanalogIn {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

void setAnalogIn(EBUanalogIn *packet, int channel, float value);
float getAnalogIn(EBUanalogIn *packet, int channel);

#define AI_1	0
#define AI_2	1
#define AI_3	2
#define AI_4	3
#define AI_5	4
#define AI_6	5
#define AI_7	6
#define AI_8	7
#define AI_9	8
#define AI_10	9
#define AI_11	10
#define AI_12	11
#define AI_13	12
#define AI_14	13
#define AI_15	14
#define AI_16	15
#define AI_17	16
#define AI_18	17
#define AI_19	18
#define AI_20	19
#define AI_21	20
#define AI_22	21
#define AI_23	22
#define AI_24	23
