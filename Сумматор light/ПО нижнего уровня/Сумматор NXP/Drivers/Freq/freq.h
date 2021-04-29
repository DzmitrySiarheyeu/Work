#ifndef FREQ_H
#define FREQ_H

#include <stdint.h>
#include "settings.h"
#include "system_lpc11xx.h"

typedef	struct
{
	uint32_t 	Count_Timer;
	uint32_t 	Count_Capture;
	uint32_t	First_Capture;
	uint32_t	Last_Capture;
	uint32_t	Tick_Counter;
	uint8_t 	State;
}FREQ_REGISTR;

typedef enum {Match0 = (1UL << 0), Match1 = (1UL << 1), Match2 = (1UL << 2), Match3 = (1UL << 3), Capture = (1UL << 4)} InterruptType;


#define TIMER_TOP				(12000)

#define TIMER_DIVIDER			(1)
#define TIMER_DIVIDER_HW		(0)
#define TIMER_FREQ				(SystemCoreClock / TIMER_DIVIDER)

#define FREQ_ALPHA				(0.25f)

#define IDLE_FREQ_MEASURE		0
#define START_FREQ_MEASURE		1	
#define FREQ_MEASURE			2	
#define FREQ_REDY_MEASURE		3

#define N_Period_Measure		100

// acceptable frequency error 
#define DOT_DELTA	50
#define DEFAULT_DOT_MAX_FREQ	1500

static inline float get_low_frequency(int dotIndex)
{
    return (get_double_P(&(ProgramDotTable[dotIndex].Ftable[ 0 ]))); //RegFile.mb_struct.dot_info.DOT[dotIndex].Ftable[ 0 ]); 
}


static inline float get_high_frequency(int dotIndex)
{
	unsigned short lastVolume = Reverse2(RegFile.mb_struct.dot_info.DOT[dotIndex].N) - 1;
    return (get_double_P(&(ProgramDotTable[dotIndex].Ftable[ lastVolume ])));//Reverse4(RegFile.mb_struct.dot_info.DOT[dotIndex].Ftable[lastVolume]); 
}

/************************************************/

void FrequencyInit(void);
void FrequencyMeasure(void);

/************************************************/

#endif // FREQ_H
