#include <stdint.h>

typedef struct EBUdigitalIn{
	uint8_t channel[8];
} EBUdigitalIn;

#define newEBUdigitalIn() {0, 0, 0, 0, 0, 0, 0, 0}

#define SI17_EB52	0
#define SI18_EB51	1
#define SI19_EB50	2
#define SI20_EB49	3

#define SI21_EB48	4
#define KEY		5
#define dummy1		6
#define dummy2		7

#define SI9_EB60	8
#define SI10_EB59	9
#define SI11_EB58	10
#define SI12_EB57	11

#define SI13_EB56	12
#define SI14_EB55	13
#define SI15_EB54	14
#define SI16_EB53	15

#define SI1_EB68	16
#define SI2_EB67	17
#define SI3_EB66	18
#define SI4_EB65	19

#define SI5_EB64	20
#define SI6_EB63	21
#define SI7_EB62	22
#define SI8_EB61	23

#define DI33_HA68	24
#define DI34_HB1	25
#define DI35_HB2	26
#define DI36_HB3	27

#define DI37_HB4	28
#define DI38_HB5	29
#define DI39_HB6	30
#define EXTRA_D1	31

#define DI25_HA60	32
#define DI26_HA61	33
#define DI27_HA62	34
#define DI28_HA63	35

#define DI29_HA64	36
#define DI30_HA65	37
#define DI31_HA66	38
#define DI32_HA67	39

#define DI17_HA42	40
#define DI18_HA43	41
#define DI19_HA44	42
#define DI20_HA45	43

#define DI21_HA46	44
#define DI22_HA57	45
#define DI23_HA58	46
#define DI24_HA59	47

#define DI9_HA34	48
#define DI10_HA35	49
#define DI11_HA36	50
#define DI12_HA37	51

#define DI13_HA38	52
#define DI14_HA39	53
#define DI15_HA40	54
#define DI16_HA41	55

#define DI1_EA15	56
#define DI2_HA10	57
#define DI3_HA11	58
#define DI4_HA12	59

#define DI5_HA13	60
#define DI6_HA14	61
#define DI7_HA15	62
#define DI8_HA16	63

void setDigitalIn(EBUdigitalIn *packet, int channel, int value);
uint8_t getDigitalIn(EBUdigitalIn *packet, int channel);
