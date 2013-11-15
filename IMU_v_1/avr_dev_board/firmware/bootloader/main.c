
#define Direct_jump_to_zero()   { asm ("jmp 0"); }

#include "common.h"
#include <avr/io.h>
#include <util/delay.h>
#include "bootloader.h"

void startApplication();
unsigned char DDRC_tmp;
int main(void) {
  // No division on clock speed
  CLKPR = 0x80;
  CLKPR = 0;
  
  // Set PC8 as input but first save the register for restoring purpose.
  // They probably have a default value as input, but I'm to lazy to check.
  DDRC_tmp = DDRC;
  DDRC &= (~0x80) ;
  
  _delay_ms(0xff);
  
  // If PC8 is high enter bootloader,
  // else start application.
  if ( (PINC & 0x80) > 0 ) {
    startBootloader();
  } else {
    startApplication();
  }
  return 0;
}

void startApplication() {
  DDRC = DDRC_tmp;
  Direct_jump_to_zero();
}

