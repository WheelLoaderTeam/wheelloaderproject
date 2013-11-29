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

uint16_t gyro;
int main(void)
{
	
	usart_init(0);
	init_ADC();
	init_timer();
	spiInit();
	
	_delay_ms(100);
	gyro = spiTransferAll(0x20000003,1);
	_delay_ms(50);
	 	usart_send((uint8_t)(gyro&0xFF000000>>24));  //Send most significant byte first
	 	usart_send((uint8_t)(gyro&0x00FF0000>>16));	//Send next byte
	 	usart_send((uint8_t)(gyro>>8));	//Send next byte
	 	usart_send((uint8_t)gyro);			//Send last byte
	gyro = spiTransferAll(0x20000000,1);
	_delay_ms(50);
	 	usart_send((uint8_t)(gyro&0xFF000000>>24));  //Send most significant byte first
	 	usart_send((uint8_t)(gyro&0x00FF0000>>16));	//Send next byte
	 	usart_send((uint8_t)(gyro>>8));	//Send next byte
	 	usart_send((uint8_t)gyro);			//Send last byte
	gyro = spiTransferAll(0x20000000,1);
	_delay_ms(0.1);
	 	usart_send((uint8_t)(gyro&0xFF000000>>24));  //Send most significant byte first
	 	usart_send((uint8_t)(gyro&0x00FF0000>>16));	//Send next byte
	 	usart_send((uint8_t)(gyro>>8));	//Send next byte
	 	usart_send((uint8_t)gyro);			//Send last byte
	
	
	sei();

    while(1);
    {
		
	//	gyro = spiTransferAll(0x30000000,1);
		
// 		usart_send((uint8_t)((gyro & 0xFF000000UL)>>24));
// 		usart_send((uint8_t)((gyro & 0x00FF0000UL)>>16));
// 		usart_send((uint8_t)(gyro>>8));
		//usart_send((uint8_t)(gyro));
		;
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
	

	usart_send('I');
// 	gyro = spiTransferAll(0x20000000,1);
//  	usart_send((uint8_t)(gyro>>24));  //Send most significant byte first
//   	usart_send((uint8_t)(gyro>>16));	//Send next byte
//   	usart_send((uint8_t)(gyro>>8));	//Send next byte
//  	usart_send((uint8_t)gyro);			//Send last byte
	usart_send('Y');	
}





