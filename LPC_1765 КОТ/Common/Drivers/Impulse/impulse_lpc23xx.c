	#include "config.h"
#if(_IMPULSE_)

#include <string.h>
#include <stdio.h>
#include "driverscore.h"

#include "impulse_lpc23xx.h"

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

static ImpulseData ImpData = {.DataPoint = 0};

DEVICE Imp_device = {
    .flDeviceOpen = 0,
    .device_io = (DEVICE_IO *)&Imp_device_io,
    .pDriverData = &imp_info,
    .pData = &ImpData
    };



int CreateImp(void)
{

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

int WriteImp(DEVICE_HANDLE handle, void * pSrc, int count)
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
	/*   IMP_LPC17xx_INFO *pInfo = (IMP_LPC17xx_INFO *)(((DEVICE *)handle)->pDriverData);
        ImpulseData *pData = (ImpulseData *)(((DEVICE *)handle)->pData);
        if(count >= 0)
        {
            memset(pDst, 0, count);
            if((pData->DataPoint + count) > MAX_IMP_DATA_OFFSET)
            {
                count = MAX_IMP_DATA_OFFSET - pData->DataPoint;
            }
            if(xSemaphoreTake(xDataText_Sem, 1000 ) == pdTRUE)
            {
#if(NUMBER_OF_CH_IMP > 0)
                sprintf((char *)&BufferText[0], "%10lu", pInfo->ImpulseCounter[0]);
                strcpy((char *)&BufferText[10], " IMP1\n");
#endif

#if(NUMBER_OF_CH_IMP > 1)
                sprintf((char *)&BufferText[16], "%10lu", pInfo->ImpulseCounter[1]);
                strcpy((char *)&BufferText[26], " IMP2\n");
#endif

#if(NUMBER_OF_CH_IMP > 2)
                sprintf((char *)&BufferText[32], "%10lu", pInfo->ImpulseCounter[2]);
                strcpy((char *)&BufferText[42], " IMP3\n");
#endif

#if(NUMBER_OF_CH_IMP > 3)
                sprintf((char *)&BufferText[48], "%10lu", pInfo->ImpulseCounter[3]);
                strcpy((char *)&BufferText[58], " IMP4\n");
#endif
                    
                memcpy(pDst, (char *)(((uint8_t *)BufferText) + pData->DataPoint), count);

                xSemaphoreGive(xDataText_Sem);
                pData->DataPoint += count;
                return count;
            }
        }*/
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
            case RESET_DEVICE:
				reset_counters();
				break;
            default:
                return -1;
        }

        return 0;
}


//сбрасываем счетчики
void reset_sens_counters(int ch)
{
#if(_TELTONIKA_ == OPTION_ON)
    IMP_LPC17xx_INFO *IMP_info = (IMP_LPC17xx_INFO *)Imp_device.pDriverData;
    uint32_t temp_counter = 0;
    uint8_t i;

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
	    IMP_LPC17xx_INFO *IMP_info = (IMP_LPC17xx_INFO *)Imp_device.pDriverData;
//    uint32_t temp_counter = 0;
    uint8_t i;

//	DEVICE_HANDLE hSaver;
//	hSaver = OpenDevice("SAVER"); 

    for(i = 0; i < NUMBER_OF_CH_IMP; i++)
    {

        IMP_info->ImpulseCounter[i] = 0;
        IMP_info->SavedCounter[i] = 0;
        IMP_info->DataReady[i] = 1;

        //записываем буфер
       // SeekDevice(hSaver, i, 0);
      //  WriteDevice(hSaver, (void *)&temp_counter, 1); 
    }
//	CloseDevice(hSaver);
#endif
}

void init_counters(void)
{
#if(_TELTONIKA_ == OPTION_ON)
	uint32_t count;
    uint8_t i;
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

//    uint32_t count;
    //uint16_t CRC;
    uint8_t i;
//    int err = 0;



	//DEVICE_HANDLE hSaver;
	//hSaver = OpenDevice("SAVER"); 

    for(i = 0; i < NUMBER_OF_CH_IMP; i++)
    {
      /*  //считываем буфер
        SeekDevice(hSaver, i, 0);
        err = ReadDevice(hSaver, (void *)&count, 1);

        if(err != -1)
        {  
            //загружаем значение в счетчик
            imp_info.ImpulseCounter[i] = 0;
            imp_info.SavedCounter[i] = count;
        }
        else
        { */
            imp_info.ImpulseCounter[i] = 0;
            imp_info.SavedCounter[i] = 0;
        //}
        imp_info.DataReady[i] = 1;
    }
	//CloseDevice(hSaver);
//	reset_counters(); 
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


#endif // _IMPULS_




