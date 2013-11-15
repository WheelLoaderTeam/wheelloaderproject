/***************************************************************************
 *   Copyright (C) 2009 by Mikael Larsmark, Luleå University of Technology *
 *   larsmark@ltu.se                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/signal.h>


#include "init.h"
#include "ext_dev.h"
#include "lcd.h"
#include "i2c.h"
#include "delay.h"
#include "lm77.h"
#include "realtime_clock.h"

//Poll the temperature each 500ms
#define COUNTER_POLL_TEMP_INTERVAL	500
#define COUNTER_POLL_TIME_INTERVAL	100

//Counter which keeps track of how many ms has passed since the last temperature polling
volatile unsigned int counter_poll_temperature = COUNTER_POLL_TEMP_INTERVAL;
//Counter which keeps track of how many ms has passed since the last time polling
volatile unsigned int counter_poll_time = COUNTER_POLL_TIME_INTERVAL;

float curr_temp = 0;

char display[2][16];

int main(void) {
	//Deactivate global interrupt
	cli();
	
	//Initialize the port directions
	init_ports();
	//Initialize the USB serial port
	init_usb_usart(BAUDRATE_19200);
	//Inititalize the LCD
	lcdInit();
	//Initialize the I2C communication bus
	i2cInit();
	//Initialize the PWM for the backlight
	init_backlight();
	//Set the default backlight level, configurable in lcdconf.h
	ext_dev_lcd_backlight(DEFAULT_BACKLIGHT_LEVEL);
	//Initialize timer0
	init_timer_0();
	
	//Enable global interrupt
	sei();
	
	printf("\nLTU AVR Development board started\n");
	printf("---------------------------------\n");
	printf("Firmware version: 0.1b\n");
	printf("Hardware design: Mikael Larsmark\n");
	printf("Software demo: Mikael Larsmark\n");
	
	//Clear the display array
	for (int y=0;y<2;y++)
		for (int x=0;x<16;x++)
			display[y][x] = 0;
	
	//Print out some text at startup
	lcdHome();
	lcdPrintData(" AVR Dev board",14);
	lcdGotoXY(0,1);
	lcdPrintData("   www.ltu.se",13);
	lcdGotoXY(0,0);
	//delay_s(2);
	lcdClear();
	
	/* Initialize the RTC, arguments are:
     Seconds, minutes, hours, day, month, year */
	rtc_init(0,0,0,4,11,9);
	
	//Stupid thing for optimizer
	while(1) {
		if (counter_poll_temperature >= COUNTER_POLL_TEMP_INTERVAL) {
			//Retrieve the temperature from the lm77 temperature sensor
			curr_temp = lm77_read_temp();
			
			//Copy the temperature into the display[0] 16 byte character buffer
			sprintf(display[0],"Temp: %.1fC",curr_temp);
			
			//If the temperature goes over 28 degrees the relay will get activated. When it
			//goes under 28 degrees the relay will be deactivated
			if (curr_temp >= 28)
				ext_dev_relay_on();
			else
				ext_dev_relay_off();
			
			lcdGotoXY(0,0);
			lcdPrintData("                ",16);
			lcdGotoXY(0,0);
			lcdPrintData(display[0],strlen(display[0]));
			
			//Reset the counter variable
			counter_poll_temperature = 0;
		}
		
		//Function will update the time at COUNTER_POLL_TIME_INTERVAL intervals (defined at the top of this file)
		if (counter_poll_time >= COUNTER_POLL_TIME_INTERVAL) {
			char temp_time[8];
			rtc_get_time((char *)temp_time);
			
			sprintf(display[1],"Time: %s",temp_time);
			
			lcdGotoXY(0,1);
			lcdPrintData(display[1],strlen(display[1]));
			
			//Reset the counter variable
			counter_poll_time = 0;
		}
	}
}

/*!Output compare 0 interrupt - "called" with 1ms intervals*/
ISR(SIG_OUTPUT_COMPARE0) {
	counter_poll_temperature++;
	counter_poll_time++;
}

/*! Interrupt which is triggered if a button is pressed */
SIGNAL(SIG_INTERRUPT6) {
	//Read the current button status
	unsigned char button_status = ext_dev_read_button(BUTTON_ALL);
	
	//Check which button is pressed or released. Depending on which state the
	//button is in we either turn on or off the LED
	if (button_status & (1<<0))
		ext_dev_led_on(1);
	else
		ext_dev_led_off(1);
	
	if (button_status & (1<<1))
		ext_dev_led_on(2);
	else
		ext_dev_led_off(2);
		
	if (button_status & (1<<2))
		ext_dev_led_on(3);
	else
		ext_dev_led_off(3);
		
	if (button_status & (1<<3))
		ext_dev_led_on(4);
	else
		ext_dev_led_off(4);
		
	if (button_status & (1<<4))
		ext_dev_led_on(5);
	else
		ext_dev_led_off(5);	
}