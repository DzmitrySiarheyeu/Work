#include "clock_drv.h"
#if _CLOCK_
#include "SetParametrs.h"
#include "User_Error.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>



static int CreateClock(void);
static DEVICE_HANDLE OpenClock(void);
static void CloseClock(DEVICE_HANDLE handle);
static int ReadClock(DEVICE_HANDLE handle, void * dst, int count);
static int WriteClock(DEVICE_HANDLE handle, void * src, int count);
static int SeekClock(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
#if _TEXT_
    static int ReadClockText(DEVICE_HANDLE handle, void * dst, int count);
    static int WriteClockText(DEVICE_HANDLE handle, void * src, int count);
    static int SeekClockText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
#endif // _TEXT_

static int ClockIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData);

#if (_TIME_SYNC_GSM_)
 	int time_synk_gsm(DEVICE_HANDLE handle, uint8_t *buf);
#endif

#if (_TIME_SYNC_GPS_)
	#include "GPS_DRV.h"
	int time_synk_gps(DEVICE_HANDLE handle);
#endif

uint64_t time_stamp(DEVICE_HANDLE handle);



volatile CLOCKData  clockDate = {
    .tenth_Sec = NULL,
    .Sec = NULL,
    .Min = NULL,
    .Hour = NULL,
    .Day = NULL,
    .Date = NULL,
    .Month = NULL,
    .Year = NULL,
    .Century = NULL,
	.Hour_delta = 0,
    .DataPointText = NULL,
	.Res_Hour = 22,
	.Res_Min = 0
    };

const DEVICE_IO clock_device_io = {
    .DeviceID = CLOCK,
	.CreateDevice =	CreateClock,
	.CloseDevice = CloseClock,
    .OpenDevice = OpenClock,
    .ReadDevice = ReadClock,
    .WriteDevice = WriteClock,
    .SeekDevice = SeekClock,
    #if _TEXT_
        .ReadDeviceText = ReadClockText,
        .WriteDeviceText = WriteClockText,
        .SeekDeviceText = SeekClockText,
    #else
        .ReadDeviceText = 0,
        .WriteDeviceText = 0,
        .SeekDeviceText = 0,
    #endif  //_TEXT_ 
    .DeviceIOCtl = ClockIOCtl,
    .DeviceName = "Clock"
    };


uint8_t BCD_TO_D(uint8_t x)						//Функция перевода из BCD в десятичный формат
{
	return ((x & 0x0F) + (x >> 4) * 10);
}
uint8_t D_TO_BCD(uint8_t x)						//Функция перевода из десятичного формата в BCD
{
	return (((x / 10) << 4) + x % 10);
}

int ReadClock(DEVICE_HANDLE handle, void * dst, int count)
{
    CLOCK_INFO *pClockInfo = (CLOCK_INFO *)(((DEVICE *)handle)->pDriverData);
	CLOCKData *pData = (CLOCKData *)(((DEVICE *)handle)->pData);

        if(count >= 0) 
        {
            if((pClockInfo->DataPoint + count) > MAX_CLOCK_DATA_OFFSET)
            {
                count = MAX_CLOCK_DATA_OFFSET - pClockInfo->DataPoint;
            }
            memcpy((uint8_t *)dst, (uint8_t *)(((uint8_t *)pData) + pClockInfo->DataPoint), count);
            pClockInfo->DataPoint += count;
            return count;
        }
		return 0;

}

int SeekClock(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
    CLOCK_INFO *pClockInfo = (CLOCK_INFO *)(((DEVICE *)handle)->pDriverData);

        switch(origin)
        {
            case 0:
                if(offset < MAX_CLOCK_DATA_OFFSET)
                {
                    pClockInfo->DataPoint = offset;
                }
                else
                {
                    pClockInfo->DataPoint = 0;
                }
                break;
            case 1:
                if((pClockInfo->DataPoint + offset) < MAX_CLOCK_DATA_OFFSET)
                {
                    pClockInfo->DataPoint = pClockInfo->DataPoint + offset;
                }
                else
                {
                    pClockInfo->DataPoint = 0;
                } 
                break;
            case 2:
                if(offset < MAX_CLOCK_DATA_OFFSET)
                {
                    pClockInfo->DataPoint = MAX_CLOCK_DATA_OFFSET - offset;
                }
                else
                {
                    pClockInfo->DataPoint = 0;
                } 
                break;
            default:
                pClockInfo->DataPoint = 0;
                return -1;                 
        }
        return pClockInfo->DataPoint;
}

int WriteClock(DEVICE_HANDLE handle, void * src, int count)
{
    CLOCK_INFO *pClockInfo = (CLOCK_INFO *)(((DEVICE *)handle)->pDriverData);

    pClockInfo->pI2C = OpenDevice(_CLOCK_L_L_DRIVER_NAME_);
    if(pClockInfo->pI2C == NULL)
    {
      DEBUG_PUTS("CLOCK: unable to open I2C\n");
	  PUT_MES_TO_LOG("CLOCK: unable to open I2C", 0, 1);
      return 0;
    }	


        SeekDevice(pClockInfo->pI2C, /*pClockInfo->DataPoint*/0xff, ADR_CLOCK);

        // Нужно сформировать буфер для передачи записываемого параметра
        uint8_t temp[2];
        // Первый элемент буфера - адрес регистра в часах, куда будем писать
        temp[0] = (uint8_t)pClockInfo->DataPoint;

        // Второй элемент буфера - записываемое значение
        if(pClockInfo->DataPoint >= CENTURY_D)  // В m41t81 за CENTURY_D (включая CENTURY_D) находяться управляющие регистры, поэтому в BCD не переводим
            temp[1] = *(uint8_t *)src;
        else
            temp[1] = D_TO_BCD(*(uint8_t *)src);  // Тут обычное время

        switch(pClockInfo->DataPoint)
        {
            case SEC_D:           // Bit7 по адресу SEC_D отвечает за остановку часов, поэтому его надо сбросить
                temp[1] &= ~(1 << 7);
                break;

            case MIN_D:           // Bit7 по адресу MIN_D должен быть сброшен в 0 (по мануалу)
                temp[1] &= ~(1 << 7);
                break;

            case HOUR_D:          // Bit7 и Bit6 по адресу HOUR_D отвечают за CENTURY BIT (не используются)
                temp[1] &= ~((1 << 7) | (1 << 6));
                break;

            case DAY_D:           // Bit7-Bit3 по адресу DAY_D должены быть сброшены в 0 (по мануалу)
                temp[1] &= ~((1 << 7) | (1 << 6) | (1 << 5) | (1 << 4) | (1 << 3));
                break;

            case DATE_D:          // Bit7 и Bit6 по адресу DATE_D должены быть сброшены в 0 (по мануалу)
                temp[1] &= ~((1 << 7) | (1 << 6));
                break;

            case MONTH_D:           // Bit7-Bit5 по адресу MONTH_D должены быть сброшены в 0 (по мануалу)
                temp[1] &= ~((1 << 7) | (1 << 6) | (1 << 5));
                break;

            default:
                break;
        }

        if(WriteDevice(pClockInfo->pI2C, temp, 2) != -1)
        {
			CloseDevice(pClockInfo->pI2C);
            return count;
        }
		CloseDevice(pClockInfo->pI2C);
		return 0;

}

#if(_TEXT_)
int ReadClockText(DEVICE_HANDLE handle, void * pDst, int count)
{


        CLOCKData *pData = (CLOCKData *)(((DEVICE *)handle)->pData);
        if(count >= 0)
        { 
            memset(pDst, 0, count);
            if((pData->DataPointText + count) > MAX_CLOCK_DATA_TEXT_OFFSET)
            {
                count = MAX_CLOCK_DATA_TEXT_OFFSET - pData->DataPointText;
            }
            if(take_sem(xDataText_Sem, 1000 ) == pdTRUE)
            { 
                sprintf((char *)&BufferText[0], "%2lu", pData->Sec);
                strcpy((char *)&BufferText[2], " Sec\n");
                sprintf((char *)&BufferText[7], "%2lu", pData->Min);
                strcpy((char *)&BufferText[9], " Min\n");
                sprintf((char *)&BufferText[14], "%2lu", pData->Hour);
                strcpy((char *)&BufferText[16], " Hour\n");
                sprintf((char *)&BufferText[22], "%2lu", pData->Day);
                strcpy((char *)&BufferText[24], " Day\n");
                sprintf((char *)&BufferText[29], "%2lu", pData->Date);
                strcpy((char *)&BufferText[31], " Date\n");
                sprintf((char *)&BufferText[37], "%2lu", pData->Month);
                strcpy((char *)&BufferText[39], " Month\n");
                sprintf((char *)&BufferText[46], "%2lu", pData->Year);
                strcpy((char *)&BufferText[48], " Year\n");
                sprintf((char *)&BufferText[54], "%2lu", pData->Century);
                strcpy((char *)&BufferText[56], " Century\n");

                memcpy(pDst, (char *)(((uint8_t *)BufferText) + pData->DataPointText), count);
                give_sem(xDataText_Sem);
                pData->DataPointText += count;
                return count;
            }
        }
		return 0;
}

int SeekClockText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{

        CLOCKData *pData = (CLOCKData *)(((DEVICE *)handle)->pData);
        switch(origin)
        {
            case 0:
                if(offset < MAX_CLOCK_DATA_TEXT_OFFSET)
                {
                    pData->DataPointText = offset;
                }
                else
                {
                    pData->DataPointText = 0;
                }
                break;
            case 1:
                if((pData->DataPointText + offset) < MAX_CLOCK_DATA_TEXT_OFFSET)
                {
                    pData->DataPointText = pData->DataPointText + offset;
                }
                else
                {
                    pData->DataPointText = 0;
                } 
                break;
            case 2:
                if(offset < MAX_CLOCK_DATA_TEXT_OFFSET)
                {
                    pData->DataPointText = MAX_CLOCK_DATA_TEXT_OFFSET - offset;
                }
                else
                {
                    pData->DataPointText = 0;
                } 
                break;
            default:
                pData->DataPointText = 0;
                return 0;                
        }
        return pData->DataPointText;
}

#endif //  _TEXT_

int WriteClockText(DEVICE_HANDLE handle, void * src, int count)
{ 
  return -1;
}

int ClockIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData)
{
    int err = 0;

    switch(code)
    {
#if _TIME_SYNC_GPS_
        case TIME_SYNC_GPS:
            err = time_synk_gps(handle);
            break;
#endif
#if _TIME_SYNC_GSM_
		case TIME_SYNC_GSM:
            err = time_synk_gsm(handle, pData);
            break;
#endif
#if _TL_TIME_STAMP_
		case TL_TIME_STAMP:
            *((uint64_t *)pData) = time_stamp(handle);
            break;
#endif
    }

    return err;	
}

int CreateClock(void)
{
 
    DEVICE_HANDLE hI2C;
    hI2C = OpenDevice(_CLOCK_L_L_DRIVER_NAME_);
    if(hI2C == NULL)
    {
      DEBUG_PUTS("CLOCK: unable to open I2C\n");
	  PUT_MES_TO_LOG("CLOCK: unable to open I2C", 0, 1);
      return DEVICE_ERROR;
    }


    CLOCK_INFO *pClockInfo = (CLOCK_INFO *)pvPortMalloc((size_t)sizeof(CLOCK_INFO));
	if(pClockInfo == NULL)
	{
		PUT_MES_TO_LOG("CLOCK: no memory", 0, 1);
		DEBUG_PUTS("CLOCK: no memory\n");
		return DEVICE_ERROR;
	}
	DEVICE     *pDevice = (DEVICE *)pvPortMalloc((size_t)sizeof(DEVICE));
	if(pDevice == NULL)
	{
		PUT_MES_TO_LOG("CLOCK: no memory", 0, 1);
		DEBUG_PUTS("CLOCK: no memory\n");
		return DEVICE_ERROR;
	}

    pClockInfo->pI2C = hI2C;
	pClockInfo->DataPoint = 0; 

	pDevice->device_io = (DEVICE_IO *)&clock_device_io;
	pDevice->pData = (void *)&clockDate;
	pDevice->pDriverData = pClockInfo;


    /* Проверяем часы и, если они не работают, запускаем */

	uint8_t addr = 0x0c;
	uint8_t ht;
	uint8_t temp[2];
	// Читаем регистр
	SeekDevice(pClockInfo->pI2C, /*addr*/0xff, ADR_CLOCK);
	WriteDevice(pClockInfo->pI2C, &addr, 1);
	ReadDevice(pClockInfo->pI2C, &ht, 1); 
	
	/* 
	Если бит 6 установлен, значит произошел сбой по питанию (переключился на батарейку).
	Это значит, что с момента переключения время не обновлялось.
	Нужно сбросить бит.
	*/
	if(ht & (1 << 6)) 
	{ 
	temp[0] = addr;
	temp[1] = 0;
	SeekDevice(pClockInfo->pI2C, /*addr*/0xff, ADR_CLOCK);
	WriteDevice(pClockInfo->pI2C, temp, 2);
	}
	
	addr = 0x01;
	uint8_t st;
	SeekDevice(pClockInfo->pI2C, /*addr*/0xff, ADR_CLOCK);
	WriteDevice(pClockInfo->pI2C, &addr, 1);
	ReadDevice(pClockInfo->pI2C, &st, 1);
	
	
	
	/* 
	Если бит 7 установлен, значит часы остановлены.
	Нужно сбросить бит.
	*/
	if(st & (1 << 7)) 
	{ 
	temp[0] = addr;
	temp[1] = 0;
	SeekDevice(pClockInfo->pI2C, /*addr*/0xff, ADR_CLOCK);
	WriteDevice(pClockInfo->pI2C, temp, 2);
	}
	
	// Включаем FREQ TEST OUTPUT
	addr = CONTROL_D;
	SeekDevice(pClockInfo->pI2C, 0xff, ADR_CLOCK);
	WriteDevice(pClockInfo->pI2C, &addr, 1);
	ReadDevice(pClockInfo->pI2C, &st, 1);
	
	/* 
	Нужно установить бит 6
	*/
	temp[0] = CONTROL_D;
	st |= (1 << 6);
	temp[1] = st;
	SeekDevice(pClockInfo->pI2C, /*CONTROL_D*/0xff, ADR_CLOCK);
	WriteDevice(pClockInfo->pI2C, &temp, 2);
	
	CloseDevice(pClockInfo->pI2C);


	vPortFree(pClockInfo);
	vPortFree(pDevice);
	//vUserDefrag();
	DEBUG_PUTS("CLOCK Created\n");

    return DEVICE_OK;	    
}


DEVICE_HANDLE OpenClock(void)
{
/*	DEVICE_HANDLE hI2C;
    hI2C = OpenDevice(_CLOCK_L_L_DRIVER_NAME_);
    if(hI2C == NULL)
    {
      DEBUG_PUTS("CLOCK: unable to open I2C\n");
	  PUT_MES_TO_LOG("CLOCK: unable to open I2C", 0, 1);
      return NULL;
    }	*/


    CLOCK_INFO *pClockInfo = (CLOCK_INFO *)pvPortMalloc((size_t)sizeof(CLOCK_INFO));
	if(pClockInfo == NULL)
	{
		PUT_MES_TO_LOG("CLOCK: no memory", 0, 1);
		DEBUG_PUTS("CLOCK: no memory\n");
		return NULL;
	}
	DEVICE     *pDevice = (DEVICE *)pvPortMalloc((size_t)sizeof(DEVICE));
	if(pDevice == NULL)
	{
		PUT_MES_TO_LOG("CLOCK: no memory", 0, 1);
		DEBUG_PUTS("CLOCK: no memory\n");
		return NULL;
	}

    //pClockInfo->pI2C = hI2C;
	pClockInfo->DataPoint = 0; 

	pDevice->device_io = (DEVICE_IO *)&clock_device_io;
	pDevice->pData = (void *)&clockDate;
	pDevice->pDriverData = pClockInfo;

	return (DEVICE_HANDLE)pDevice;
}

void CloseClock(DEVICE_HANDLE handle)
{
	CLOCK_INFO *pClockInfo = (CLOCK_INFO *)(((DEVICE *)handle)->pDriverData);

	//CloseDevice(pClockInfo->pI2C);

	vPortFree(pClockInfo);
	vPortFree(handle);
}



int clock_setup(DEVICE_HANDLE handle, struct tm AllTime)
{
	CLOCK_INFO *pClockInfo = (CLOCK_INFO *)(((DEVICE *)handle)->pDriverData);
	uint8_t temp[2];
    int err = 0;

	pClockInfo->pI2C = OpenDevice(_CLOCK_L_L_DRIVER_NAME_);
    if(pClockInfo->pI2C == NULL)
    {
      return -1;
    }

	
	SeekDevice(pClockInfo->pI2C, 0xFF, ADR_CLOCK);
	temp[0] = (uint8_t)SEC_D;
	temp[1] =  (D_TO_BCD(AllTime.tm_sec) &  ~(1 << 7));  // Bit7 по адресу SEC_D отвечает за остановку часов, поэтому его надо сбросить
	err = WriteDevice(pClockInfo->pI2C, temp, 2);
	if(err == -1)
	{
		CloseDevice(pClockInfo->pI2C);
		return -1;
	}  


	SeekDevice(pClockInfo->pI2C, 0xFF, ADR_CLOCK);
	temp[0] = (uint8_t)MIN_D;
	temp[1] =  (D_TO_BCD(AllTime.tm_min) &  ~(1 << 7));  // Bit7 по адресу MIN_D должен быть сброшен в 0 (по мануалу)
	err = WriteDevice(pClockInfo->pI2C, temp, 2);
	if(err == -1)
	{
		CloseDevice(pClockInfo->pI2C);
		return -1;
	}

	SeekDevice(pClockInfo->pI2C, 0xFF, ADR_CLOCK);
	temp[0] = (uint8_t)HOUR_D;
	temp[1] =  D_TO_BCD((AllTime.tm_hour) &  ~((1 << 7) | (1 << 6)));  // Bit7 и Bit6 по адресу HOUR_D отвечают за CENTURY BIT (не используются)
	err = WriteDevice(pClockInfo->pI2C, temp, 2);
	if(err == -1)
	{
		CloseDevice(pClockInfo->pI2C);
		return -1;
	}  


	SeekDevice(pClockInfo->pI2C, 0xFF, ADR_CLOCK);
	temp[0] = (uint8_t)DATE_D;
	temp[1] =  (D_TO_BCD(AllTime.tm_mday) &  ~((1 << 7) | (1 << 6)));   // Bit7 и Bit6 по адресу DATE_D должены быть сброшены в 0 (по мануалу)
	err = WriteDevice(pClockInfo->pI2C, temp, 2);
	if(err == -1)
	{
		CloseDevice(pClockInfo->pI2C);
		return -1;
	}

	SeekDevice(pClockInfo->pI2C, 0xFF, ADR_CLOCK);
	temp[0] = (uint8_t)MONTH_D;
	temp[1] =  (D_TO_BCD(AllTime.tm_mon) &  ~((1 << 7) | (1 << 6) | (1 << 5)));   // Bit7-Bit5 по адресу MONTH_D должены быть сброшены в 0 (по мануалу)
	err = WriteDevice(pClockInfo->pI2C, temp, 2);
	if(err == -1)
	{
		CloseDevice(pClockInfo->pI2C);
		return -1;
	}

	SeekDevice(pClockInfo->pI2C, 0xFF, ADR_CLOCK);
	temp[0] = (uint8_t)YEAR_D;
	temp[1] =   D_TO_BCD(AllTime.tm_year);   // Bit7-Bit5 по адресу MONTH_D должены быть сброшены в 0 (по мануалу)
	err = WriteDevice(pClockInfo->pI2C, temp, 2);
	if(err == -1)
	{
		CloseDevice(pClockInfo->pI2C);
		return -1;
	} 

	CloseDevice(pClockInfo->pI2C);

	PoolClock();

	return 0;
}


#if _TIME_SYNC_GSM_
	 int time_synk_gsm(DEVICE_HANDLE handle, uint8_t *buf)
	 {
	 	 struct tm AllTime;
		 int i = 0, k = 0, len, err = 0, delta = 0;

		 len = strlen((char *)buf);

		 while(i != len)
		 {
		 	 if(buf[i] == '/' || buf[i] == ',' || buf[i] == ':' || buf[i] == '+' || buf[i] == '-' || buf[i] == '\"')
			 	buf[i] = 0;

		 	i++;
		 }

		 i = 0;
		 k = atoi((char *)(buf + i));
		 AllTime.tm_year = k;

		 i += 3;
		 k = atoi((char *)(buf + i));
		 AllTime.tm_mon = k;

		 i += 3;
		 k = atoi((char *)(buf + i));
		 AllTime.tm_mday = k;

		 i += 3;
		 k = atoi((char *)(buf + i));
		 AllTime.tm_hour = k + clockDate.Hour_delta;

		 i += 3;
		 k = atoi((char *)(buf + i));
		 AllTime.tm_min = k;

		 i += 3;
		 k = atoi((char *)(buf + i));
		 AllTime.tm_sec = k;

		 i += 3;
		 k = atoi((char *)(buf + i));
		 delta = k / 4;

		 AllTime.tm_hour += delta;
		 if(AllTime.tm_hour >= 24)
		 {
		 	AllTime.tm_hour -= delta;
			return -1;
		 }
		 	

		 err = clock_setup(handle, AllTime);
		return err;
	 }
#endif

#if _TIME_SYNC_GPS_
int time_synk_gps(DEVICE_HANDLE handle)
{
	struct tm AllTime;
    int err = 0;
	uint64_t time = 0;

	DEVICE_HANDLE hGPS = OpenDevice("GPS");    //   Открываем GPS
	if(hGPS == NULL) return -1;

	err = SeekDevice(hGPS, TIME, 0);
	if(err == -1)
	{
		CloseDevice(hGPS);
		return -1;
	}
   	err = ReadDevice(hGPS, (uint8_t *)&time, TIME_SIZE);	 //   Читаем время с 1970 года в секундах
	if(err == -1)
	{
		CloseDevice(hGPS);
		return -1;
	}

	if(time == 0)	             //  Если время по GPS не определено - выходим
	{
		CloseDevice(hGPS);
		return -1;
	}

    err = SeekDevice(hGPS, ALLTIME, 0);		 
	if(err == -1)
	{
		CloseDevice(hGPS);
		return -1;
	}
    err = ReadDevice(hGPS, (uint8_t *)&AllTime, sizeof(AllTime));	 //   Читаем все данные о времени и дате
	if(err == -1)
	{
		CloseDevice(hGPS);
		return -1;
	}

   	CloseDevice(hGPS);

	//AllTime.tm_hour += clockDate.Hour_delta;
	//AllTime.tm_year -= 100;
	AllTime.tm_mon += 1;

    err = clock_setup(handle, AllTime);

	return err;

}

#endif // _TIME_SYNC_GPS_


#if _TIME_SYNC_GSM_
	extern xQueueHandle modem_queue;
	#include "ModemTask.h"
#endif

int PoolClock(void)
{

	DEVICE_HANDLE hI2C;
    hI2C = OpenDevice(_CLOCK_L_L_DRIVER_NAME_);
    if(hI2C == NULL)
    {
      DEBUG_PUTS("CLOCK: unable to open I2C\n");
	  PUT_MES_TO_LOG("CLOCK: unable to open I2C", 0, 1);
      return 0;
    }	

        CLOCKData *pData = (CLOCKData *)&clockDate;

        uint8_t temp;

        SeekDevice(hI2C, 0xFF, ADR_CLOCK);
        temp = SEC_D;
        if(WriteDevice(hI2C, &temp, 1) == -1) return -1;
        if(ReadDevice(hI2C, &pData->Sec, 7) == -1) return -1;

        pData->Sec &= ~(1 << 7);
        pData->Sec = BCD_TO_D(pData->Sec);

        pData->Min &= ~(1 << 7);
        pData->Min = BCD_TO_D(pData->Min);

        pData->Hour &= ~((1 << 7) | (1 << 6));
        pData->Hour = BCD_TO_D(pData->Hour);

        pData->Day &= ~((1 << 7) | (1 << 6) | (1 << 5) | (1 << 4) | (1 << 3));
        pData->Day = BCD_TO_D(pData->Day);

        pData->Date &= ~((1 << 7) | (1 << 6));
        pData->Date = BCD_TO_D(pData->Date);

        pData->Month &= ~((1 << 7) | (1 << 6) | (1 << 5));
        pData->Month = BCD_TO_D(pData->Month);

        pData->Year = BCD_TO_D(pData->Year);  

		CloseDevice(hI2C);
#if _TIME_SYNC_GSM_
		if(clockDate.reset_flag == 1)
		{
			if(clockDate.Hour == clockDate.Res_Hour)
			{
				if(clockDate.Min == clockDate.Res_Min)
				{
					MODEM_MESSAGE message;
					message.ID = 1;
					xQueueSendToBack(modem_queue, ( void * )&message, 10); 	
				}
			}	
		}
#endif
        
        return 0;
}




uint64_t time_stamp(DEVICE_HANDLE handle)
{
	struct tm AllTime;

	AllTime.tm_year = clockDate.Year + 100;
	AllTime.tm_mon = clockDate.Month - 1;
	AllTime.tm_mday = clockDate.Date;
	AllTime.tm_hour = clockDate.Hour;
	AllTime.tm_min = clockDate.Min;
	AllTime.tm_sec = clockDate.Sec;

	return ((uint64_t)mktime(&AllTime) * 1000);
}



void Init_clock(void)
{
	uint8_t *p_data;

    
    if(GetSettingsFileStatus())
    {
		p_data = (uint8_t *)GetPointToElement("INFO", "Hour_D");
        if(p_data == 0)
             clockDate.Hour_delta = 0;
		else clockDate.Hour_delta = atoi((char *)p_data);

		p_data = (uint8_t *)GetPointToElement("INFO", "Daily_r");
        if(p_data == 0)
			clockDate.reset_flag = 1;
		else clockDate.reset_flag = atoi((char *)p_data);

		p_data = (uint8_t *)GetPointToElement("INFO", "R_hour");
        if(p_data == 0)
			clockDate.Res_Hour = 22;
		else clockDate.Res_Hour = atoi((char *)p_data);

		p_data = (uint8_t *)GetPointToElement("INFO", "R_min");
        if(p_data == 0)
			clockDate.Res_Min = 0;
		else clockDate.Res_Min = atoi((char *)p_data);
	}
}

#endif  // _CLOCK_
