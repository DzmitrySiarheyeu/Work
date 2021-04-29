#ifndef I2C_H
#define I2C_H
#include "Config.h"
#if(_I2C_)

#include <stdint.h>
#include "DriversCore.h"

//#define RD_BIT    0x01

#define MAX_I2C_DATA_OFFSET    30

typedef struct __attribute__ ((__packed__)) tagI2CData
{
    uint32_t tx_count;
    uint32_t rx_count;
    uint32_t DataPoint;
}I2CData;

typedef struct tagI2C_INFO_17xx
{
      void  *pI2C;
	  uint8_t Dev_Adr;
      uint8_t Mem_Adr;
      #if(_MULTITASKING_)
      xSemaphoreHandle Sem;
	  #else
	  uint8_t Sem;
	  #endif
}I2C_INFO_17xx;


#endif

#endif


