#include "potentiometer.h"
#include "spi.h"

double PotValue; 

inline void PotInit(void)
{
    SPIMasterInit();
    
    PotValue = (POT_MAX_VALUE - POT_MIN_VALUE) / 2;
    
    PotSet(PotValue);

}

inline void PotSet(unsigned char b)
{
    SELECT_POT;
    SPIWrite(WRITE_DATA_0_COMMAND); // command
    SPIWrite(b);                    // data byte
    UNSELECT_POT;
}

