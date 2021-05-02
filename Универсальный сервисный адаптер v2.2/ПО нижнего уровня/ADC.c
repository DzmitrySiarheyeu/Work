#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "usb_adapt.h"
#include "ADC.h"

#define VCC 5.0

//------------ADC-------------------------
#define AIN_U_CHANNEL   0
#define AIN_I_CHANNEL   6
#define REF_1_1_CHANNEL	14
#define AIN_DOT_CHANNEL 2

#define DIDR_AIN_U      ADC0D		// просто чтобы не удал€ть, так как 6 и 7 нету
#define DIDR_AIN_I      ADC5D

#ifndef sbi
	#define sbi(reg, bit) (reg |= (1<<bit))
#endif

#ifndef cbi
	#define cbi(reg, bit) (reg &= (~(1<<bit)))
#endif

static volatile ADC_BUF adc_buf[2];
static unsigned char cur_adc_ch = 0;

static unsigned char ADC_CH_LUT[NUM_ADC_CHANNEL] = {AIN_U_CHANNEL, AIN_I_CHANNEL};


static inline void ADCAddValue(uint32_t value, unsigned char channel)
{
	unsigned char i = 0;
	adc_buf[channel].buf[ adc_buf[channel].in ] = value;
	adc_buf[channel].in++;
	if(adc_buf[channel].in >= 10)
	{
		adc_buf[channel].in = 0;
	}

	adc_buf[channel].proc_value = 0;

	for(i = 0; i < 10; i++)
	{
		adc_buf[channel].proc_value += adc_buf[channel].buf[i];
	}

	adc_buf[channel].proc_value = adc_buf[channel].proc_value / 10;

}


static inline void ADCAcumValue(uint32_t value, unsigned char channel)
{

	
	adc_buf[channel].Acum = adc_buf[channel].Acum + value;
	adc_buf[channel].count++;
	
	if(adc_buf[channel].count == 115)
	{
		adc_buf[channel].Acum = adc_buf[channel].Acum / 115;
		ADCAddValue(adc_buf[channel].Acum, channel);
		adc_buf[channel].Acum = 0;
		adc_buf[channel].count = 0;
	}    
}

// ≈сли в буфере есть измеренное значение возвращаем 1
int16_t ADCGetValue(unsigned char channel)
{
	if(adc_buf[channel].out == adc_buf[channel].in)
	{
		return -1;
	} 
	adc_buf[channel].out++;
	if(adc_buf[channel].out >= ADC_BUF_MAX)
	{
		adc_buf[channel].out = 0;
	}
	return adc_buf[channel].proc_value;
}

void ADCSetChannel(unsigned char channel)
{
	channel = ADC_CH_LUT[channel];
    channel &= 0x0F;
    channel |= (ADMUX & 0xF0);
    ADMUX = channel;
}

ISR(ADC_vect)
{
	ADCAcumValue(ADC, cur_adc_ch);
	
	cur_adc_ch++;
	if(cur_adc_ch == NUM_ADC_CHANNEL){
		cbi(ADCSRA, ADEN);    // Off ADC
		cur_adc_ch = 0;
	}
	else {
		ADCSetChannel(cur_adc_ch);
		sbi(ADCSRA, ADSC);           //Start conversion
	}

}

void IsrTimeOutADC(void)
{
	sbi(ADCSRA, ADEN);    // On ADC
	ADCSetChannel(cur_adc_ch);
	sbi(ADCSRA, ADSC);           //Start conversion
}

inline void ADCInit(void)
{
	int i = 0;
	for(i = 0; i < NUM_ADC_CHANNEL; i++)
	{
		adc_buf[i].in = 0;
		adc_buf[i].out = 0;
		adc_buf[i].Acum = 0;
		adc_buf[i].count= 0;
	}

    DIDR0 |= (1<<DIDR_AIN_U) | (1<<DIDR_AIN_I);
    
    cbi(PRR,  PRADC);   //On ADC

    ADC_SET_REF_VCC;
    ADCSetChannel(AIN_U_CHANNEL);

	//Clock Speed
	ADCSRA |= ADC_CLOCK_DIVIDER | (1 << ADIE); 
}

double ADCGetRoundedValue(unsigned char channel)
{

	static double adc_value[NUM_ADC_CHANNEL] = {0,0};
	double A, B, C, alpha;
	int16_t value;

	A = Ram_reg.A[channel];
	B = Ram_reg.B[channel];
	C = Ram_reg.C[channel];
	alpha = Ram_reg.Alpha_A[channel];

	value = ADCGetValue(channel);
	if(value == -1)
	{
		return (A + adc_value[channel] * B + adc_value[channel] * adc_value[channel] *C);
	}
	adc_value[channel] = value;
	return (A + adc_value[channel] * B + adc_value[channel] * adc_value[channel] * C);
}
