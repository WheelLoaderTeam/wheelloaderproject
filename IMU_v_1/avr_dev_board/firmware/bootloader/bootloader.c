#include "common.h"
#include <util/delay.h>
#include "usart.h"
#include <avr/io.h>
#include "flash/flash_boot_lib.h"

void bl_start();
unsigned char rxProg();
void program();
unsigned int crc(uint16_t crc, uint8_t byte);
void rxFill();
// Buffer used by rxProg()
unsigned char buf[400];
// Current index of buf
unsigned int buf_index = sizeof(buf);

enum {
  SEND_MORE_BYTE = 0x76,
  SEND_OK_BYTE = 0x94,
  SEND_RESEND_BYTE = 0x52,
};

enum {
  S_START,
  S_CONNECTED,
  S_EXIT,
};

void startBootloader() {
  unsigned char state = S_START;
  // Init uart
  initUsart1(25);
  _delay_ms(0xff);
  
  // Turn on LED3 to indicate that the bootloader has started
  DDRC = 0x1;
  PORTC = 0x1;

  while (state != S_EXIT) {
    switch (state) {
      case S_START:
        bl_start();
        state = S_CONNECTED;
        break;
      case S_CONNECTED:
        flash_full_erase(); 
        program();
        state = S_EXIT;
        break;
    }
  }

  // Turn off LED3 to indicate that the bootloader is done programing
  PORTC = 0;
  while(1);
}

/**
 * bl_start waits for the startup sequence from the programmer.
 * The startup sequence consists of 3 bytes 0x1 0x2 0x3.
 * After it has received the startup sequence the MCU will reply back
 * with the same sequence.
 */
void bl_start() {
  bool connected = false;
  unsigned char in;

  while (!connected) {
    in = rxUsart1();
first:
    if (in != 0x1) {
      continue;
    }
    in = rxUsart1();
    if (in != 0x2) {
      if (in == 0x1) {
        goto first;
      }
      continue;
    }
    in = rxUsart1();
    if (in != 0x3) {
      if (in == 0x1) {
        goto first;
      }
      continue;
    }
    txUsart1(0x1);
    txUsart1(0x2);
    txUsart1(0x3);
    connected = true;
  }
}

/**
 * Programs the program memory.
 */
void program() {
  unsigned int sections;
  unsigned long section_size;
  unsigned long address;
  unsigned int i;
  unsigned long j;

  sections = (unsigned int)rxProg() << 8;
  sections += rxProg();
  for (i = 0; i < sections; ++i) {
    address = (unsigned long)rxProg() << 24;
    address += (unsigned long)rxProg() << 16;
    address += (unsigned long)rxProg() << 8;
    address += (unsigned long)rxProg();
    section_size = (unsigned long)rxProg() << 24;
    section_size += ((unsigned long)rxProg() << 16);
    section_size += ((unsigned long)rxProg() << 8);
    section_size += rxProg();
    for (j = 0; j < section_size; /*++j*/) {
      unsigned long wr_addr = address + j;
      unsigned int remaining = section_size - j;
      // If the address to be written is above 0xFFFF the RAMPZ register must be used.
      // TODO(henrik): I have tested the RAMPZ but it didn't seem to work, fix this if Larsmark says so.

      // If we still must write more than the buffer can hold or holds, write everything in the buffer to program memory.
      // Else write the remaining part.
      if (remaining >= sizeof(buf) || ((sizeof(buf) - buf_index) < remaining)) {
        flash_wr_block(&buf[buf_index], (unsigned int)wr_addr, sizeof(buf) - buf_index);
        j += sizeof(buf)-buf_index; 
        buf_index = sizeof(buf);
        // The buffer is now empty so it must be filled again.
        rxFill();
      } else {
        flash_wr_block(&buf[buf_index], (unsigned int)wr_addr, (unsigned int)remaining);
        buf_index = remaining;
        j += remaining;
      }
      //flash_wr_byte((unsigned int)wr_addr, rxProg());
    }
  }
}

/**
 * Gets the next byte in the receive buffer buf.
 */
unsigned char rxProg() {
  // Fill the buffer if needed
  rxFill();
  return buf[buf_index++];
}

/**
 * If the buffer is empty rxFill() sends a SEND_MORE_BYTE to the programmer which then must send
 * sizeof(buf) number of bytes. If the crc sum does not match a SEND_RESEND_BYTE
 * will be sent to the programmer which then should resend the same bytes. When
 * the crc sum matches the MCU will reply with a SEND_OK_BYTE.
 */
void rxFill() {
  unsigned int i;
  unsigned int crc_in;
  unsigned int crc_calc;
  bool done = false;

  if (buf_index == sizeof(buf)) {
    buf_index = 0;
    txUsart1(SEND_MORE_BYTE);
    while (done != true) {
      crc_calc = 0;
      for (i = 0; i < sizeof(buf); ++i) {
        buf[i] = rxUsart1();
        crc_calc = crc(crc_calc, buf[i]);
      }
      crc_in = rxUsart1() << 8;
      crc_in += rxUsart1();
      if (crc_in == crc_calc) {
        done = true;
        txUsart1(SEND_OK_BYTE);
      } else {
        txUsart1(SEND_RESEND_BYTE);
      }
    }
  }
}


/**
 * Function for crc calculations.
 */
unsigned int crc(unsigned int crc, unsigned char data)
{
    unsigned char lo8 = crc & 0x00FF;
    unsigned char hi8 = (crc >> 8) & 0x00FF;
    data ^= lo8;
    data ^= data << 4;

    return ((((unsigned int)data << 8) | hi8 /*hi8 (crc)*/) ^ (unsigned char)(data >> 4) 
        ^ ((unsigned int)data << 3));
}
