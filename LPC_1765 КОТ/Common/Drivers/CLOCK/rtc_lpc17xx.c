#include "clock_drv.h"
#if _CLOCK_RTC_
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "lpc17xx_rtc.h"
#include "lpc17xx.h"
#include "SetParametrs.h"
#include "User_Error.h"
											  
static int CreateClock(void);
static DEVICE_HANDLE OpenClock(void);
static void CloseClock(DEVICE_HANDLE handle);
static int ReadClock(DEVICE_HANDLE handle, void * dst, int count);
static int WriteClock(DEVICE_HANDLE handle, const void * src, int count);
static int SeekClock(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
#if _TEXT_
    static int ReadClockText(DEVICE_HANDLE handle, void * dst, int count);
    static int WriteClockText(DEVICE_HANDLE handle, void * src, int count);
    static int SeekClockText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
#endif // _TEXT_

static int ClockIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData);
int PoolClock(void);


#if (_TIME_SYNC_GSM_)
 	int time_synk_gsm(DEVICE_HANDLE handle, uint8_t *buf);
#endif

#if (_TIME_SYNC_GPS_)
	#include "GPS_DRV.h"
	int time_synk_gps(DEVICE_HANDLE handle);
#endif

#if (_DAILY_RESET_)
	void AwakeResetTask(int isr_fl, char *data);
#endif

uint64_t time_stamp(DEVICE_HANDLE handle);



CLOCKData  clockDate = {
    .Sec = NULL,
    .Min = NULL,
    .Hour = NULL,
		.DOW = NULL,
    .Day = NULL,
    .Date = NULL,
    .Month = NULL,
    .Year = NULL,
    .DataPointText = NULL,
		.Hour_delta = 0,
		.Res_Hour = 20,
		.Res_Min = 0,
		.reset_flag = 1
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

int ReadClock(DEVICE_HANDLE handle, void * dst, int count)
{
    CLOCK_INFO *pClockInfo = (CLOCK_INFO *)(((DEVICE *)handle)->pDriverData);
	CLOCKData *pData = (CLOCKData *)(((DEVICE *)handle)->pData);

        if(count > 0) 
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

int WriteClock(DEVICE_HANDLE handle, const void * src, int count)
{
    CLOCK_INFO *pClockInfo = (CLOCK_INFO *)(((DEVICE *)handle)->pDriverData);

	if((pClockInfo->DataPoint + count) > MAX_CLOCK_DATA_OFFSET)
		return 0;

	int i = 0;
	for(; i < count; i++)
	{
		uint32_t temp = *((uint8_t *)src + i);

		switch(pClockInfo->DataPoint)
		{
			case SEC_D:
     			if(temp > RTC_SECOND_MAX)
					temp = 0;
				break;

			case MIN_D:
				if(temp > RTC_MINUTE_MAX)
					temp = 0;
				break;				

			case HOUR_D:
				if(temp > RTC_HOUR_MAX)
					temp = 0;
				break;

			case DOW_D:
				if(temp > RTC_DAYOFWEEK_MAX)
					temp = 0;
				break;				

			case DATE_D:
				if(temp > RTC_DAYOFMONTH_MAX)
					temp = 1;
				break;

			case DAY_D:
				if(temp > RTC_DAYOFYEAR_MAX)
					temp = 1;
				break;

			case MONTH_D:
				if(temp > RTC_MONTH_MAX)
					temp = 1;
				break;

			case YEAR_D: 
				temp += 2000; 
				break;

			default :
				return 0;
				
		}
			
		RTC_SetTime(LPC_RTC, pClockInfo->DataPoint++, temp);	
	}

	return count;
}

#if(_TEXT_)
int ReadClockText(DEVICE_HANDLE handle, void * pDst, int count)
{
    CLOCKData *pData = (CLOCKData *)(((DEVICE *)handle)->pData);
	int sprintf_count;
    char *BufferText;
	
	if(count >= 0)
	{
		memset(pDst, 0, count);
		BufferText = pvPortMalloc(300);
		if(BufferText != NULL)
        { 
            sprintf_count = sprintf(BufferText, "Date - %2u.%2u.%2u %2u:%2u:%2u\n",pData->Date, pData->Month, pData->Year, pData->Hour, pData->Min, pData->Sec);
            sprintf_count += sprintf(BufferText + sprintf_count, "Day - %2u\n", pData->Day);
            sprintf_count += sprintf(BufferText + sprintf_count, "Century - %2u\n", pData->Century);

            if((pData->DataPointText + count) > sprintf_count)
			{
				count = sprintf_count - pData->DataPointText;
			}
			memcpy(pDst, (char *)(((uint8_t *)BufferText) + pData->DataPointText), count);
			pData->DataPointText += count;
			vPortFree(BufferText);
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
	uint32_t val = *(uint32_t *)pData;
    int err = -1;
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
        case POOL:
            err = PoolClock();
            break;

		case SET_CALIB_DIR:
			if(val)
				LPC_RTC->CALIBRATION |= (1 << 17);
			else
				LPC_RTC->CALIBRATION &= ~(1 << 17);

			err = 0;
			break;

		case SET_CALIB_VAL:
			LPC_RTC->CALIBRATION &=	~(0x0001ffff);
			LPC_RTC->CALIBRATION |=	0x0001ffff & val;

			err = 0;
			break;

		case GET_CALIB_DIR:
			if(LPC_RTC->CALIBRATION & (1 << 17))
				*(uint32_t *)pData = 1;
			else
				*(uint32_t *)pData = 0;

			err = 0;
			break;

		case GET_CALIB_VAL:
			*(uint32_t *)pData = (LPC_RTC->CALIBRATION & 0x0001ffff);
		/*	if(*(uint32_t *)pData != 0)
				*(uint32_t *)pData++;*/

			err = 0;
			break;
		
		case SET_GPREG0:
			RTC_WriteGPREG(LPC_RTC, 0, *(uint32_t *)pData);
			err = 0;
			break;

		case GET_GPREG0:
			*(uint32_t *)pData = RTC_ReadGPREG(LPC_RTC, 0);
			err = 0;
			break;

		case SET_GPREG1:
			RTC_WriteGPREG(LPC_RTC, 1, *(uint32_t *)pData);
			err = 0;
			break;

		case GET_GPREG1:
			*(uint32_t *)pData = RTC_ReadGPREG(LPC_RTC, 1);
			err = 0;
			break;
    }

    return err;	
}

int CreateClock(void)
{
	// Init RTC module
//#if(INIT_ON)
	RTC_Init(LPC_RTC);

	/* Enable rtc (starts increase the tick counter and second counter register) */
	RTC_ResetClockTickCounter(LPC_RTC);
	RTC_Cmd(LPC_RTC, ENABLE);

#if(USE_RTC_CALIB)
	uint32_t val;
	val = RTC_ReadGPREG(LPC_RTC, 0);
	if(val != 0x12344321)
	{
		RTC_CalibConfig(LPC_RTC, 1, RTC_CALIB_DIR_FORWARD);	
		RTC_WriteGPREG(LPC_RTC, 0, 0x12344321);
	}	

	RTC_CalibCounterCmd(LPC_RTC, ENABLE);
#else
	RTC_CalibCounterCmd(LPC_RTC, DISABLE);	
#endif	 // USE_RTC_CALIB
//#endif	//	INIT_ON
	PoolClock();
    return DEVICE_OK;	    
}


DEVICE_HANDLE OpenClock(void)
{
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

	pClockInfo->DataPoint = 0; 

	pDevice->device_io = (DEVICE_IO *)&clock_device_io;
	pDevice->pData = &clockDate;
	pDevice->pDriverData = pClockInfo;

	return (DEVICE_HANDLE)pDevice;
}

void CloseClock(DEVICE_HANDLE handle)
{
	CLOCK_INFO *pClockInfo = (CLOCK_INFO *)(((DEVICE *)handle)->pDriverData);

	vPortFree(pClockInfo);
	vPortFree(handle);
}

void SaveClockDate(void)
{
	uint32_t data_1 = 0, data_2 = 0;
	data_1 = (1 << 24) | (clockDate.Hour << 16) | (clockDate.Min << 8) | (clockDate.Sec);
	data_2 = (1 << 24) | (clockDate.Date << 16) | (clockDate.Month << 8) | (clockDate.Year);
	RTC_WriteGPREG(LPC_RTC, 0, data_1);
	RTC_WriteGPREG(LPC_RTC, 1, data_2);
}

#if(_DAILY_RESET_)
extern xTaskHandle BODTaskHandle;
#endif

int PoolClock(void)
{
	clockDate.Sec = (uint8_t)RTC_GetTime(LPC_RTC, RTC_TIMETYPE_SECOND);
	clockDate.Min = (uint8_t)RTC_GetTime(LPC_RTC, RTC_TIMETYPE_MINUTE);
	clockDate.Hour = (uint8_t)RTC_GetTime(LPC_RTC, RTC_TIMETYPE_HOUR);
	clockDate.DOW = (uint8_t)RTC_GetTime(LPC_RTC, RTC_TIMETYPE_DAYOFWEEK);
	clockDate.Date = (uint8_t)RTC_GetTime(LPC_RTC, RTC_TIMETYPE_DAYOFMONTH);
	clockDate.Day = (uint8_t)RTC_GetTime(LPC_RTC, RTC_TIMETYPE_DAYOFYEAR);
	clockDate.Month = (uint8_t)RTC_GetTime(LPC_RTC, RTC_TIMETYPE_MONTH);
	uint32_t year = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_YEAR);
	if(year > 2255)
		clockDate.Year = 255;
	else
		clockDate.Year = year - 2000;
		
#if(_DAILY_RESET_)

	if(clockDate.reset_flag == 1)
		if(clockDate.Hour == clockDate.Res_Hour)
			if(clockDate.Min == clockDate.Res_Min)
				AwakeResetTask(0, "DAILY_RESET");
	
#endif 	
    
    return 0;
}


int clock_setup(DEVICE_HANDLE handle, struct tm AllTime)
{

	if(AllTime.tm_sec > RTC_SECOND_MAX)
		AllTime.tm_sec = 0;
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_SECOND, AllTime.tm_sec);

	if(AllTime.tm_min > RTC_MINUTE_MAX)
		AllTime.tm_min = 0;
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MINUTE, AllTime.tm_min);

	if(AllTime.tm_hour > RTC_HOUR_MAX)
		AllTime.tm_hour = 0;
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_HOUR, AllTime.tm_hour);

	if(AllTime.tm_mday > RTC_DAYOFMONTH_MAX || AllTime.tm_mday < RTC_DAYOFMONTH_MIN)
		AllTime.tm_mday = RTC_DAYOFMONTH_MIN;
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, AllTime.tm_mday);
    
	if(AllTime.tm_mon > RTC_MONTH_MAX || AllTime.tm_mon < RTC_MONTH_MIN)
		AllTime.tm_mon = RTC_MONTH_MIN;
	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_MONTH, AllTime.tm_mon);


	RTC_SetTime(LPC_RTC, RTC_TIMETYPE_YEAR, AllTime.tm_year + 1900);

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
		 AllTime.tm_year = k + (2000 - 1900);  // AllTime задает в ремя с 1900г.

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


	AllTime.tm_hour += clockDate.Hour_delta;
	if(AllTime.tm_hour >= 24)
	{
		AllTime.tm_hour -= clockDate.Hour_delta;
		return -1;
	}

	AllTime.tm_mon += 1;
	AllTime.tm_year += 100;

    err = clock_setup(handle, AllTime);

	return err;

}

#endif

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
		p_data = (uint8_t *)GetPointToElement("INFO", "Daily_r");
        if(p_data == 0)
			clockDate.reset_flag = 1;
		else clockDate.reset_flag = atoi((char *)p_data);

		p_data = (uint8_t *)GetPointToElement("INFO", "R_hour");
        if(p_data == 0)
			clockDate.Res_Hour = 20;
		else clockDate.Res_Hour = atoi((char *)p_data);

		p_data = (uint8_t *)GetPointToElement("INFO", "R_min");
        if(p_data == 0)
			clockDate.Res_Min = 0;
		else clockDate.Res_Min = atoi((char *)p_data);
			
	}
}

#endif  // _CLOCK_
