
#define S19 	0
#define S20 	1
#define P15 	2
#define P16 	3

#define S21 	4
#define res1	5
#define res2	6
#define res3	7

#define S15		8
#define S16 	9
#define P11 	10
#define P12 	11

#define S17 	12
#define S18 	13
#define P13 	14
#define P14 	15

#define S11 	16
#define S12 	17
#define P7 		18
#define P8 		19

#define S13 	20
#define S14 	21
#define P9 		22
#define P10 	23

#define S7 		24
#define S8 		25
#define P3 		26
#define P4 		27

#define S9 		28
#define S10 	29
#define P5 		30
#define P6 		31

#define S1 		32
#define S2 		33
#define S3 		34
#define S4 		35

#define S5 		36
#define S6 		37
#define P1 		38
#define P2 		39

#define F2 		40
#define D37 	41
#define F3 		42
#define D38 	43

#define F4 		44
#define D39 	45
#define F5 		46
#define F6 		47

#define D21 	48
#define CAN1 	49
#define J1708 	50
#define CAN2 	51

#define D34 	52
#define D35 	53
#define F1 		54
#define D36 	55

#define D30 	56
#define D17 	57
#define D31 	58
#define D18 	59

#define D32 	60
#define D19 	61
#define D33 	62
#define D20 	63

#define D7 		64
#define D27 	65
#define D14 	66
#define D8 		67

#define D28 	68
#define D15 	69
#define D29 	70
#define D16 	71

#define D4 		72
#define D11 	73
#define D5 		74
#define D25 	75

#define D12 	76
#define D6 		77
#define D26 	78
#define D13 	79

#define D1 		80
#define D22 	81
#define D2 		82
#define D9 		83

#define D23 	84
#define D3 		85
#define D10 	86
#define D24 	87

#define A11 	88
#define A19 	89
#define A6 		90
#define A12 	91

#define A20 	92
#define A7 		93
#define A13 	94
#define res4 	95

#define A3 		96
#define A9 		97
#define A17 	98
#define A4 		99

#define A10 	100
#define A18 	101
#define A5 		102
#define res5 	103

#define A14 	104
#define A1 		105
#define A15 	106
#define A2 		107

#define A8 		108
#define A16 	109
#define res6 	110
#define res7 	111

typedef struct analog_in_t {
	uint16_t channel[24];
}analog_in_t;

typedef struct analog_out_t{
	uint16_t channel[24];
} analog_out_t;

typedef struct digital_in_t{
	uint8_t channel[6];
} digital_in_t;


typedef struct digital_out_t{
	uint8_t channel[8];
} digital_out_t;


typedef struct relay_t{
	uint8_t channel[14];
} relay_t;

void setRelay(relay_t packet, int relay, int relayValue){
	int byte = relay / 8;
	int ofset = relay % 8;
	
	uint8_t mask = 0x80 >> ofset;
	
	uint8_t value = 0;
	if  relayValue {
		value = 0xFF;
	}
	
	packet->channel[byte] = (packet->channel[byte] & mask) | (value & mask);
	
}