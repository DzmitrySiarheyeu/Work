#ifndef IMPULSE_H
#define IMPULSE_H
#include "Config.h"
#if(_IMPULSE_)

#include <stdint.h>
#include "DriversCore.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define MAX_IMPULSE_DATA     1

#define POOL_IMP 0
#define SAVED_DATA   2

#define IMP_DEVICE_QUANTITY        4

typedef struct tagIMP_LM_INFO	
{
        uint32_t ImpulseCounter[4];
        uint8_t  DataReady[4];
        uint32_t SavedCounter[4];
        uint8_t         DataPoint;
        //xSemaphoreHandle Sem;
}IMP_LM_INFO;


extern DEVICE Imp_device;

extern DEVICE_HANDLE hImp;

#endif

#endif


