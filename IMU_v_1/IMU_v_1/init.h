# ifndef	_INIT_H_
#define		_INIT_H_

#define		BAUDRATE_19200 51

void init_ports(void);

void init_usb_usart(unsigned char baudrate);

void init_ADC(void);

#endif
