#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include "global.h"
#include <util/delay.h>
#include "spi_1.h"
#include "init.h"
void spiInit()
{

SPCR = (1<<MSTR)|(1<<SPE);
SPSR = (1<<SPI2X);

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
	PORTB &= ~(1<<PB0);
	if (CS == 1)
	{
		PORTB &= ~(1<<PB4);	//Set CS1 low
	}
	if (CS == 2)
	{
		PORTB &= ~(1<<PB5);	//Set CS2 low
	}	 
	if (CS == 3)
	{
		PORTB &= ~(1<<PB6);	//Set CS3 low
	}
	spiSendByte((uint8_t)(data>>24));  //Send most significant byte first
	spiSendByte((uint8_t)(data>>16));	//Send next byte		
	spiSendByte((uint8_t)(data>>8));	//Send next byte
	spiSendByte((uint8_t)data);			//Send last byte
	
	
	outdata = 0;
	spiSendByte(0x10);
	outdata = ((uint32_t)SPDR)<<24;
	spiSendByte(0x00);
	outdata |= ((uint32_t)SPDR)<<16;
	spiSendByte(0x00);
	outdata |= ((uint32_t)SPDR)<<8;
	spiSendByte(0x00);
	outdata |= ((uint32_t)SPDR);
		if (CS == 1)
		{
			PORTB |= (1<<PB4);	//Set CS1 low
		}
		if (CS == 2)
		{
			PORTB |= (1<<PB5);	//Set CS2 low
		}
		if (CS == 3)
		{
			PORTB |= (1<<PB6);	//Set CS3 low
		}
		
		PORTB |= (1<<PB0);
	return outdata;
}
	
	//Skriv while loop som väntar på flagga. Loopa fyra gånger, gyro skickar 32 bitar, MCU tar emot 8 -> fyra läsningar. Skicka dummy-variabel för att starta.

	//Definiera, open, write, wait, read, close.