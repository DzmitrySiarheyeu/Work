#include <stdint.h>
#include <string.h>
#include "settings.h"
#include "vout.h"
#include "fout.h"
#include "errors_led.h"
#include "sbl_iap.h"
#include "gpio.h"
#include "freq.h"

#include "dot.h"
#include "wdt.h"
#include "uart.h"

#include "mbclient.h"
#include "mbfanction0.h"
#include "omnicomm.h"


FL_REGISTER_STRUCT *pFlashRegFile = (FL_REGISTER_STRUCT *)TABLE_START_ADDRESS;	    //  Указатель на страницу памяти во Флэш
FL_REGISTER_STRUCT FlashRegFile;													//  Дубликат структуры кот храниться во флэши
																					//  нужен для изменений параметров и обратного копирования во флэш
REGISTER_FILE RegFile;


DOT_TABLE *ProgramDotTable = 0;

DEVICE_TYPE DevType;

static uint8_t rs_pow = 0; 

int a = sizeof(REGISTER_FILE);

void init_device_type(void)
{
	if((LPC_GPIO2->DATA & FERQ_DEVICE_PINS) == FERQ_DEVICE_PINS)
		DevType = DevFreq;
	else if((LPC_GPIO2->DATA & ANALOG_DEVICE_PINS) == ANALOG_DEVICE_PINS)
		DevType = DevAnalog;	
}


void init_rs_check_pin(void)
{
	LPC_IOCON->SWDIO_PIO1_3 = (2 << 3) | (1 << 0) | (1 << 7);
	GPIOSetDir(1, 3, 0);
} 

void check_rs_pow(void)
{
	if((LPC_GPIO1->DATA & (1 << 3)) == 0)
		rs_pow = 1;
	else rs_pow = 0;		
}


FL_REGISTER_STRUCT * GetFlashActualData(void)
{
	memcpy((char *)&FlashRegFile, (char *)pFlashRegFile, sizeof(FL_REGISTER_STRUCT));
	return &FlashRegFile;
}

float get_double_P(void *p)
{
	float temp;
	memcpy(&temp, p, sizeof(temp));
	return temp;
}

/*Функция считает crc таблицы параметров и записывает всю таблицу во флэш*/
void WriteFlash(FL_REGISTER_STRUCT *flash)
{
	uint16_t crc = 0xFFFF;
	crc = CRC16(((uint8_t *)flash), sizeof(FL_REGISTER_STRUCT) - 2, 0xffff);
	flash->crc = crc;
	write_flash((unsigned *)pFlashRegFile, (char *)flash, sizeof(FL_REGISTER_STRUCT));
}


/*Функция проверки целостности данных во флэше*/
void init_flash_param(void)
{
	FL_REGISTER_STRUCT *pROMFile = GetFlashActualData();
	uint16_t crc = CRC16(((uint8_t *)pFlashRegFile), sizeof(FL_REGISTER_STRUCT) - 2, 0xffff);
	if(pFlashRegFile->crc != crc)
	{												   //  Если слитела флэш заполняем ее по умолчанию
	    memset((char *)pROMFile, 0, sizeof(FL_REGISTER_STRUCT));       // Обнуляем все регистры modbus

		pROMFile->mb_struct.dot_info.DOT[0].N = Reverse2(2);
		pROMFile->mb_struct.dot_info.DOT[1].N = Reverse2(2);
		pROMFile->mb_struct.dot_info.DOT[2].N = Reverse2(2);
		pROMFile->mb_struct.dot_info.DOT[3].N = Reverse2(2);
		pROMFile->mb_struct.dot_info.N = Reverse2(1);

		pROMFile->omn_param.e2_id = VERSION_PO;
		pROMFile->omn_param.e2_n = 0;
		pROMFile->omn_param.e2_f_curr = 0;
		pROMFile->omn_param.e2_net_mode = 0;
		pROMFile->omn_param.e2_t = 0;
		pROMFile->omn_param.e2_period_auto = 1;
		pROMFile->omn_param.e2_report_on = 0;
		pROMFile->omn_param.e2_old_contin = 0;

		pROMFile->ID = 99;
		pROMFile->boudrate = 19200;
		WriteFlash(pROMFile);
	}

	memcpy((char *)&RegFile, (char *)pFlashRegFile, sizeof(REGISTER_FILE));		   //  Считываем с флэша в RAM	 все параметры
	RegFile.mb_struct.U_type = VERSION_PO;

	memcpy((char *)&omn_param, (char *)&pFlashRegFile->omn_param, sizeof(OMNI_PARAM));
}


void Init(void)
{
	ProgramDotTable = pFlashRegFile->ProgramDotTable;
	init_device_type();
	init_flash_param();
	Read_parameters(); // Omnicomm
    LED_INIT;
	UARTInit(pFlashRegFile->boudrate);
	FrequencyInit();
	init_rs_check_pin();

	if(DevType == DevFreq)
		FOutInit();
	else if(DevType == DevAnalog)
		VOutInit();
//    UARTInit(pFlashRegFile->boudrate);
	
}


//void ReservProcess(void)
//{
//    double RezervPoint = Reverse4(RegFile.mb_struct.Reserv);
//	
//	if (VolumePercent < RezervPoint)
//	{
//		RESERV_ON;
//	}
//	if (VolumePercent > RezervPoint + 0.01)	
//	{
//		RESERV_OFF;
//	}
//	
//}

int main(void)
{
	//volatile int aaa = (sizeof(DOT_TABLE) * MAX_DOT_COUNT);
	float dop_volume = 0;
	SystemInit();
	//SystemCoreClockUpdate();
#ifndef DEBUG
	WDTInit();
#endif


	unsigned char rout_error = 0; 
	unsigned char dot_error = 0;

   	Init();
//	set_Freq(1000.0);
	while (1)
	{

#ifndef DEBUG
		WDTFeed();
#endif

	    ModbusProcess();
        dot_error  = DotsProcess(); 
		if(DevType == DevFreq)
			set_Freq(VolumePercent * 1000.0 + 500.0);
		else if(DevType == DevAnalog){
			dop_volume = VolumePercent;
			check_rs_pow();
		 	if(rs_pow == 1)
				VolumePercent = 0.5;	
			VOutProcess();
			VolumePercent = dop_volume;
		}		   
		ErrorProcess(dot_error, rout_error);
	}
}


void mem_reverce(void *data, unsigned char count)
{
	unsigned char i;
	byte temp;
	for(i = 0; i < count/2; i++)
	{
		temp = ((unsigned char *)data)[ i ];
		((unsigned char *)data)[ i ] = ((unsigned char *)data)[ count - i - 1 ];
		((unsigned char *)data)[ count - i -1 ] = temp;
	}
}

inline float Reverse4(float in)
{
    byte tmp;
    byte * p = (byte *)(&in);
    
    tmp = *p;       *p = *(p + 3);       *(p + 3) = tmp;
    tmp = *(p + 1); *(p + 1) = *(p + 2); *(p + 2) = tmp;

    return in;
}

inline unsigned short Reverse2(unsigned short in)
{
    byte tmp;
    byte * p = (byte *)(&in);
    
    tmp = *p;   *p = *(p + 1);  *(p + 1) = tmp;

    return in;
}

