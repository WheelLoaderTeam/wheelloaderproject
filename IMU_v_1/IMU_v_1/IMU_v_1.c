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

int main(void)
{
	char buffer[17];
	usart_init(25);
	init_ADC();
	int length;
    while(1)
    {
        for(i=0; i<3; i++){
				adc_value = read_adc(i);
				usart_putstring("Reading channel ",16);
				usart_send('0' + i);
				usart_putstring(" : ",3);
				itoa(adc_value,buffer,10);
				length = strlen(buffer);
				usart_putstring(buffer,length);
				usart_putstring("  ",2);
				_delay_ms(500);			
				}
		usart_send('\r');
		usart_send('\n');
	}
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