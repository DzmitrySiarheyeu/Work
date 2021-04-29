#pragma once

#include "util.h"

#define SET_SPI_CLOCK_DIVIDER_2     sbi(SPSR, SPI2X); cbi(SPCR, SPR1); cbi(SPCR, SPR0);
#define SET_SPI_CLOCK_DIVIDER_4     cbi(SPSR, SPI2X); cbi(SPCR, SPR1); cbi(SPCR, SPR0);
#define SET_SPI_CLOCK_DIVIDER_8     sbi(SPSR, SPI2X); cbi(SPCR, SPR1); sbi(SPCR, SPR0);
#define SET_SPI_CLOCK_DIVIDER_16    cbi(SPSR, SPI2X); cbi(SPCR, SPR1); sbi(SPCR, SPR0);
#define SET_SPI_CLOCK_DIVIDER_32    sbi(SPSR, SPI2X); sbi(SPCR, SPR1); cbi(SPCR, SPR0);
#define SET_SPI_CLOCK_DIVIDER_64    cbi(SPSR, SPI2X); sbi(SPCR, SPR1); cbi(SPCR, SPR0);
#define SET_SPI_CLOCK_DIVIDER_128   cbi(SPSR, SPI2X); sbi(SPCR, SPR1); sbi(SPCR, SPR0);

//-------------SET--------------------

#define SET_SPI_CLOCK   SET_SPI_CLOCK_DIVIDER_4

//-------------SET--------------------

void SPIMasterInit(void);
void SPIWrite(unsigned char a);
