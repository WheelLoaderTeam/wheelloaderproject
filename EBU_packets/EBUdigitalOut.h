#include <stdint.h>

typedef struct EBUdigitalOut{
	uint8_t channel[8];
} EBUdigitalOut;

#define newEBUdigitalOut() {0, 0, 0, 0, 0, 0, 0, 0}

#define SO17_HB66	0
#define SO18_HB67	1
#define SO19_HB68	2
#define SO20_HB69	3

#define SO21_HB70	4
#define EXTRA_SO1	5
#define EXTRA_SO2	6
#define EXTRA_SO3	7

#define SO9_HB58	8
#define SO10_HB59	9
#define SO11_HB60	10
#define SO12_HB61	11

#define SO13_HB62	12
#define SO1_HB50	13
#define SO14_HB63	14
#define SO2_HB51	15

#define SO15_HB64	16
#define SO3_HB52	17
#define SO16_HB65	18
#define SO4_HB53	19

#define SO5_HB54	20
#define SO6_HB55	21
#define SO7_HB56	22
#define SO8_HB57	23

#define DO36_EB21	24
#define DO33_EA50	25
#define Dummy1		26
#define DO37_EB20	27

#define DO38_EB19	28
#define DO39_EB18	29
#define DO34_EB23	30
#define DO35_EB22	31

#define DO28_EA55	32
#define DO25_EA58	33
#define DO32_EA51	34
#define DO29_EA54	35

#define DO30_EA53	36
#define DO31_EA52	37
#define DO26_EA57	38
#define DO27_EA56	39

#define DO20_EA26	40
#define DO17_EA29	41
#define DO24_EA59	42
#define DO21_EA25	43

#define DO22_EA61	44
#define DO23_EA60	45
#define DO18_EA28	46
#define DO19_EA27	47

#define DO12_EA34	48
#define DO9_EA37	49
#define DO16_EA30	50
#define DO13_EA33	51

#define DO14_EA32	52
#define DO15_EA31	53
#define DO10_EA36	54
#define DO11_EA35	55

#define DO4_EA12	56
#define DO1_HA9		57
#define DO8_EA8		58
#define DO5_EA11	59

#define DO6_EA10	60
#define DO7_EA9		61
#define DO2_EA14	62
#define DO3_EA13	63

void setDigitalOut(EBUdigitalOut *packet, int channel, int value);
uint8_t getDigitalOut(EBUdigitalOut *packet, int channel);
