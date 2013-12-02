#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>

#include "init.h"
#include "usart.h"


void init_ports(void)	{
	DDRA = 0xFF; //Not used, set as outputs
	DDRB = 0xF7; //Set not used as outputs, MISO input, MOSI output, SCK output, all CS is active low
	DDRC = 0xFF; //Not used, 
	DDRD = 0xFF; //Set not used as outputs, UART set later
	DDRE = 0xFE; //Set not used as outputs, Rx,Tx correctly..
	DDRF = 0x08; //JTAG and ADC 
	DDRG = 0xFF; //Not used
	
	//SPI pins
	PORTB |= (1<<PB0);
	PORTB |= (1<<PB1);
	PORTB |= (1<<PB4);
	PORTB |= (1<<PB5);
	PORTB |= (1<<PB6);
}
/*! Larsmarks code */
void init_usb_usart(unsigned char baudrate) {
	usart1_init(baudrate);
	
	//Make printf work as output to USB
	fdevopen((void*)usart1_transmit, (void*)usart1_receive_loopback);
}

void init_ADC(void) {
	ADMUX &= ~(1<<REFS1);	//Clear REFS1
	ADMUX |= (1<<REFS0);	//Set voltage reference to AVcc
	ADMUX &= ~(1<<ADLAR);	//Make sure ADLAR is zero (right adjusted result)
	ADCSRA |= ((1<<ADPS2)|(1<<ADPS1));	//Set division to 8MHz/64 = 125kHz
	ADCSRA |= (1<<ADEN);	//Turn on ADC 
	ADCSRA |= (1<<ADSC);	//Do an initial conversion (takes longest time)
}

void init_timer(void){		//Timer for creating interrupts to send measured data (CTC)
	TCCR1B |= (1<<WGM12)|(1<<CS11);  //Set in CTC-mode, compare with OCR1A, fIO/8
	TIMSK1 |= (1<<OCIE1A);	//Enable compare match interrupt
	OCR1A = 0x270F;			//0x270F with fIO/8 creates a 100 Hz interrupt frequency

/*! Just a test to see correctness, interrupt at 1Hz
TCCR1B |= (1<<WGM12)|(1<<CS12)|(1<<CS10);
TIMSK1 |= (1<<OCIE1A);
OCR1A = 0x1E83;
*/
}

//The USART function are taken from Larsmark
void usart_init(uint16_t baudrate) {
	/* Set baud rate */
	UBRR1H = (uint8_t) (baudrate>>8);
	UBRR1L = (uint8_t) baudrate;
	/* Set frame format: 8data, no parity & 1 stop bits */
	UCSR1C = (0<<UMSEL0) | (0<<USBS0) | (1<<UCSZ1) | (1<<UCSZ0) | (0<<UCSZ2);
	/* Enable receiver, transmitter and receive interrupt */
	//UCSR1B = (1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1);
	UCSR1B = (1<<TXEN1); 
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


//All this should be put in another file, like init.c
uint16_t read_adc(uint8_t channel){
	ADMUX &= 0xF0;			//Clear older read channel
	ADMUX |= channel;		//Define new channel to read
	ADCSRA |= (1<<ADSC);	//Starts a new conversion
	while(ADCSRA & (1<<ADSC)); //Wait until conversion is complete
	return ADCW;
}