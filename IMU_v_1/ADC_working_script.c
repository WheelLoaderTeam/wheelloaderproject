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



#include "init.h"

uint16_t adc_value;		

uint8_t adc_x_hi;
uint8_t adc_x_lo;
uint8_t adc_y_hi;
uint8_t adc_y_lo;
uint8_t adc_z_hi;
uint8_t adc_z_lo;

uint16_t gyro;
int main(void)
{
	usart_init(0);
	init_ADC();
	init_timer();
	spiInit();
	sei();
    while(1);
    {
	;
	}
}
ISR(TIMER1_COMPA_vect){	
	//The ADC sending that works correct
	adc_value = read_adc(0);
	adc_z_hi = (uint8_t)(adc_value>>8);
	adc_z_lo = (uint8_t)adc_value;
	usart_send(adc_z_hi);
	usart_send(adc_z_lo);
	usart_send('\n')
	adc_value = read_adc(1);
	adc_y_hi = (uint8_t)(adc_value>>8);
	adc_y_lo = (uint8_t)adc_value;
	usart_send(adc_y_hi);
	usart_send(adc_y_lo);
	usart_send('\n')
	adc_value = read_adc(2);
	adc_x_hi = (uint8_t)(adc_value>>8);
	adc_x_lo = (uint8_t)adc_value;
	usart_send(adc_x_hi);
	usart_send(adc_x_lo);
	usart_send('\n')
}
