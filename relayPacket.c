
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

