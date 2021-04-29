#pragma once

#include "util.h"
#include "vout.h"
#include "settings.h"

//-------------SET--------------------

// acceptable frequency error 
#define DOT_DELTA	50
#define DEFAULT_DOT_MAX_FREQ	1500
//-------------SET--------------------

static inline double get_low_frequency(int dotIndex)
{
    return (get_double_P(&(ProgramDotTable[dotIndex].Ftable[ 0 ]))); //RegFile.mb_struct.dot_info.DOT[dotIndex].Ftable[ 0 ]); 
}


static inline double get_high_frequency(int dotIndex)
{
	unsigned short lastVolume = Reverse2(RegFile.mb_struct.dot_info.DOT[dotIndex].N) - 1;
    return (get_double_P(&(ProgramDotTable[dotIndex].Ftable[ lastVolume ])));//Reverse4(RegFile.mb_struct.dot_info.DOT[dotIndex].Ftable[lastVolume]); 
}

typedef struct tagFREQ_Registr	
{
	unsigned int 	Count_Timer;
	unsigned int 	Count_Capture;
	unsigned char 	State;
	unsigned short	First_Capture;
	unsigned short	Last_Capture;
	long	Tick_Counter;
}FREQ_REGISTR;

//********************************
// Состояние режимов работы измерителя частоты
#define IDLE_FREQ_MEASURE		0
#define START_FREQ_MEASURE		1    	// Начало измерения частоты
#define FREQ_MEASURE			2		// Непосредсвенно измерение частоты
#define FREQ_REDY_MEASURE		3		// Результат измерения готов

#define N_Period_Measure		20		// Колличество переполнений таймера в за время которых производится измерение частоты

extern volatile double generatorFrequency; 

void StoreFreqDotLevelPercent(DOT_PARAM* dotParam);
void FreqDotInit();
void FrequancyMeasure(void);

