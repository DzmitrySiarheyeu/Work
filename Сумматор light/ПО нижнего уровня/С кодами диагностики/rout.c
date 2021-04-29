#include "rout.h"
#include "adc.h" 
#include <math.h>
#include "potentiometer.h"
#include "omnicomm.h"

volatile double ROutPWMValue;

inline void ROutInit(void)
{   
    ADCInit();
    
    PotInit();  

    cbi(PRR,  PRTIM0);      //On TIMER0
 
    TCCR0B = ROUT_TIMER_CLOCK_DIVIRDER;

	sbi(TCCR0A, COM0A1);    //  Clear OC0A on Compare Match, set OC0A at BOTTOM,
                            //  (non-inverting mode) 
	sbi(TCCR0A, WGM00);
	sbi(TCCR0A, WGM01);

	//��������� ����� ������������ �������� "������ ���"
	sbi(FREQ_OUT_PORT, FREQ_OUT_PIN);					//��������������� 1 �� ������
	FREQ_OUT_DDR|=(1<<FREQ_OUT_PIN);	//���� ��������������� ��� �����

	if(RegFile.mb_struct.log_param.Rtable[ 0 ] > RegFile.mb_struct.log_param.Rtable[ LOG_PAR_COUNT - 1 ]) // ��������� �� ��������� �� � ��� ������� �������������
    	ROutPWMValue = 255; // ON PWM
	else ROutPWMValue = 0;  // OFF PWM

    OCR0A = ROutPWMValue;

    // pin to out
    sbi(OC0A_DDR, OC0A_PIN);
}

inline void ROutWidthSet(unsigned char value)
{
        OCR0A = value;
}
double uR = 0;
inline double GetAIN_U_Value()
{
	double value ;

	value = ADCGetRoundedValue(AIN_U_CHANNEL);
	value = value * (AIN_U_LOW_R + AIN_U_HIGH_R) / AIN_U_LOW_R;
    return value;  
}

double iR = 0;
inline double GetAIN_I_Value()
{
  	double value ;

	value = ADCGetRoundedValue(AIN_I_CHANNEL);
    value = value / (AIN_I_R * AIN_I_AMP_K);   
    return value;  
}

volatile double currentR = 0;

inline double GetR()
{
	iR = GetAIN_I_Value();
	uR = GetAIN_U_Value();

	if(uR < 0.015)
		return -1;

	if (iR < 0.0001)
	{
		if (ROutPWMValue >= (ROUT_PWM_MIN + ROUT_PWM_STEP))
            {
                // Using PWM
                ROutPWMValue -= ROUT_PWM_STEP;
                ROutWidthSet((unsigned char)ROutPWMValue);
            }
        return -1;
	}

    double iDivider = uR / (AIN_U_HIGH_R + AIN_U_LOW_R);
    
    iR += iDivider;

    return (uR / iR);
}

inline unsigned char SetR(double newR, double maxR)
{
    
	double delta_R;
	double pot_step;
	double pot_max;
	
    currentR = GetR();
	
	if(currentR == -1)
		return SET_R_ERROR;
	
	delta_R = fabs(newR - currentR);

    if (delta_R <= DELTA_R)
        return SET_R_OK;

	if(delta_R > (maxR * 0.01))
		pot_step = 0.5;
	else
		pot_step = POT_STEP;    

    if (currentR < newR)
    {
		// need to increase R_OUT
        if (PotValue >= (POT_MIN_VALUE + pot_step) )
        {
            // Using Digital Potentiometer if possible
            PotValue -= pot_step;
            PotSet((unsigned char)PotValue);
        } 
        else
        {
            if (ROutPWMValue <= (ROUT_PWM_MAX - ROUT_PWM_STEP))
            {
                // Using PWM
                ROutPWMValue += ROUT_PWM_STEP;
                ROutWidthSet((unsigned char)ROutPWMValue);
            }
			else 
            {
				if (delta_R > (maxR * HIGH_R_ERROR_TOLERANCE))
				{
					return SET_R_ERROR;
				}
            }
        }
    }
    else
    {
		if(newR < 35)
			pot_max = POT_VALUE(1000); // ������ ������� ��� ����� ��������� �������������
		else
			pot_max = POT_MAX_VALUE;
        // need to decrease R_OUT
        if (PotValue <= (pot_max - pot_step) )
        {
            // Using Digital Potentiometer if possible
            PotValue += pot_step;
            PotSet((unsigned char)PotValue);
        } 
        else
        {
            if (ROutPWMValue >= (ROUT_PWM_MIN + ROUT_PWM_STEP))
            {
                // Using PWM
                ROutPWMValue -= ROUT_PWM_STEP;
                ROutWidthSet((unsigned char)ROutPWMValue);
            }
			else 
            {
				if (delta_R > (maxR * HIGH_R_ERROR_TOLERANCE))
				{
					return SET_R_ERROR;
				}
            }
        }
    }
 
    return SET_R_OK;
}

// ���������� �������� ���������� �������
double GetLogR(void)
{
	byte i;
	double  temp;
	double  x1, x2, y1, y2;  // ���������� ����������� ��� ���������� �� ��������� ������ �������������� �������� ����� ����� ���������� ����������
												    	
	i = LOG_PAR_COUNT - 2;
	while((VolumePercent < Reverse4(RegFile.mb_struct.log_param.Vtable[i])) && i > 0) i--; // ������� ����� ����� ����� � ������� ��������� �������������� ��������
	// ����� ��������� �� ��������� ������ ������������� ������� ����� ����� ����������
	// ����� ������ ���� ���������� ����������� �������� ��� ������������� ������ �������. ��� ����� �� �������� �� ���������� ������� � �����������.
	// ����� ������� �� �������� ���������� ������� ����� ����� ���������� ����������
	
    x1 = Reverse4(RegFile.mb_struct.log_param.Vtable[i]); 
	x2 = Reverse4(RegFile.mb_struct.log_param.Vtable[i+1]);
	
    y1 = Reverse4(RegFile.mb_struct.log_param.Rtable[i]);
	y2 = Reverse4(RegFile.mb_struct.log_param.Rtable[i+1]);
	
    temp  = (y2 - y1) * (VolumePercent - x1);
	temp /= (x2 - x1);
	temp += (y1);
		
	return temp;
}

unsigned char ROutProcess()
{
	double maxR;

	if (RegFile.mb_struct.Fl_logometr)
    {
        maxR = Reverse4(RegFile.mb_struct.log_param.Rtable[ LOG_PAR_COUNT - 1 ]);
		if(Reverse4(RegFile.mb_struct.log_param.Rtable[ 0 ]) > maxR) // ��������� �� ��������� �� � ��� ������� �������������
			maxR = Reverse4(RegFile.mb_struct.log_param.Rtable[ 0 ]); 
		return SetR(GetLogR(), maxR);
    }
    
    return ERROR_OK_LED;
}

