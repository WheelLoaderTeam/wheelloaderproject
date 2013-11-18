#include "usart.h"
#include <avr/io.h>

// I think everything here should be self explained.
// If you dont understand it then you probably should not touch it!
void initUsart1(unsigned int baud) {
  /* Set baud rate */
  UBRR1H = (unsigned char) (baud>>8);
  UBRR1L = (unsigned char) baud;
  /* Set frame format: 8data, no parity & 1 stop bits */
  UCSR1C = (0<<UMSEL1) | (3<<UCSZ10);
  /* Enable receiver and transmitter */
  UCSR1B = (1<<RXEN1) | (1<<TXEN1);
}

void txUsart1 (unsigned char data)
{
  /* Wait for empty transmit buffer */
  while ( ! ( UCSR1A & (1<<UDRE1)));
  /* Put data into buffer, sends the data */
  UDR1 = data;
}

unsigned char rxUsart1 (void)
{
  /* Wait for data to be received */
  while ( ! (UCSR1A & (1<<RXC1)));
  /* Get and return received data from buffer */
  return UDR1;
}
