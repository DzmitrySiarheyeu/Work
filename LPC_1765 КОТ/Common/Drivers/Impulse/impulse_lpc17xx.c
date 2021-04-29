#include "config.h"
#if(_IMPULSE_)

#include <string.h>
#include <stdio.h>
#include "driverscore.h"
#include "LPC17xx.h" 
#include "lpc17xx_gpio.h"

#include "impulse_lpc17xx.h"

static int CreateImp(void);
static DEVICE_HANDLE OpenImp(void);
static void CloseImp(DEVICE_HANDLE handle);
static int ReadImp(DEVICE_HANDLE handle, void * dst, int count);
static int WriteImp(DEVICE_HANDLE handle, const void * dst, int count);
static int SeekImp(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);

static int ReadImpText(DEVICE_HANDLE handle, void * dst, int count);
static int WriteImpText(DEVICE_HANDLE handle, void * dst, int count);
static int SeekImpText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);

static int IOCtlImp(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData);

void AwakeResetTask(int isr_fl, char *data);


IMP_LPC17xx_INFO imp_info = {.ReadMode = READ_WITH_SHIFT};

const DEVICE_IO Imp_device_io = {
    .DeviceID = IMP,
	.CreateDevice =	CreateImp,
	.CloseDevice = CloseImp,
    .OpenDevice = OpenImp,
    .ReadDevice = ReadImp,
    .WriteDevice = WriteImp,
    .SeekDevice = SeekImp,
    .ReadDeviceText = ReadImpText,
    .WriteDeviceText = WriteImpText,
    .SeekDeviceText = SeekImpText,
    .DeviceIOCtl = IOCtlImp,
    .DeviceName = "IMPULSE"
    };

ImpulseData ImpData = {.DataPoint = 0};

DEVICE Imp_device = {
    .flDeviceOpen = 0,
    .device_io = (DEVICE_IO *)&Imp_device_io,
    .pDriverData = &imp_info,
    .pData = &ImpData
    };



int CreateImp(void)
{

//#if(NUMBER_OF_CH_FR_IMP > 4)
//	#error "NUMBER_OF_CH_IMP MUST NOT BE MORE THAN 4"
//#endif
	
	GPIO_SetDir(UIN_CAP_CH_PORT, 1 << UIN_CAP_CH_PIN, 0);
#if(UIN_CAP_CH_PORT == 2)
	LPC_GPIOINT->IO2IntEnF |= (1 << UIN_CAP_CH_PIN);
	LPC_GPIOINT->IO2IntEnR &= ~(1 << UIN_CAP_CH_PIN);
	LPC_GPIOINT->IO2IntClr |= (1 << UIN_CAP_CH_PIN);	
#elif(UIN_CAP_CH_PORT == 0)
	LPC_GPIOINT->IO0IntEnF |= (1 << UIN_CAP_CH_PIN);
	LPC_GPIOINT->IO0IntEnR &= ~(1 << UIN_CAP_CH_PIN);
	LPC_GPIOINT->IO0IntClr |= (1 << UIN_CAP_CH_PIN);
#else	
	#error "UIN_CAP_CH_PORT MUST  BE == 2 OR == 0"
#endif

#if(NUMBER_OF_CH_FR_IMP > 0)
		
	#if(IMP_CH_0_PORT == 1)
		#error "IMPOSSIBLE SET INTERRUPT ON PORT 1"
	#endif

	GPIO_SetDir(IMP_CH_0_PORT, 1 << IMP_CH_0_PIN, 0);

	#if(IMP_CH_0_PORT == 0)
		LPC_GPIOINT->IO0IntEnR |= (1 << IMP_CH_0_PIN);
		LPC_GPIOINT->IO0IntEnF &= ~(1 << IMP_CH_0_PIN);
		LPC_GPIOINT->IO0IntClr |= (1 << IMP_CH_0_PIN);
	#else
		LPC_GPIOINT->IO2IntEnR |= (1 << IMP_CH_0_PIN);
		LPC_GPIOINT->IO2IntEnF &= ~(1 << IMP_CH_0_PIN);
		LPC_GPIOINT->IO2IntClr |= (1 << IMP_CH_0_PIN);
	#endif
#endif

#if(NUMBER_OF_CH_FR_IMP > 1)
		
	#if(IMP_CH_1_PORT == 1)
		#error "IMPOSSIBLE SET INTERRUPT ON PORT 1"
	#endif

	GPIO_SetDir(IMP_CH_1_PORT, 1 << IMP_CH_1_PIN, 0);

	#if(IMP_CH_0_PORT == 0)
		LPC_GPIOINT->IO0IntEnR |= (1 << IMP_CH_1_PIN);
		LPC_GPIOINT->IO0IntEnF &= ~(1 << IMP_CH_1_PIN);
		LPC_GPIOINT->IO0IntClr |= (1 << IMP_CH_1_PIN);
	#else
		LPC_GPIOINT->IO2IntEnR |= (1 << IMP_CH_1_PIN);
		LPC_GPIOINT->IO2IntEnF &= ~(1 << IMP_CH_1_PIN);
		LPC_GPIOINT->IO2IntClr |= (1 << IMP_CH_1_PIN);
	#endif
#endif

#if(NUMBER_OF_CH_FR_IMP > 2)
		
	#if(IMP_CH_2_PORT == 1)
		#error "IMPOSSIBLE SET INTERRUPT ON PORT 1"
	#endif

	GPIO_SetDir(IMP_CH_2_PORT, 1 << IMP_CH_2_PIN, 0);

	#if(IMP_CH_0_PORT == 0)
		LPC_GPIOINT->IO0IntEnR |= (1 << IMP_CH_2_PIN);
		LPC_GPIOINT->IO0IntEnF &= ~(1 << IMP_CH_2_PIN);
		LPC_GPIOINT->IO0IntClr |= (1 << IMP_CH_2_PIN);
	#else
		LPC_GPIOINT->IO2IntEnR |= (1 << IMP_CH_2_PIN);
		LPC_GPIOINT->IO2IntEnF &= ~(1 << IMP_CH_2_PIN);
		LPC_GPIOINT->IO2IntClr |= (1 << IMP_CH_2_PIN);
	#endif
#endif

#if(NUMBER_OF_CH_FR_IMP > 3)
		
	#if(IMP_CH_3_PORT == 1)
		#error "IMPOSSIBLE SET INTERRUPT ON PORT 1"
	#endif

	GPIO_SetDir(IMP_CH_3_PORT, 1 << IMP_CH_3_PIN, 0);

	#if(IMP_CH_0_PORT == 0)
		LPC_GPIOINT->IO0IntEnR |= (1 << IMP_CH_3_PIN);
		LPC_GPIOINT->IO0IntEnF &= ~(1 << IMP_CH_3_PIN);
		LPC_GPIOINT->IO0IntClr |= (1 << IMP_CH_3_PIN);
	#else
		LPC_GPIOINT->IO2IntEnR |= (1 << IMP_CH_3_PIN);
		LPC_GPIOINT->IO2IntEnF &= ~(1 << IMP_CH_3_PIN);
		LPC_GPIOINT->IO2IntClr |= (1 << IMP_CH_3_PIN);
	#endif
#endif

    // Прерывания по портам - тот же адрес, что и EINT3
    NVIC_EnableIRQ(EINT3_IRQn);
	NVIC_SetPriority(EINT3_IRQn, 31);


    DEBUG_PUTS("IMP Created\n");
    return DEVICE_OK;
}

DEVICE_HANDLE OpenImp(void)
{
	if(Imp_device.flDeviceOpen == DEVICE_CLOSE)
	{
		 Imp_device.flDeviceOpen = DEVICE_OPEN;
	}
	else
	{
		  DEBUG_PUTS("IMP RE_OPEN\n");
	}

	return (DEVICE_HANDLE)&Imp_device;
}


void CloseImp(DEVICE_HANDLE handle)
{
	if(Imp_device.flDeviceOpen == DEVICE_OPEN)
	{
		 Imp_device.flDeviceOpen = DEVICE_CLOSE;
	}
	else
	{
		  DEBUG_PUTS("IMP RE_CLOSE\n");
	}
}



static int SeekImp(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
    IMP_LPC17xx_INFO *pInfo = (IMP_LPC17xx_INFO *)(((DEVICE *)handle)->pDriverData);

    if( offset > NUMBER_OF_CH_IMP) 
        return -1;
    else pInfo->DataPoint = offset;

    return offset;
}

int WriteImp(DEVICE_HANDLE handle, const void * pSrc, int count)
{
    return -1;
}

int ReadImp(DEVICE_HANDLE handle, void * pDst, int count) 
{
    IMP_LPC17xx_INFO *pInfo = (IMP_LPC17xx_INFO *)(((DEVICE *)handle)->pDriverData);

        if(pInfo->ReadMode == READ_WITH_SHIFT)
        {
            if(pInfo->DataReady[pInfo->DataPoint])
            {
                *((uint32_t *)pDst) = pInfo->SavedCounter[pInfo->DataPoint];
                pInfo->DataReady[pInfo->DataPoint] = 0;

                return 1;
            }
            
            return 0;
        }
        else if(pInfo->ReadMode == SAVE_COUNTER_READ)
        {
            *((uint32_t *)pDst) = pInfo->SavedCounter[pInfo->DataPoint];
            pInfo->ReadMode = READ_WITH_SHIFT;
            return 1;
        }
        else
        {
            *((uint32_t *)pDst) = pInfo->ImpulseCounter[pInfo->DataPoint];
            return 1;
        }
}



int WriteImpText(DEVICE_HANDLE handle, void * pDst, int count)
{
    return -1;
}

int ReadImpText(DEVICE_HANDLE handle, void * pDst, int count)
{
	IMP_LPC17xx_INFO *pInfo = (IMP_LPC17xx_INFO *)(((DEVICE *)handle)->pDriverData);
    ImpulseData *pData = (ImpulseData *)(((DEVICE *)handle)->pData);
    int i;
    char *BufferText;
	int sprintf_count = 0;
	
	if(count >= 0)
	{
		memset(pDst, 0, count);
		BufferText = pvPortMalloc(300);
		if(BufferText != NULL)
        {
			for(i = 0; i < NUMBER_OF_CH_DIS; i++)
			{
				 sprintf_count += sprintf(BufferText + sprintf_count, "IMP%d %10u\n", i + 1, pInfo->ImpulseCounter[i]);
			}
                    
            if((pData->DataPoint + count) > sprintf_count)
			{
				count = sprintf_count - pData->DataPoint;
			}
			memcpy(pDst, (char *)(((uint8_t *)BufferText) + pData->DataPoint), count);
			pData->DataPoint += count;
			vPortFree(BufferText);
            return count;
		}
	}
	return 0;
}

int SeekImpText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{

        ImpulseData *pData = (ImpulseData *)(((DEVICE *)handle)->pData);
        switch(origin)
        {
            case 0:
                if(offset < MAX_IMP_DATA_OFFSET)
                {
                    pData->DataPoint = offset;
                }
                else
                {
                    pData->DataPoint = 0;
                    return 0;
                }
                break;
            case 1:
                if((pData->DataPoint + offset) < MAX_IMP_DATA_OFFSET)
                {
                    pData->DataPoint = pData->DataPoint + offset;
                }
                else
                {
                    pData->DataPoint = 0;
                    return 0;
                } 
                break;
            case 2:
                if(offset < MAX_IMP_DATA_OFFSET)
                {
                    pData->DataPoint = MAX_IMP_DATA_OFFSET - offset;
                }
                else
                {
                    pData->DataPoint = 0;
                    return 0;
                } 
                break;
            default:
                pData->DataPoint = 0;
                return 0;            
        }
        return pData->DataPoint;
}


int IOCtlImp(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData)
{
    IMP_LPC17xx_INFO *pInfo = (IMP_LPC17xx_INFO *)(((DEVICE *)handle)->pDriverData);

        switch(code)
        {
            case SET_READ_MODE:
                pInfo->ReadMode = *pData;
                break;
			case SET_VALID_DATA:
				pInfo->DataReady[*pData] = 1;
                break;
                     
            default:
                return -1;
        }

        return 0;
}

#if(BOD_TASK == DEVICE_ON)
	extern xTaskHandle BODTaskHandle;
#endif

void EINT3_IRQ(void)
{
#if(NUMBER_OF_CH_FR_IMP > 0)

	IMP_LPC17xx_INFO *pInfo = (IMP_LPC17xx_INFO *)Imp_device.pDriverData;

	#if(IMP_CH_0_PORT == 0)
		if((LPC_GPIOINT->IO0IntStatR & (1 << IMP_CH_0_PIN)) > 0)  // IMP1
	    {
	        LPC_GPIOINT->IO0IntClr |= (1 << IMP_CH_0_PIN);
	        pInfo->ImpulseCounter[0] += 1;
	        pInfo->SavedCounter[0] += 1;
	        pInfo->DataReady[0] = 1;
	    }
	#else
		if((LPC_GPIOINT->IO2IntStatR & (1 << IMP_CH_0_PIN)) > 0)  // IMP1
	    {
	        LPC_GPIOINT->IO2IntClr |= (1 << IMP_CH_0_PIN);
	        pInfo->ImpulseCounter[0] += 1;
	        pInfo->SavedCounter[0] += 1;
	        pInfo->DataReady[0] = 1;
	    }
	#endif
#endif

#if(NUMBER_OF_CH_FR_IMP > 1)
		
	#if(IMP_CH_1_PORT == 0)
		if((LPC_GPIOINT->IO0IntStatR & (1 << IMP_CH_1_PIN)) > 0)  // IMP1
	    {
	        LPC_GPIOINT->IO0IntClr |= (1 << IMP_CH_1_PIN);
	        pInfo->ImpulseCounter[1] += 1;
	        pInfo->SavedCounter[1] += 1;
	        pInfo->DataReady[1] = 1;
	    }
	#else
		if((LPC_GPIOINT->IO2IntStatR & (1 << IMP_CH_1_PIN)) > 0)  // IMP1
	    {
	        LPC_GPIOINT->IO2IntClr |= (1 << IMP_CH_1_PIN);
	        pInfo->ImpulseCounter[1] += 1;
	        pInfo->SavedCounter[1] += 1;
	        pInfo->DataReady[1] = 1;
	    }
	#endif
#endif

#if(NUMBER_OF_CH_FR_IMP > 2)
		
	#if(IMP_CH_2_PORT == 0)
		if((LPC_GPIOINT->IO0IntStatR & (1 << IMP_CH_2_PIN)) > 0)  // IMP1
	    {
	        LPC_GPIOINT->IO0IntClr |= (1 << IMP_CH_2_PIN);
	        pInfo->ImpulseCounter[2] += 1;
	        pInfo->SavedCounter[2] += 1;
	        pInfo->DataReady[2] = 1;
	    }
	#else
		if((LPC_GPIOINT->IO2IntStatR & (1 << IMP_CH_2_PIN)) > 0)  // IMP1
	    {
	        LPC_GPIOINT->IO2IntClr |= (1 << IMP_CH_2_PIN);
	        pInfo->ImpulseCounter[2] += 1;
	        pInfo->SavedCounter[2] += 1;
	        pInfo->DataReady[2] = 1;
	    }
	#endif
#endif

#if(NUMBER_OF_CH_FR_IMP > 3)
		
	#if(IMP_CH_3_PORT == 0)
		if((LPC_GPIOINT->IO0IntStatR & (1 << IMP_CH_3_PIN)) > 0)  // IMP1
	    {
	        LPC_GPIOINT->IO0IntClr |= (1 << IMP_CH_3_PIN);
	        pInfo->ImpulseCounter[3] += 1;
	        pInfo->SavedCounter[3] += 1;
	        pInfo->DataReady[3] = 1;
	    }
	#else
		if((LPC_GPIOINT->IO2IntStatR & (1 << IMP_CH_3_PIN)) > 0)  // IMP1
	    {
	        LPC_GPIOINT->IO2IntClr |= (1 << IMP_CH_3_PIN);
	        pInfo->ImpulseCounter[3] += 1;
	        pInfo->SavedCounter[3] += 1;
	        pInfo->DataReady[3] = 1;
	    }
	#endif
#endif

		if((LPC_GPIOINT->IO2IntStatF & (1 << UIN_CAP_CH_PIN)) > 0)  // IMP1
	  {
			LPC_GPIOINT->IO2IntClr |= (1 << UIN_CAP_CH_PIN);
			AwakeResetTask(1, "UIN CAP - POWER OFF");
	     //  vTaskResume( BODTaskHandle );
	  }	
}

//сбрасываем счетчики
void reset_counters(uint8_t ch)
{

    IMP_LPC17xx_INFO *IMP_info = (IMP_LPC17xx_INFO *)Imp_device.pDriverData;
//    uint32_t temp_counter = 0;
 //   uint8_t i;
//	DEVICE_HANDLE hSaver;
//	hSaver = OpenDevice("SAVER");

    IMP_info->ImpulseCounter[ch] = 0;
    IMP_info->SavedCounter[ch] = 0;
    IMP_info->DataReady[ch] = 1;


//	SeekDevice(hSaver, 0, ch + IMP_1_SEG);
   // WriteDevice(hSaver, (void *)&(imp_info.SavedCounter[ch]), 1);

//	CloseDevice(hSaver);
}

void init_counters(void)
{
   // uint32_t count;
    uint8_t i;
  //  int err = 0;



//	DEVICE_HANDLE hSaver;
//	hSaver = OpenDevice("SAVER");

    for(i = IMP_1_SEG; i < NUMBER_OF_CH_IMP + IMP_1_SEG; i++)
    {
//        считываем буфер
     //   SeekDevice(hSaver, 0, i);
    //    err = ReadDevice(hSaver, (void *)&count, 1);

     //   if(err != -1)
    //    {
    //        //загружаем значение в счетчик
    //        imp_info.ImpulseCounter[i] = 0;
    //        imp_info.SavedCounter[i] = count;
    //    }
    //    else
    //    {
            imp_info.ImpulseCounter[i] = 0;
            imp_info.SavedCounter[i] = 0;
    //    }
        imp_info.DataReady[i] = 1;
    }
    //reset_counters();
//	CloseDevice(hSaver);
}

//сохраняем счетчики
void save_counters(uint8_t ch)
{

//	DEVICE_HANDLE hSaver;
//	hSaver = OpenDevice("SAVER");


        //записываем буфер
   //  SeekDevice(hSaver, 0, ch + IMP_1_SEG);
   //  WriteDevice(hSaver, (void *)&(imp_info.SavedCounter[ch]), 1);
  //
//	 CloseDevice(hSaver);

}


#endif // _IMPULS_




