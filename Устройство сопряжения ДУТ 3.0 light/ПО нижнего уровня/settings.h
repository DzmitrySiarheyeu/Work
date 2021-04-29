#pragma once

#include "util.h"

#define MAX_DOT_COUNT	4
#define LOG_PAR_COUNT	9
#define DOT_PAR_COUNT	30


// 0x2100 - добавлено поддержка 30 точек
// 0x2200 - тест логометра
// 0x2212 - откорректирован алгоритм обработки значений выхода частоты датчика за верхний предел
// C 22 февраля 2010 формат версии следующий
// 0xHHTL
// HH - Старший номер версии
// T - тип устройства 0 - устройство сапряжения большое; 1 - сумматор; 2 - устройство сопряжения малое
// L - младший номер версии
#define VERSION_PO		0x2212

#define FREQUENCY_INPUT	1
//#define R_OUT_ENABLE	1

#ifndef FREQUENCY_INPUT
#define ANALOG_INPUT 1
#endif

#define __ALSO_FUNC__		__attribute__ ((section (".also_var")))

typedef struct tagLOG_param
{
	double Vtable[ LOG_PAR_COUNT ];
	double Rtable[ LOG_PAR_COUNT ];
}LOG_PARAM;	//4*8*2=64

typedef struct tagDOT_table
{
	double Ftable[ DOT_PAR_COUNT ];
	double Vtable[ DOT_PAR_COUNT ];
}DOT_TABLE;

typedef struct tagDOT_param
{
	double frequency;		// процент заполнения бака по уровню
						// !!!!!!!!! ЭТИ ТАБЛИЦЫ УЖЕ НЕ ИСПОЛЬЗУЮТСЯ
	double Ftable[10];	// таблица аппроксимации из двух массивов
	double Vtable[10];
	unsigned short N; 	// число точек аппроксимации объема
}DOT_PARAM;	//86 bytes

typedef struct tagDOT_info
{
	unsigned short N; // число ДОТ поддерживаемых устройством. До 2шт.
	unsigned short fl_freq; //флаг наличия частотного ДОТ
	DOT_PARAM DOT[ MAX_DOT_COUNT ];
}DOT_INFO;	//86*2 + 4 =	176

typedef struct tagRegisterStruct
{
	unsigned short U_type;		// 0 - выход напряжения до 4,5 В, 1 - выход напряжения до 12В, 2 - выход напряжения до 20В.
	unsigned short R_type;		// 0 - выход 0-90 Ом, 1 - выход 0-350 Ом, 2 - выход 0 - 900 Ом
	unsigned short Fl_logometr;	// показывает будет подключен логометр или нет.
	unsigned short Configured;	// 0 - не настроен 1 - настроен
	double VolumePercent;
	// 6 & 7 register
	double Reserv;			// порог срабатывания выхода "РЕЗЕРВ" в % от общего объема. Диапазон от 0 до 100.
	// 8 & 9 register
	double U_max;	// Общий объем в %
	DOT_INFO dot_info;
	LOG_PARAM log_param;
}REGISTER_STRUCT;	//176 + 20 + 64 = 260

typedef union tagRegisterFile
{
	REGISTER_STRUCT mb_struct;
	unsigned short mb_array[sizeof(REGISTER_STRUCT)/2];
}REGISTER_FILE;

double get_double_P(PGM_VOID_P p);

extern __ATTR_EEPROM__ REGISTER_FILE EepromRegFile;
extern REGISTER_FILE RegFile;

extern __ATTR_EEPROM__ byte  EEP_ID0 ;	
extern __ATTR_EEPROM__ unsigned long   EEP_Boudrate0 ;
extern DOT_TABLE ProgramDotTable[MAX_DOT_COUNT];
