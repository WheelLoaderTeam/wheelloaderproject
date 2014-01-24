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
#include "spi.h"


uint16_t adc_value;		


uint8_t adc_x_hi;
uint8_t adc_x_lo;
uint8_t adc_y_hi;
uint8_t adc_y_lo;
uint8_t adc_z_hi;
uint8_t adc_z_lo;

uint32_t gyroX;
uint32_t gyroY;
uint32_t gyroZ;

int main(void)
{
	
	usart_init(0);
	init_ADC();
	init_timer();
	spiInit();
	
	_delay_ms(100);
	gyroZ = spiTransferAll(0x20000003,1);		// Start-up sequence, see data sheet
	_delay_ms(50);

	gyroZ = spiTransferAll(0x20000000,1);
	_delay_ms(50);

	gyroZ = spiTransferAll(0x20000000,1);
	_delay_ms(0.1);

	gyroZ = spiTransferAll(0x20000000,1);
	_delay_ms(0.1);
	
	gyroX = spiTransferAll(0x20000003,2);
	_delay_ms(50);

	gyroX = spiTransferAll(0x20000000,2);
	_delay_ms(50);

	gyroX = spiTransferAll(0x20000000,2);
	_delay_ms(0.1);

	gyroX = spiTransferAll(0x20000000,2);
	_delay_ms(0.1);
	
	gyroY = spiTransferAll(0x20000003,3);
	_delay_ms(50);

	gyroY = spiTransferAll(0x20000000,3);
	_delay_ms(50);

	gyroY = spiTransferAll(0x20000000,3);	
	_delay_ms(0.1);

	gyroY = spiTransferAll(0x20000000,3);
	_delay_ms(0.1);
	
	sei();

    while(1)
    {
		;
	}
}

/* The coordinates are defined in the documentation.*/

ISR(TIMER1_COMPA_vect){
	usart_send(0x00);
	usart_send(0x00);
	usart_send(0x00);
	usart_send(0x00);
	gyroX = spiTransferAll(0x20000000,3);	//Pos. X direction
						
	usart_send((uint8_t)(gyroX>>24));		//Send most significant byte first
	usart_send((uint8_t)(gyroX>>16));		//Send next byte
	usart_send((uint8_t)(gyroX>>8));		//Send next byte
	usart_send((uint8_t)gyroX);				//Send last byte
	gyroY = spiTransferAll(0x20000000,2);	//Neg. Y direction

	usart_send((uint8_t)(gyroY>>24));		//Send most significant byte first
	usart_send((uint8_t)(gyroY>>16));		//Send next byte
	usart_send((uint8_t)(gyroY>>8));		//Send next byte
	usart_send((uint8_t)gyroY);				//Send last byte
	gyroZ = spiTransferAll(0x20000000,1);	//Pos. Z direction

	usart_send((uint8_t)(gyroZ>>24));		//Send most significant byte first
	usart_send((uint8_t)(gyroZ>>16));		//Send next byte
	usart_send((uint8_t)(gyroZ>>8));		//Send next byte
	usart_send((uint8_t)gyroZ);				//Send last byte
	adc_value = read_adc(1);				//Neg. X direction (corresponding to y in acc)
	adc_x_hi = (uint8_t)(adc_value>>8);
	adc_x_lo = (uint8_t)adc_value;
	usart_send(adc_x_hi);
	usart_send(adc_x_lo);
	adc_value = read_adc(2);				//Neg. Y direction (corresponding to x in acc)
	adc_y_hi = (uint8_t)(adc_value>>8);
	adc_y_lo = (uint8_t)adc_value;
	usart_send(adc_y_hi);
	usart_send(adc_y_lo);
	adc_value = read_adc(0);				//Neg. Z direction (corresponding to z in acc)
	adc_z_hi = (uint8_t)(adc_value>>8);
	adc_z_lo = (uint8_t)adc_value;
	usart_send(adc_z_hi);
	usart_send(adc_z_lo);
}





