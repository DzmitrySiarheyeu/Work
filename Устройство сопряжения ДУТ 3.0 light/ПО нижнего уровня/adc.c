#include "adc.h"

volatile double ADCRef = 0;

inline void ADCSetChannel(unsigned char channel)
{
    channel &= 0x0F;
    channel |= (ADMUX & 0xF0);
    ADMUX = channel;
}

inline void ADCInit(void)
{
    DIDR0 |= (1<<DIDR_AIN_U) | (1<<DIDR_AIN_I);
    
    cbi(PRR,  PRADC);   //On ADC

    ADC_SET_REF_VCC;
    ADCSetChannel(AIN_U_CHANNEL);

	//Clock Speed
	ADCSRA |= ADC_CLOCK_DIVIDER; 
    sbi(ADCSRA, ADEN);    // On ADC

}

inline unsigned short ADCRead()
{
    sbi(ADCSRA, ADSC);           //Start conversion
     
    while (!(ADCSRA & (1<<ADIF)));    //wait for conversion complete
    
    ADCSRA |= (1<<ADIF);        //Clear flag;

    return ADC;
}

double refValue = 0;
inline double ADCGetRoundedValue(unsigned char channel)
{
    ADCSetChannel(channel);
	ADC_SET_REF_1_1;
    
    ADCRead();  // dummy	
		
	double value = 0;
	double nextValue = 0;
	

	unsigned char below1_1 = 1;
	unsigned char i;
	
	for (i = 0; i < ADC_ROUND_COUNT; i++)
	{
		nextValue = ADCRead();
		if (nextValue > 1000)
		{
			 below1_1 = 0;
			 break;
		}
		value += nextValue;
	}
	

	ADC_SET_REF_VCC;
	ADCRead();  // dummy

	if (! below1_1)
	{
		value = 0;

		for (i = 0; i < ADC_ROUND_COUNT; i++)
			value += ADCRead();
	}
	else
	{
		// get real 1.1 ref value
		ADCSetChannel(REF_1_1_CHANNEL);
		
		ADCRead();  // dummy
		
		nextValue = 0;
		for (i = 0; i < ADC_ROUND_COUNT; i++)
			nextValue += ADCRead();
		
		nextValue /= ADC_ROUND_COUNT;
		nextValue = nextValue * ADCRef / 1024;
		
		refValue = refValue + 0.1 * (nextValue - refValue);
		ADCRef = refValue;
	}

	value /= ADC_ROUND_COUNT;
   
	value = value * ADCRef / 1024;

	return value;
}

