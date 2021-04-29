	#include "config.h"
#if(_IMPULSE_)

#include <string.h>
#include <stdio.h>

#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"

#include "driverscore.h"

#include "impulse_stm32f10x.h"
#include "Freq_stm32f10x.h"

extern FREQ_INFO freq_info;

static int CreateImp(void);
static DEVICE_HANDLE OpenImp(void);
static void CloseImp(DEVICE_HANDLE handle);
static int ReadImp(DEVICE_HANDLE handle, void * dst, int count);
static int WriteImp(DEVICE_HANDLE handle, void * dst, int count);
static int SeekImp(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);

static int ReadImpText(DEVICE_HANDLE handle, void * dst, int count);
static int WriteImpText(DEVICE_HANDLE handle, void * dst, int count);
static int SeekImpText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);

static int IOCtlImp(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData);


IMP_STM32F1_INFO imp_info = {.ReadMode = READ_WITH_SHIFT};

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

static ImpulseData ImpData = {.DataPoint = 0};

DEVICE Imp_device = {
    .flDeviceOpen = 0,
    .device_io = (DEVICE_IO *)&Imp_device_io,
    .pDriverData = &imp_info,
    .pData = &ImpData
    };



int CreateImp(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/* Configure PA.00 pin as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource13);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
	
	/* Configure EXTI0 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line12 | EXTI_Line13 | EXTI_Line14;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	/* Enable and set EXTI0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

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
    IMP_STM32F1_INFO *pInfo = (IMP_STM32F1_INFO *)(((DEVICE *)handle)->pDriverData);

    if( offset >= NUMBER_OF_CH_IMP) 
        return -1;
    else pInfo->DataPoint = offset;

    return offset;
}

int WriteImp(DEVICE_HANDLE handle, void * pSrc, int count)
{
    return -1;
}

int ReadImp(DEVICE_HANDLE handle, void * pDst, int count) 
{
    IMP_STM32F1_INFO *pInfo = (IMP_STM32F1_INFO *)(((DEVICE *)handle)->pDriverData);

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
 	IMP_STM32F1_INFO *pInfo = (IMP_STM32F1_INFO *)(((DEVICE *)handle)->pDriverData);
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
    IMP_STM32F1_INFO *pInfo = (IMP_STM32F1_INFO *)(((DEVICE *)handle)->pDriverData);

        switch(code)
        {
            case SET_READ_MODE:
                pInfo->ReadMode = *pData;
                break;
                     
            default:
                return -1;
        }

        return 0;
}


//сбрасываем счетчики
void reset_sens_counters(int ch)
{

    IMP_STM32F1_INFO *IMP_info = (IMP_STM32F1_INFO *)Imp_device.pDriverData;
    uint8_t i;
	uint32_t temp_counter = 0;

#if(_TELTONIKA_ == OPTION_ON)

	DEVICE_HANDLE hSaver;
	hSaver = OpenDevice("SAVER"); 

    //for(i = 0; i < NUMBER_OF_CH_IMP; i++)
    //{

        IMP_info->ImpulseCounter[ch] = 0;
        IMP_info->SavedCounter[ch] = 0;
        IMP_info->DataReady[ch] = 1;

        //записываем буфер
        SeekDevice(hSaver, 0, ch+IMP_1_SEG);
        WriteDevice(hSaver, (void *)&temp_counter, 1); 
    //}
	CloseDevice(hSaver);
#else


    for(i = 0; i < NUMBER_OF_CH_IMP; i++)
    {

        IMP_info->ImpulseCounter[i] = 0;
        IMP_info->DataReady[i] = 1;
    }
#endif

}

void init_counters(void)
{
    uint8_t i;

#if(_TELTONIKA_ == OPTION_ON)
	uint32_t count;
    int err = 0;



	DEVICE_HANDLE hSaver;
	hSaver = OpenDevice("SAVER"); 

    for(i = 0; i < NUMBER_OF_CH_IMP; i++)
    {
        //считываем буфер
        SeekDevice(hSaver, 0, i+IMP_1_SEG);
        err = ReadDevice(hSaver, (void *)&count, 1);

        if(err != -1)
        {  
            //загружаем значение в счетчик
            imp_info.ImpulseCounter[i] = 0;
            imp_info.SavedCounter[i] = count;
        }
        else
        { 
            imp_info.ImpulseCounter[i] = 0;
            imp_info.SavedCounter[i] = 0;
        }
        imp_info.DataReady[i] = 1;
    }
	CloseDevice(hSaver);
//	reset_counters();
#else

    for(i = 0; i < NUMBER_OF_CH_IMP; i++)
    {
        imp_info.ImpulseCounter[i] = 0;
        imp_info.DataReady[i] = 1;
    }
#endif
}

//сохраняем счетчики
void save_counters(uint8_t ch)
{
#if(_TELTONIKA_ == OPTION_ON)								  
	DEVICE_HANDLE hSaver;
	hSaver = OpenDevice("SAVER");

        //записываем буфер
        SeekDevice(hSaver, 0, ch+IMP_1_SEG);
        WriteDevice(hSaver, (void *)&(imp_info.SavedCounter[ch]), 1);

	CloseDevice(hSaver); 
#endif
}

void EXTI15_10_IRQ(void)
{
	if(EXTI_GetITStatus(EXTI_Line14) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line14);

		imp_info.ImpulseCounter[0]++;
		imp_info.SavedCounter[0]++;
		imp_info.DataReady[0] = 1;

		freq_info.Freq_Count[0]++;
	}

	if(EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line13);

		imp_info.ImpulseCounter[1]++;
		imp_info.SavedCounter[1]++;
		imp_info.DataReady[1] = 1;

		freq_info.Freq_Count[1]++;
	}

	if(EXTI_GetITStatus(EXTI_Line12) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line12);

		imp_info.ImpulseCounter[2]++;
		imp_info.SavedCounter[2]++;
		imp_info.DataReady[2] = 1;

		freq_info.Freq_Count[2]++;
	}
}


#endif // _IMPULS_




