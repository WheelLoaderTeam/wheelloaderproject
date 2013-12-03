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
#include "spi_1.h"


uint16_t adc_value;		
//uint8_t i=0;


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
	//init_timer();
	spiInit();
//	uint8_t i = 0;
// 	while(1) {
// 		usart_send(i++);
// 		spiTransferAll(0x88888888, 1);
// 		spiTransferAll(0x88888888, 2);
// 		spiTransferAll(0x88888888, 3);
// 		_delay_ms(1);
// 	}
// 	
// 	return 0;
	
	_delay_ms(100);
	gyroZ = spiTransferAll(0x20000003,1);
	_delay_ms(50);

	gyroZ = spiTransferAll(0x20000000,1);
	_delay_ms(50);

	gyroZ = spiTransferAll(0x20000000,1);
		//Future: See if self-test was correct
	_delay_ms(0.1);

	gyroZ = spiTransferAll(0x20000000,1);
	_delay_ms(0.1);
	
	gyroX = spiTransferAll(0x20000003,2);
	_delay_ms(50);

	gyroX = spiTransferAll(0x20000000,2);
	_delay_ms(50);

	gyroX = spiTransferAll(0x20000000,2);
	//Future: See if self-test was correct
	_delay_ms(0.1);

	gyroX = spiTransferAll(0x20000000,2);
	_delay_ms(0.1);
	
	gyroY = spiTransferAll(0x20000003,3);
	_delay_ms(50);

	gyroY = spiTransferAll(0x20000000,3);
	_delay_ms(50);

	gyroY = spiTransferAll(0x20000000,3);
	//Future: See if self-test was correct
	_delay_ms(0.1);

	gyroY = spiTransferAll(0x20000000,3);
	_delay_ms(0.1);
	
	sei();

    while(1)
    {
		usart_send(0x00);
		usart_send(0x00);
		usart_send(0x00);
		usart_send(0x00);
 		gyroX = spiTransferAll(0x20000000,2);
		_delay_ms(0.1);
		gyroX = spiTransferAll(0x20000000,2);
		_delay_ms(0.1);
		usart_send((uint8_t)(gyroX>>24)); //Send most significant byte first
		usart_send((uint8_t)(gyroX>>16));	//Send next byte
		usart_send((uint8_t)(gyroX>>8));				//Send next byte
		usart_send((uint8_t)gyroX);					//Send last byte
		gyroY = spiTransferAll(0x20000000,3);
		_delay_ms(0.1);
		gyroY = spiTransferAll(0x20000000,3);
		_delay_ms(0.1);
		usart_send((uint8_t)(gyroY>>24)); //Send most significant byte first
		usart_send((uint8_t)(gyroY>>16));	//Send next byte
		usart_send((uint8_t)(gyroY>>8));				//Send next byte
		usart_send((uint8_t)gyroY);
		gyroZ = spiTransferAll(0x20000000,1);
		_delay_ms(0.1);
		gyroZ = spiTransferAll(0x20000000,1);
		_delay_ms(0.1);
		usart_send((uint8_t)(gyroZ>>24)); //Send most significant byte first
		usart_send((uint8_t)(gyroZ>>16));	//Send next byte
		usart_send((uint8_t)(gyroZ>>8));				//Send next byte
		usart_send((uint8_t)gyroZ);
		adc_value = read_adc(2);
		adc_x_hi = (uint8_t)(adc_value>>8);
		adc_x_lo = (uint8_t)adc_value;
		usart_send(adc_x_hi);
		usart_send(adc_x_lo);
		adc_value = read_adc(1);
		adc_y_hi = (uint8_t)(adc_value>>8);
		adc_y_lo = (uint8_t)adc_value;
		usart_send(adc_y_hi);
		usart_send(adc_y_lo);
		adc_value = read_adc(0);
		adc_z_hi = (uint8_t)(adc_value>>8);
		adc_z_lo = (uint8_t)adc_value;
		usart_send(adc_z_hi);
		usart_send(adc_z_lo);
	}
}


//char a = 0;

ISR(TIMER1_COMPA_vect){
	/*
	adc_value = read_adc(0);
	adc_z_hi = (uint8_t)(adc_value>>8);
	adc_z_lo = (uint8_t)adc_value;
	usart_send(adc_z_hi);
	usart_send(adc_z_lo);
	usart_send('\n');
	adc_value = read_adc(1);
	adc_y_hi = (uint8_t)(adc_value>>8);
	adc_y_lo = (uint8_t)adc_value;
	usart_send(adc_y_hi);
	usart_send(adc_y_lo);
	usart_send('\n');
	adc_value = read_adc(2);
	adc_x_hi = (uint8_t)(adc_value>>8);
	adc_x_lo = (uint8_t)adc_value;
	usart_send(adc_x_hi);
	usart_send(adc_x_lo);
	usart_send('\n');
	*/
	

// 	usart_send('I');
//  	gyro = spiTransferAll(0x20000000,1);
// 	  	usart_send((uint8_t)(gyro>>24));	//Send most significant byte first
// 	   	usart_send((uint8_t)(gyro>>16));	//Send next byte
// 	   	usart_send((uint8_t)(gyro>>8));		//Send next byte
// 	  	usart_send((uint8_t)gyro);			//Send last byte
// 	usart_send('Y');	
}





