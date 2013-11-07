/*
 * IMU_v_1.c
 *
 * Created: 2013-11-01 13:57:17
 *  Author: jesham-9
 */ 


#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/signal.h>


#include "init.h"

uint16_t adc_value_x;		//x-axis value
uint16_t adc_value_y;		//y-axis value
uint16_t adc_value_z;		//z-axis value
uint16_t read_adc(uint8_t channel);		//Function to read ADC, channel = 0x

int main(void)
{
    while(1)
    {
        for(i=0; i<3, i++){
			if(i == 0){
				adc_value_z = read_adc(i);
			}
			if(i == 1){
				adc_value_y = read_adc(i);
			}
			if(i==2){
				adc_value_x = read_adc(i);
			}
		}
    }
}

uint16_t read_adc(uint8_t channel){
	ADMUX &= 0xF0;			//Clear older read channel
	ADMUX |= channel;		//Define new channel to read
	ADCSRA |= (1<<ADSC);	//Starts a new conversion
	while(ADCRSA & (1<<ADSC)); //Wait until conversion is complete
	return ADCW;
}