#include "Config.h"
#if(_IMPULSE_)
#include <stdint.h>
#include <interrupt.h>
#include <timer.h>
#include <gpio.h>
#include <hw_ints.h>
#include <hw_timer.h>
#include <hw_memmap.h>
#include <hw_types.h>
#include <lm3s808.h>
#include "driverscore.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "Impulse/Impulse_lm.h"

static int OpenImp(DEVICE_HANDLE handle);
static int ReadImp(DEVICE_HANDLE handle, void * dst, int count);
static int WriteImp(DEVICE_HANDLE handle, void * dst, int count);
static int SeekImp(DEVICE_HANDLE handle, uint16_t  offset, uint8_t origin);
static int IOCtlImp(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData);


IMP_LM_INFO imp_info;

DEVICE_HANDLE hImp;


const DEVICE_IO Imp_device_io = {
    .DeviceID = IMP,
    .OpenDevice = OpenImp,
    .ReadDevice = ReadImp,
    .WriteDevice = WriteImp,
    .SeekDevice = SeekImp,
    .ReadDeviceText = 0,
    .WriteDeviceText = 0,
    .SeekDeviceText = 0,
    .DeviceIOCtl = IOCtlImp,
    .DeviceName = "Imp"
    };

DEVICE Imp_device = {
    .pDriverList = NULL,
    .pNext = NULL,
    .flDeviceOpen = 0,
    .device_io = (DEVICE_IO *)&Imp_device_io,
    .pDriverData = &imp_info,
    .pData = 0
    };



int SeekImp(DEVICE_HANDLE handle, uint16_t  offset, uint8_t origin)
{
    IMP_LM_INFO *pImpInfo = (IMP_LM_INFO *)(((DEVICE *)handle)->pDriverData);

    if(offset > 3) return -1;

    pImpInfo->DataPoint = (uint8_t )offset;

    return 0;
}


int WriteImp(DEVICE_HANDLE handle, void * pSrc, int count)
{
    return -1;
}

int ReadImp(DEVICE_HANDLE handle, void * pDst, int count) {

    IMP_LM_INFO *pImpInfo = (IMP_LM_INFO *)(((DEVICE *)handle)->pDriverData);

    uint8_t buf_number = 0;
//    uint16_t i;

    buf_number = pImpInfo->DataPoint;

    //if( xSemaphoreTake( pImpInfo->Sem, 1000 ) == pdTRUE )    //   ќжидаем симафора доступа к GPS данным
    //{
        
           // for(i = 0; i < count; i++)
           // {
               // if (pImpInfo->in == pImpInfo->out)
                //{
                 // xSemaphoreGive(pImpInfo->Sem);
                //  return i;
               // }
               switch(count)
               {
                  case 1 :
                           if(pImpInfo->DataReady[buf_number])
                           {
                                *((uint32_t *)pDst) = pImpInfo->SavedCounter[buf_number];
                                pImpInfo->DataReady[buf_number] = 0;
                                return 1;
                           }
                           else return 0;

                   case 2 :*((uint32_t *)pDst) = pImpInfo->ImpulseCounter[buf_number];
                            return 1;
                   case 3 :*((uint32_t *)pDst) = pImpInfo->SavedCounter[buf_number];
                            return 1;
                   
                   default : return 0;
               }
                //if(pImpInfo->out == MAX_IMPULSE_DATA) pImpInfo->out = 0;
           // }

          //  xSemaphoreGive(pImpInfo->Sem);

   // }
  //  return -1;

}

int IOCtlImp(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData)
{
    int err = 0;
   
    IMP_LM_INFO *pImpInfo = (IMP_LM_INFO *)(((DEVICE *)handle)->pDriverData);


    switch(code)
    {
        case SAVED_DATA:
            ((uint32_t *)pData)[0] = pImpInfo->SavedCounter[pImpInfo->DataPoint];
            break;
        case SET_READ_BUF:
            if(*pData > (IMP_DEVICE_QUANTITY - 1)) return -1;
            pImpInfo->DataPoint = (uint8_t)pData[0];
            break;
    }

    return err;
}

int OpenImp(DEVICE_HANDLE handle)
{
    IMP_LM_INFO *pImpInfo = (IMP_LM_INFO *)(((DEVICE *)handle)->pDriverData);
    
    int ch = 0;
    for(ch = 0; ch < 4; ch ++)
    {
        pImpInfo->ImpulseCounter[ch] = 0;
    }

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0);
    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_RISING_EDGE);
    IntEnable(INT_GPIOB);
    GPIOPinIntEnable(GPIO_PORTB_BASE, GPIO_PIN_0);

    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_1);
    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_RISING_EDGE);
    IntEnable(INT_GPIOB);
    GPIOPinIntEnable(GPIO_PORTB_BASE, GPIO_PIN_1);

    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_6);
    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_RISING_EDGE);
    IntEnable(INT_GPIOC);
    GPIOPinIntEnable(GPIO_PORTC_BASE, GPIO_PIN_6);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_7);
    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_7, GPIO_RISING_EDGE);
    IntEnable(INT_GPIOC);
    GPIOPinIntEnable(GPIO_PORTC_BASE, GPIO_PIN_7);

   return ((DEVICE *)handle)->flDeviceOpen = 0x00; // true
}

void GPIO_Port_B_ISR(void)
{
    
    if(GPIOPinIntStatus(GPIO_PORTB_BASE, true) & GPIO_PIN_0)
    {
        GPIOPinIntClear(GPIO_PORTB_BASE, GPIO_PIN_0); 
        IMP_LM_INFO *p = (IMP_LM_INFO *)&imp_info;
        p->ImpulseCounter[0]++;
        p->SavedCounter[0]++;
        p->DataReady[0] = 1;
        //if(p->in == MAX_IMPULSE_DATA) p->in = 0;
    }
    if(GPIOPinIntStatus(GPIO_PORTB_BASE, true) & GPIO_PIN_1)
    {
        GPIOPinIntClear(GPIO_PORTB_BASE, GPIO_PIN_1);
        IMP_LM_INFO *p = (IMP_LM_INFO *)&imp_info;
        p->ImpulseCounter[1]++;
        p->SavedCounter[1]++;
        p->DataReady[1] = 1;
        //if(p->in == MAX_IMPULSE_DATA) p->in = 0;
    }
}

void GPIO_Port_C_ISR(void)
{
    
    if(GPIOPinIntStatus(GPIO_PORTC_BASE, true) & GPIO_PIN_6)
    {
        GPIOPinIntClear(GPIO_PORTC_BASE, GPIO_PIN_6);
        IMP_LM_INFO *p = (IMP_LM_INFO *)&imp_info;
        p->ImpulseCounter[2]++;
        p->SavedCounter[2]++;
        p->DataReady[2] = 1;
        //if(p->in == MAX_IMPULSE_DATA) p->in = 0;
    }
    if(GPIOPinIntStatus(GPIO_PORTC_BASE, true) & GPIO_PIN_7)
    {
        GPIOPinIntClear(GPIO_PORTC_BASE, GPIO_PIN_7);
        IMP_LM_INFO *p = (IMP_LM_INFO *)&imp_info;
        p->ImpulseCounter[3]++;
        p->SavedCounter[3]++;
        p->DataReady[3] = 1;
       // if(p->in == MAX_IMPULSE_DATA) p->in = 0;
    }
}
#endif




