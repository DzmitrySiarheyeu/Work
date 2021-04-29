#include "adc.h" 
#include <math.h>
#include "analog_dot.h"
#include "omnicomm.h"

#ifdef ANALOG_INPUT

#define KOF_DELITEL_AIN_DOT ((10.0 + 5.1)/5.1)

void AnalogDotMeasure(void)
{
	double U_current = 0;
	double U_dot;
	U_current = ADCGetRoundedValue(AIN_DOT_CHANNEL) * KOF_DELITEL_AIN_DOT;
	U_dot = Reverse4(RegFile.mb_struct.dot_info.DOT[0].frequency);
	if(U_dot != 0)
		U_dot = U_dot + 0.005 * (U_current - U_dot);
	else
		U_dot = U_current;
	RegFile.mb_struct.dot_info.DOT[0].frequency = Reverse4(U_dot);
}

#endif
