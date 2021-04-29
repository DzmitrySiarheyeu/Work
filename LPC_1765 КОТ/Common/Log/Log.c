#include "Config.h"
#if(_LOG_)
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "Log.h"
#include "DriversCore.h"
#include "VFS.h"
#include "CLOCK_DRV.h"
#include "SetParametrs.h"
#include "User_Error.h"

#include "FreeRTOS.h"
#include "semphr.h"

xSemaphoreHandle logSem;

//#if (!_CLOCK_)
//  #error "If you want to use LOG, you have to define _CLOCK_= 1 in your Config.h"
//#endif

s_LogInfo Log;
s_LogInfo *pLog = &Log;
int clock_data_form(void);

static char recordBuf[LOG_RECORD_LEN];

static int checkCurrentPos(uint16_t oldestPos, uint16_t newestPos, uint16_t currentPos)
{

	if(currentPos > newestPos && currentPos < oldestPos)
		return 1;

	return 0;
}

int AddLogMessage(uint8_t *message, uint8_t type, uint8_t level)
{	
	if(type == COMMON_MES)
    {
        if(pLog->com_mes == 0 || level > pLog->com_mes_level) return -1;
    }
    else if(type == MODEM_MES)
    {
        if(pLog->modem_mes == 0 || level > pLog->modem_mes_level) return -1;
    }
    else if(type == FTP_MES)
    {
        if(pLog->ftp_mes == 0 || level > pLog->ftp_mes_level) return -1;
    }
    else return -1;

	if(take_sem(logSem, 1000) == pdTRUE)
	{
		DEVICE_HANDLE hSaver = OpenDevice("SAVER");
	
		LOG_COUNTERS logCounters;
		SeekDevice(hSaver, 0, LOG_COUNTERS_SEG);
		int res = ReadDevice(hSaver, &logCounters, 1);
		if(res == 0)
		{
			logCounters.numberOfRecords = 0;
			logCounters.oldestPos = 0;
			logCounters.newestPos = 0xffff;
			logCounters.currentPos = 0;	
		}
	
		if(logCounters.numberOfRecords < LOG_MAX_NUMBER_OF_RECORDS)
			logCounters.numberOfRecords++;	
	
		logCounters.newestPos++; // позиция записи 
		//if(logCounters.numberOfRecords >= ARCH_MAX_RECORDS)
		//	logCounters.oldestPos++;	
	
		// если вышли за границы
		if(logCounters.newestPos >= LOG_MAX_NUMBER_OF_RECORDS)
			// переходим в начало
			logCounters.newestPos = 0;
			
		if(logCounters.newestPos == logCounters.oldestPos)
			logCounters.oldestPos += (_SAVER_FLASH_PAGE_SIZE_ / LOG_RECORD_LEN);		
	
		if(logCounters.oldestPos >= LOG_MAX_NUMBER_OF_RECORDS)
			logCounters.oldestPos = 0;
	
		// если отображаемая позиция попала в пустой сектор, ее тоже надо передвинуть
		if(checkCurrentPos(logCounters.oldestPos, logCounters.newestPos, logCounters.currentPos))
			logCounters.currentPos = logCounters.newestPos;
	
		SeekDevice(hSaver, 0, LOG_COUNTERS_SEG);
		WriteDevice(hSaver, &logCounters, 1);
	
	    clock_data_form();               //  Формируем строку данных о времени
		memset(recordBuf, 0, LOG_RECORD_LEN);
		memcpy(recordBuf, pLog->DateBuf, CLOCK_DATA_LENGTH);
		int len = strlen((char *)message);
		if(len > (LOG_RECORD_LEN - CLOCK_DATA_LENGTH)) len = (LOG_RECORD_LEN - CLOCK_DATA_LENGTH);
		memcpy(&recordBuf[CLOCK_DATA_LENGTH], (char *)message, len);
	
		SeekDevice(hSaver, logCounters.newestPos * LOG_RECORD_LEN, LOG_SEG);
		WriteDevice(hSaver, (uint8_t *)recordBuf, 1);
	
		CloseDevice(hSaver);

		give_sem(logSem);
	}

    return LOG_RECORD_LEN;
}


int clock_data_form(void)
{
    uint8_t temp;
	DEVICE_HANDLE hClock;
	
	hClock = OpenDevice("Clock"); 
    
    pLog->DateBuf[0] = '\r';

    SeekDevice(hClock, HOUR_D, 0);
    ReadDevice(hClock, (uint8_t *)&temp, 1);
    sprintf((char *)&(pLog->DateBuf[1]), "%02d", temp);

    pLog->DateBuf[3] = ':';

    SeekDevice(hClock, MIN_D, 0);
    ReadDevice(hClock, (uint8_t *)&temp, 1);
    sprintf((char *)&(pLog->DateBuf[4]), "%02d", temp);

    pLog->DateBuf[6] = ' ';

    SeekDevice(hClock, DATE_D, 0);
    ReadDevice(hClock, (uint8_t *)&temp, 1);
    sprintf((char *)&(pLog->DateBuf[7]), "%02d", temp);

    pLog->DateBuf[9] = '.';

    SeekDevice(hClock, MONTH_D, 0);
    ReadDevice(hClock, (uint8_t *)&temp, 1);
    sprintf((char *)&(pLog->DateBuf[10]), "%02d", temp);

    pLog->DateBuf[12] = '.';

    SeekDevice(hClock, YEAR_D, 0);
    ReadDevice(hClock, (uint8_t *)&temp, 1);
    sprintf((char *)&(pLog->DateBuf[13]), "%02d", temp);

	CloseDevice(hClock);

   pLog->DateBuf[15] = '\r';

   return 0;
}

void GetLogRecord(char *buf)
{
	DEVICE_HANDLE hSaver = OpenDevice("SAVER");

	LOG_COUNTERS logCounters;
	SeekDevice(hSaver, 0, LOG_COUNTERS_SEG);
	ReadDevice(hSaver, &logCounters, 1);

	if(logCounters.numberOfRecords == 0)
	{
		buf = 0;
		return;	
	}

	SeekDevice(hSaver, logCounters.currentPos * LOG_RECORD_LEN, LOG_SEG);
	ReadDevice(hSaver, (uint8_t *)buf, 1);

	if(logCounters.currentPos == 0)
	{
		if(logCounters.oldestPos < logCounters.newestPos)
			logCounters.currentPos = logCounters.newestPos;
		else logCounters.currentPos = LOG_MAX_NUMBER_OF_RECORDS - 1;		
	}
	else
		logCounters.currentPos--;

	if(logCounters.currentPos > logCounters.numberOfRecords)
		logCounters.currentPos = logCounters.newestPos;

	// если отображаемая позиция попала в пустой сектор, ее надо передвинуть
	if(checkCurrentPos(logCounters.oldestPos, logCounters.newestPos, logCounters.currentPos))
		logCounters.currentPos = logCounters.newestPos;

	SeekDevice(hSaver, 0, LOG_COUNTERS_SEG);
	WriteDevice(hSaver, &logCounters, 1);

	CloseDevice(hSaver);
}

void GoToTheNewestRecord(void)
{
	DEVICE_HANDLE hSaver = OpenDevice("SAVER");

	LOG_COUNTERS logCounters;
	SeekDevice(hSaver, 0, LOG_COUNTERS_SEG);
	ReadDevice(hSaver, &logCounters, 1);
	
	logCounters.currentPos = logCounters.newestPos;
	
	SeekDevice(hSaver, 0, LOG_COUNTERS_SEG);
	WriteDevice(hSaver, &logCounters, 1);
	
	CloseDevice(hSaver);	
}

void GetArchInfo(uint16_t *numberOfRecords, uint16_t *currentNumber)
{
	DEVICE_HANDLE hSaver = OpenDevice("SAVER");

	LOG_COUNTERS logCounters;
	SeekDevice(hSaver, 0, LOG_COUNTERS_SEG);
	ReadDevice(hSaver, &logCounters, 1);
	
	CloseDevice(hSaver);
	
	*numberOfRecords = logCounters.numberOfRecords;
	*currentNumber = logCounters.currentPos;	
}

int InitLog(void)
{ 
	uint8_t *p_data = NULL;

	vSemaphoreCreateBinary(logSem);

    if(GetSettingsFileStatus())
    {
        p_data = (uint8_t *)GetPointToElement("LOG", "COM");
        if(p_data == 0 || strlen((char *)p_data) > 1)
        {
             pLog->com_mes = 1;
			 pLog->com_mes_level = 0;
        }
        else
        {
            pLog->com_mes = 1;
            pLog->com_mes_level = (uint8_t)atoi((char *)p_data);
        }

        p_data = (uint8_t *)GetPointToElement("LOG", "MODEM");
        if(p_data == 0 || strlen((char *)p_data) > 1)
        {
             pLog->modem_mes = 1;
			 pLog->modem_mes_level = 0;
        }
        else
        {
            pLog->modem_mes = 1;
            pLog->modem_mes_level = (uint8_t)atoi((char *)p_data);
        }

        p_data = (uint8_t *)GetPointToElement("LOG", "FTP");
        if(p_data == 0 || strlen((char *)p_data) > 1)
        {
             pLog->ftp_mes = 1;
			 pLog->ftp_mes_level = 0;
        }
        else
        {
            pLog->ftp_mes = 1;
            pLog->ftp_mes_level = (uint8_t)atoi((char *)p_data);
        }     
    }
    else
	{
		pLog->com_mes = 1;
		pLog->com_mes_level = 1;
		pLog->modem_mes = 1;
		pLog->modem_mes_level = 0;
		pLog->ftp_mes = 1;
		pLog->ftp_mes_level = 0;   	
		AddError(NULL);
	}
	return 0;
}

#endif
