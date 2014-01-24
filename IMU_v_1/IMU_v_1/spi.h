#ifndef SPI_H
#define SPI_H

#include "global.h"

void spiInit(void);
void spiSendByte(uint8_t data);
uint32_t spiTransferAll(uint32_t data, int CS);

#endif