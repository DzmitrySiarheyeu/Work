#include "spi.h"

inline void SPIMasterInit(void)
{
    cbi(PRR,  PRSPI);   //On SPI

    sbi(SCK_DDR,    SCK_PIN);
	sbi(MOSI_DDR,   MOSI_PIN);
    sbi(SS_DDR,     SS_PIN);
    sbi(POT_SS_DDR, POT_SS_PIN);

    UNSELECT_POT;
    
    SET_SPI_CLOCK;

    sbi(SPCR, MSTR);    //Master
    sbi(SPCR, SPE);     //ON
}

inline void SPIWrite(unsigned char a)
{
	SPDR = a;					
	while(!(SPSR & (1 << SPIF)));
}
