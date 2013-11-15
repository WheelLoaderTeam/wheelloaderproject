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
#include "global.h"
#include <util/delay.h>

#include "usart.h"

#include "init.h"

uint16_t adc_value_x;		//x-axis value
uint16_t adc_value_y;		//y-axis value
uint16_t adc_value_z;		//z-axis value
uint8_t i=0;
uint16_t read_adc(uint8_t channel);		//Function to read ADC

int main(void)
{
	char buffer[10];
	usart1_init(25);
	init_ADC();
		
    while(1)
    {
        for(i=0; i<3; i++){
			if(i == 0){
				adc_value_z = read_adc(i);
				usart1_sendstring("Reading channel ",16);
				usart1_transmit('0' + i);
				usart1_sendstring(" : ",3);
				itoa(adc_value_z,buffer,10);
				usart1_sendstring(buffer,10);
				usart1_sendstring("  ",2);
			}
			if(i == 1){
				adc_value_y = read_adc(i);
				usart1_sendstring("Reading channel ",16);
				usart1_transmit('0' + i);
				usart1_sendstring(" : ",3);
				itoa(adc_value_y, buffer,10);
				usart1_sendstring(buffer,10);
				usart1_sendstring("  ",2);
			}
			if(i==2){
				adc_value_x = read_adc(i);
				usart1_sendstring("Reading channel ",16);
				usart1_transmit('0' + i);
				usart1_sendstring(" : ",3);
				itoa(adc_value_x, buffer,10);
				usart1_sendstring(buffer,10);
				usart1_sendstring("  ",2);
			}
			
		}
		usart1_transmit('\r');
		usart1_transmit('\n');
	/*!	itoa(adc_value_x, &text[0],10);
		usart1_sendstring(&text[0],10);
		_delay_ms(100);
		itoa(adc_value_y, &text[1],10);
		usart1_sendstring(&text[1],10);
		_delay_ms(100);
		itoa(adc_value_z, &text[2],10);
		usart1_sendstring(&text[2],10);
		_delay_ms(500);*/
	}
}

uint16_t read_adc(uint8_t channel){
	ADMUX &= 0xF0;			//Clear older read channel
	ADMUX |= channel;		//Define new channel to read
	ADCSRA |= (1<<ADSC);	//Starts a new conversion
	while(ADCSRA & (1<<ADSC)); //Wait until conversion is complete
	return ADCW;
}