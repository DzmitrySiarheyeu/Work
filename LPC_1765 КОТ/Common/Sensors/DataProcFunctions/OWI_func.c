#include "Config.h"
#if _SENSORS_ && _OWI_

#include <stdint.h>
#include "SensorCore.h"
//#include "discrete/discrete.h"
//#include "Freq/Freq_lm.h"
#include "DriversCore.h"
#include "DataFunc.h"
#include "main.h"
#include "User_Error.h"
#include "Events.h"
#include "OWI_func.h"
#include "OWI_DRV.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


static OWI_SENS_INFO OWISInfo = {{NULL, NULL, NULL, NULL, NULL}, 0, 0, 0};



static uint8_t char_to_hex(char ch)
{
    if(0x30 <= ch && ch <= 0x39) return (ch - 0x30);
    else if(0x41 <= ch && ch <= 0x46) return (ch - 0x37);
    else if(0x61 <= ch && ch <= 0x66) return (ch - 0x57);
    else return 0;
}



static int str_to_hex(char *str, uint8_t *buf, int count)
{
    int i = 0;
    while(str[i] != 0 && i < count)
    {
        buf[i] = (char_to_hex(str[i*2]) << 4)  | char_to_hex(str[i*2 + 1]);
        i++;
    }
    return i;
}


uint8_t ComputeCRC8(uint8_t inData, uint8_t seed);


static int check_rom_crc(uint8_t *romValue, int count)
{
    unsigned char i;
    unsigned char crc8 = 0;
	  unsigned char fl_no_zero = 0;
    
    for (i = 0; i < count; i++)
    {
        crc8 = ComputeCRC8(*romValue, crc8);
			  if(fl_no_zero == 0)	// Проверяем что все данные не равны нулю (на КЗ линии)
					fl_no_zero = *romValue;
        romValue++;
    }
    if (crc8 == (*romValue) && fl_no_zero)
    {
        return 0;
    }
    return -1;
}

/*! brief  Compute the CRC16 value of a data set.
 *
 *  This function will compute the CRC16 of inData using seed
 *  as inital value for the CRC.
 *
 *  param  inData  One byte of data to compute CRC from.
 *
 * param  seed    The starting value of the CRC.
 *
 *  return The CRC16 of inData with seed as initial value.
 *
 *  note   Setting seed to 0 computes the crc16 of the inData.
 *
 *  note   Constantly passing the return value of this function 
 *          As the seed argument computes the CRC16 value of a
 *          longer string of data.
 */

 
uint16_t ComputeCRC16(uint8_t inData, uint16_t seed)
{
    uint8_t bitsLeft;
    uint8_t temp;

    for (bitsLeft = 8; bitsLeft > 0; bitsLeft--)
    {
        temp = ((seed ^ inData) & 0x01);
        if (temp == 0)
        {
            seed >>= 1;
        }
        else
        {
            seed ^= 0x4002;
            seed >>= 1;
            seed |= 0x8000;
        }
        inData >>= 1;
    }
    return seed;    
}

static int check_data_crc16(uint8_t *romValue, int count)
{
    unsigned char i;
    unsigned int crc16_in, crc16 = 0;
	unsigned char fl_no_zero = 0;
    
    for (i = 0; i < count; i++)
    {
        crc16 = ComputeCRC16(*romValue, crc16);
			  if(fl_no_zero == 0)	// Проверяем что все данные не равны нулю (на КЗ линии)
					fl_no_zero = *romValue;
        romValue++;
    }
	memcpy(&crc16_in, romValue, 2);
	crc16_in = crc16_in ^ 0xFFFF;
    if (crc16 == crc16_in && fl_no_zero)
    {
        return 0;
    }
    return -1;
}


#if _TEMPER_FUNC_
    static float TemperFunc(SENSOR_HANDLE handle);
    static int InitTemperFunc(SENSOR_HANDLE handle, uint8_t *buf, uint16_t count);
#endif


#if _TEMPER_FUNC_
DATA_FUNC Temper = {.Addr = (SENSOR_PROC_FUNC *)&TemperFunc, .Init_Proc = (INIT_PROC *)&InitTemperFunc, .Name = "Temper", .pNext = NULL, .pFuncList = NULL};


int ReadTemperature(SENSOR *pSensor, SENSOR_INFO *pSensInfo, PROC_INFO *pProcInfo)
{
	int16_t data[5]; //Должны иметь возможность считать 9 байт
	uint8_t command;
	uint8_t *family;
	float temper = 0;
	
	DeviceIOCtl(pSensor->SensorDevise, OWI_MATCH_ROM, (uint8_t *)(pProcInfo->Data));   //  захват устройства по его ID

  command = 0xBE;   // Команда для чтения всей памяти устройства
  if(WriteDevice(pSensor->SensorDevise, &command, 1) == -1)
	{
		CloseDevice(pSensor->SensorDevise);
	 	return -1;
	}
  if(ReadDevice(pSensor->SensorDevise, (uint8_t *)data, 9) == -1)
	{
		CloseDevice(pSensor->SensorDevise);
	 	return -1;
	}
  if(check_rom_crc((uint8_t *)data, 8) == -1)
	{
		CloseDevice(pSensor->SensorDevise);
	 	return -1;
	}

	CloseDevice(pSensor->SensorDevise);

	//  Приобразуем считанные данные в измеренную температуру.
	
	temper = (float)data[0];
	family = (uint8_t *)(pProcInfo->Data);
	if(*family == 0x10) // если у нас чип 18S20
	{
		temper = temper/2;
	}
	else	// иначе у нас чип 18B20
	{
		temper = temper/16;
	}

	if(temper != 85)  // 85 - значение температуры при включении датчика
	{
		pSensInfo->Data = temper;
		pSensInfo->PhysValue = PhysProcSensor(pSensor);
		pProcInfo->Flag = 0; //сбрасываем счетчик ошибок
	}
	return 0;
}

int ReadDS2408(SENSOR *pSensor, SENSOR_INFO *pSensInfo, PROC_INFO *pProcInfo)
{
	uint8_t command[ 3 + 10 ] = { 0xF0, 0x88, 0x00 };
	float temper = 0;
	
	DeviceIOCtl(pSensor->SensorDevise, OWI_MATCH_ROM, (uint8_t *)(pProcInfo->Data));   //  захват устройства по его ID

	if(WriteDevice(pSensor->SensorDevise, command, 3) == -1)
	{
		CloseDevice(pSensor->SensorDevise);
	 	return -1;
	}
	if(ReadDevice(pSensor->SensorDevise, command + 3, 10) == -1)
	{
		CloseDevice(pSensor->SensorDevise);
	 	return -1;
	}
	if(check_data_crc16(command, 8 + 3) == -1)
	{
		CloseDevice(pSensor->SensorDevise);
	 	return -1;
	}

	CloseDevice(pSensor->SensorDevise);

	//  Приобразуем считанные данные в измеренную температуру.
	
	temper = (float)(command[3] ^ 0xFF);
	pSensInfo->Data = temper;
	pSensInfo->PhysValue = PhysProcSensor(pSensor);
	pProcInfo->Flag = 0; //сбрасываем счетчик ошибок
	
	return 0;
}

float TemperFunc(SENSOR_HANDLE handle)
{
	SENSOR *pSensor;			   
	SENSOR_INFO *pSensInfo;
	PROC_INFO *pProcInfo;
	uint8_t command;
	uint8_t *family;
	float err = 0;
	SET_SNaN(&err);

	if(OWISInfo.pass_count < OWISInfo.pass_lim)	   //  Если время пулится еще не пришло - выходим
	{
		OWISInfo.pass_count++;
		return err;
	}

	pSensor = OWISInfo.pSInfo[OWISInfo.cur_sen];
	pSensInfo = (SENSOR_INFO*)pSensor->pSensorData;
	pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

 	pSensor->SensorDevise = OpenDevice("OWI");
	pProcInfo->Flag++; // Увеличиваем счетчик ошибок
	if(pProcInfo->Flag > 5)
	{
		pProcInfo->Flag = 5;
		DeviceIOCtl(pSensor->SensorDevise, GET_STATUS_DEVICE, &command);
		if(command & (1 << IWIRE_SD_BIT))
			pSensInfo->Data = 200;
		else
			pSensInfo->Data = 300;
		pSensInfo->PhysValue = PhysProcSensor(pSensor);
	}
	family = (uint8_t *)(pProcInfo->Data);
	switch(*family)
	{
		case 0x10:
		case 0x28:
			if(ReadTemperature(pSensor, pSensInfo, pProcInfo) == 0)
				pProcInfo->Flag = 0;
			break;
		case 0x29:
			if(ReadDS2408(pSensor, pSensInfo, pProcInfo) == 0)
				pProcInfo->Flag = 0;
	}
  		
  //  Переходим к следующему датчику

	OWISInfo.cur_sen++;
	if(OWISInfo.cur_sen >= OWISInfo.num_sens)
		OWISInfo.cur_sen = 0;

	pSensor = OWISInfo.pSInfo[OWISInfo.cur_sen];
	pSensInfo = (SENSOR_INFO*)pSensor->pSensorData;
	pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

	pSensor->SensorDevise = OpenDevice("OWI");
	
	switch(*family)
	{
		case 0x10:
		case 0x28:
			DeviceIOCtl(pSensor->SensorDevise, OWI_MATCH_ROM, (uint8_t *)(pProcInfo->Data));   //  захват устройства по его ID
			DeviceIOCtl(pSensor->SensorDevise, OWI_CHECK_PULL_UP, NULL);   //  запись с pullup'ом
		  
			command = 0x44;   // Команда для запуска измерения температуры
			if(WriteDevice(pSensor->SensorDevise, &command, 1) == -1)
			{
				CloseDevice(pSensor->SensorDevise);
				return err;
			}
			break;
		case 0x29:
			break;
	}

	

	CloseDevice(pSensor->SensorDevise);

	OWISInfo.pass_count = 0;

	return err;
}

int InitTemperFunc(SENSOR_HANDLE handle, uint8_t *data, uint16_t count)
{
    int i = 0;
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);
    uint8_t *p = NULL;

    p = (uint8_t *)pvPortMalloc((size_t)8);  //   Память для хранения ID температурного датчика
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }

    i = str_to_hex((char *)data, p, 8);

    if(i != 8)   // ВЫДАТЬ ОШИБКУ
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }

    //if(DeviceIOCtl((DEVICE_HANDLE *)(((SENSOR *)handle)->SensorDevise), 0x25, (uint8_t *)p) == 0) return -1;

    pProcInfo->Data = (void *)p;

    p = pvPortMalloc((size_t)2);  //  Память для хранения флага
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }
    *p = 0;
    pProcInfo->Buf = (void *)p;
		
		pProcInfo->Flag = 0;
		pSensInfo->Data = 300; // начальное значение датчика темпе5ратуры, чтобы однозначно было понятно что это не измеренное значение


    OWISInfo.pSInfo[OWISInfo.num_sens] = (SENSOR *)handle;
	OWISInfo.num_sens++;
	OWISInfo.pass_lim = OWISInfo.num_sens * (1000/SENSORS_POOL_TIMER_INTERVAL) + 1;

    return 0;
}

void DeInitOwiFunc(void)
{
	OWISInfo.num_sens = 0;
	OWISInfo.pass_lim = 0;
}

#endif // _TEMPER_FUNC_


#endif 

