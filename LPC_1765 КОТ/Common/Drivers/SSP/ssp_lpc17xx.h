#ifndef SSP_H
#define SSP_H

#include "config.h"


#if(_SSP_)
#include "DriversCore.h"


#define MAX_SSP_DATA_OFFSET    30

typedef struct __attribute__ ((__packed__)) tagSSPData
{
    uint32_t tx_count;
    uint32_t rx_count;
    uint16_t DataPoint;
}SSPData;

typedef struct tagSSP_INFO_17xx
{
	void  *pSSP;

#if(_MULTITASKING_)
	volatile xSemaphoreHandle Sem;
#else
	uint8_t Sem;
#endif
	
}SSP_INFO_17xx;
#endif

#endif

