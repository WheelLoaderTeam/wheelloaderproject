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
uint8_t i=0;
uint16_t read_adc(uint8_t channel);		//Function to read ADC
void usart_init(unsigned int baudrate);
void usart_send(unsigned char data);
void usart_putstring(char* data, unsigned char length);
char buffer_x[17];
char buffer_y[17];
char buffer_z[17];
uint8_t adc_x_hi;
uint8_t adc_x_lo;
char adc_x_hi_buf;
char adc_x_lo_buf;
int length_x;
int length_y;
int length_z;

int main(void)
{
	
	usart_init(1);
	init_ADC();
	init_timer();

	sei();
    while(1)
    {
		//i = 0 is z-axis, i = 1 is y-axis, i = 2 is x-axis
		for(i=0; i<3; i++){
			adc_value = read_adc(i);
			if(i == 0){
				itoa(adc_value, buffer_z,10);
				length_z = strlen(buffer_z);
			}
			if(i == 1){
				itoa(adc_value,buffer_y,10);
				length_y = strlen(buffer_y);
			}
			if(i ==2){
				itoa(adc_value,buffer_x,10);
				length_x = strlen(buffer_x);
				adc_x_hi = (uint8_t)(adc_value>>8);
				adc_x_lo = (uint8_t)adc_value;
			}
		}
	}
}

ISR(TIMER1_COMPA_vect){
//	usart_putstring("Reading channel ",16);
	//usart_putstring("z",1);
	//usart_putstring(" : ",3);
	//usart_putstring(buffer_z,length_z);
	//usart_putstring(" ",1);
	//usart_putstring("Reading channel ",16);
	//usart_putstring("z",1);
	//usart_putstring(" : ",3);
 	//usart_putstring(buffer_y,length_y);
// 	usart_putstring(" ",1);
	//usart_putstring("Reading channel ",16);
	//usart_putstring("z",1);
	//usart_putstring(" : ",3);
	//usart_putstring(buffer_x,length_x);
	//usart_send('\r');
	//usart_send('\n');
	usart_send(adc_x_hi);
	usart_send(adc_x_lo);
}

//All this should be put in another file, like init.c
uint16_t read_adc(uint8_t channel){
	ADMUX &= 0xF0;			//Clear older read channel
	ADMUX |= channel;		//Define new channel to read
	ADCSRA |= (1<<ADSC);	//Starts a new conversion
	while(ADCSRA & (1<<ADSC)); //Wait until conversion is complete
	return ADCW;
}

//The USART function are taken from Larsmark
void usart_init(unsigned int baudrate) {
	/* Set baud rate */
	UBRR1H = (unsigned char) (baudrate>>8);
	UBRR1L = (unsigned char) baudrate;
	/* Set frame format: 8data, no parity & 1 stop bits */
	UCSR1C = (0<<UMSEL0) | (0<<USBS0) | (1<<UCSZ1) | (1<<UCSZ0) | (0<<UCSZ2);
	/* Enable receiver, transmitter and receive interrupt */
	UCSR1B = (1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1);
}

void usart_send(unsigned char data){
	/* Wait for empty transmit buffer */
	while (!( UCSR1A & (1<<UDRE1)));
	/* Put data into buffer, sends the data */
	UDR1 = data;
}
void usart_putstring(char* data, unsigned char length){
	int i;
	for (i=0;i<length;i++)
	usart_send(*(data++));
}