#ifndef __USART_H__
#define __USART_H__

void initUsart1(unsigned int baud);
void txUsart1 (unsigned char data);
unsigned char rxUsart1 (void);

#endif // __USART_H__
