#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include "global.h"
#include <util/delay.h>
#include "spi.h"
#include "init.h"
void spiInit()
{
	DDRB = (1<<1)|(1<<2)|(1<<4)|(1<<5)|(1<<6);
	SPCR = (1<<MSTR)|(1<<SPE);
	SPSR = (1<<SPI2X);
	PORTB |= (1<<PB4)|(1<<PB5)|(1<<PB6);
}

void spiSendByte(uint8_t data)
{
	SPDR = data;
	/*Wait for transmission complete*/
	while(!(SPSR & (1<<SPIF)));
}


/* Function to send 32 bit command and receive 32 bit data*/
uint32_t spiTransferAll(uint32_t data, int CS)
{
	uint32_t outdata;
	PORTB &= ~(1<<PB0);					//Possibly needed to start CLK
	if (CS == 1)
	{
		PORTB &= ~(1<<PB4);				//Set CS1 low
	}
	if (CS == 2)
	{
		PORTB &= ~(1<<PB5);				//Set CS2 low
	}	 
	if (CS == 3)
	{
		PORTB &= ~(1<<PB6);				//Set CS3 low
	}
	outdata = 0;
	spiSendByte((uint8_t)(data>>24));	//Send most significant byte first
	outdata = ((uint32_t)SPDR)<<24;
	spiSendByte((uint8_t)(data>>16));	//Send next byte
	outdata |= ((uint32_t)SPDR)<<16;		
	spiSendByte((uint8_t)(data>>8));	//Send next byte
	outdata |= ((uint32_t)SPDR)<<8;
	spiSendByte((uint8_t)data);			//Send last byte
	outdata |= ((uint32_t)SPDR);
	if (CS == 1)
	{
		PORTB |= (1<<PB4);				//Set CS1 high
	}
	if (CS == 2)
	{
		PORTB |= (1<<PB5);				//Set CS2 high
	}
	if (CS == 3)
	{
		PORTB |= (1<<PB6);				//Set CS3 high
	}
	PORTB |= (1<<PB0);
	return outdata;
}
	

