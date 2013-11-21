#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>

#include "spi_1.h"




void SPI_MasterTransmit(char cData)
{
	// Start transmission
	SPDR = cData;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
}

void spiInit()
{
	// setup SPI I/O pins
	sbi(PORTB, 1);			// set SCK high
	sbi(DDRB, 1);			// set SCK as output
	cbi(DDRB, 3);			// set MISO as input
	sbi(DDRB, 2);			// set MOSI as output
	sbi(DDRB, 0);			// SS must be output for Master mode to work
	sbi(DDRB, 4);			// CS1, rot around z 
	sbi(DDRB, 5);			// CS2, rot around
	sbi(DDRB, 6);			//

	// setup SPI interface :
	sbi(SPCR, MSTR);			// master mode
	cbi(SPCR, SPR0);			// clock = f/2
	cbi(SPCR, SPR1);			//
	cbi(SPCR, CPOL);			// clock polarity = 0
	cbi(SPCR, CPHA);			// clock phase = 0
	cbi(SPCR,DORD);			// Data order MSB first
	sbi(SPCR, SPE);			// enable SPI

	sbi(SPSR, SPI2X);		// SPI speed

}
void spiSendByte(uint8_t data)
{
	SPDR = data;
	/*Wait for transmission complete*/
	while(!(SPSR & (1<<SPIF)));
}

void spiTransferByte(char data)

/* Function to send 32 bit command and receive 32 bit data*/
uint32_t spiTransferAll(uint32_t data, int CS)
	if (CS == 1)
	{
		PORTB &= ~(1<<DDB4);	//Set CS1 low
	}
	if (CS == 2)
	{
		PORTB &= ~(1<<DDB5);	//Set CS2 low
	}	 
	if (CS == 3)
	{
		PORTB &= ~(1<<DDB6);	//Set CS3 low
	}
	spiSendByte((uint8_t)(data>>24));  //Send most signifiant byte first
	spitSendByte((uint8_t)(data>>16));	//Send next byte
	spiSendByte((uint8_t)(data>>8));	//Send next byte
	spiSendByte((uint8_t)data);			//Send last byte
	
	
	//Skriv while loop som väntar på flagga. Loopa fyra gånger, gyro skickar 32 bitar, MCU tar emot 8 -> fyra läsningar. Skicka dummy-variabel för att starta.

	//Definiera, open, write, wait, read, close.