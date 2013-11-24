# ifndef	_INIT_H_
#define		_INIT_H_

#define		BAUDRATE_19200 51

void init_ports(void);

void init_usb_usart(unsigned char baudrate);

void init_ADC(void);

void init_timer(void);

void usart_init(unsigned int baudrate);

void usart_send(unsigned char data);

void usart_putstring(char* data, unsigned char length);

uint16_t read_adc(uint8_t channel);		//Function to read ADC
#endif
