#pragma once

#include "util.h"

extern volatile double ADCRef;



// ADC Clock = CLK / DIVIDER
#define ADC_CLOCK_DIVIDER_2    1
#define ADC_CLOCK_DIVIDER_4    2
#define ADC_CLOCK_DIVIDER_8    3
#define ADC_CLOCK_DIVIDER_16   4
#define ADC_CLOCK_DIVIDER_32   5
#define ADC_CLOCK_DIVIDER_64   6
#define ADC_CLOCK_DIVIDER_128  7

//-------------SET--------------------

#define ADC_CLOCK_DIVIDER ADC_CLOCK_DIVIDER_64
#define ADC_ROUND_COUNT	16
#define CHANGE_REF_DELAY_MS	1
//-------------SET--------------------

#define ADC_SET_REF_VCC cbi(ADMUX, REFS1); sbi(ADMUX, REFS0); ADCRef = VCC; //_delay_ms(CHANGE_REF_DELAY_MS); 
#define ADC_SET_REF_1_1 sbi(ADMUX, REFS1); sbi(ADMUX, REFS0); ADCRef = 1.1; //_delay_ms(CHANGE_REF_DELAY_MS); 

void ADCInit(void);
unsigned short ADCRead(void);
void ADCSetChannel(unsigned char channel);
double ADCGetRoundedValue(unsigned char channel);
