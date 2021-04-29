	  #ifndef IMPULSE_H
#define IMPULSE_H
#include "Config.h"
#if(_IMPULSE_)

#include <stdint.h>										  
#include "DriversCore.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define MAX_IMP_DATA_OFFSET   (NUMBER_OF_CH_IMP * 16)

#define SAVE_COUNTER_READ      99



typedef struct __attribute__ ((__packed__)) tagIMP_STM32F1_INFO	
{
        uint8_t  ReadMode;
        uint32_t ImpulseCounter[NUMBER_OF_CH_IMP];
        uint8_t  DataReady[NUMBER_OF_CH_IMP];
		uint32_t SavedCounter[NUMBER_OF_CH_IMP];
        uint8_t         DataPoint;
}IMP_STM32F1_INFO;

typedef struct __attribute__ ((__packed__)) tagImpulseData
{
    uint16_t DataPoint;
}ImpulseData;

extern IMP_STM32F1_INFO imp_info;


void init_counters(void);
void reset_sens_counters(int ch);

#endif // _IMPULSE_

#endif


