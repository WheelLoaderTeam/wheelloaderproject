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
}
/*! Larsmarks code */
void init_usb_usart(unsigned char baudrate) {
	usart1_init(baudrate);
	
	//Make printf work as output to USB
	fdevopen((void*)usart1_transmit, (void*)usart1_receive_loopback);
}

void init_ADC(void) {
	ADMUX &= ~(1<<REFS1);	\\Clear REFS1
	ADMUX |= (1<<REFS0);	\\Set voltage reference to AVcc
	ADMUX &= ~(1<<ADLAR);	\\Make sure ADLAR is zero (right adjusted result)
	
	
}