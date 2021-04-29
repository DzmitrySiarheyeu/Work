#include "freq_dot.h"
#include "errors_led.h"

                                    		// Переменная для создания интервалов времени, за которые подсчитывается количество импульсов генератора. 
											// Изменяется в прерывании таймера.
											// Переменная для подсчета импульсов, выдаваемых генератором. 
											// Изменяется по фронтуи срезу поступающего сигнала на микроконтроллер.
											// Поэтому переменная freq содержит удвоенное число подсчитанных импульсов
volatile FREQ_REGISTR   FREQ_DOT;

SIGNAL(SIG_OVERFLOW1)
{
	switch(FREQ_DOT.State)
	{
		case START_FREQ_MEASURE:
				FREQ_DOT.Count_Timer = 0;
				FREQ_DOT.Count_Capture = 0;
				FREQ_DOT.State = FREQ_MEASURE;
			break;
		case FREQ_MEASURE:
				FREQ_DOT.Count_Timer++;
				if(FREQ_DOT.Count_Timer >= N_Period_Measure)
				{
					FREQ_DOT.State = FREQ_REDY_MEASURE;
				}
			break;
		case IDLE_FREQ_MEASURE: FREQ_DOT.State = START_FREQ_MEASURE;
			break;

	}
	error_led_isr();
}

#ifdef FREQUENCY_INPUT

inline void FreqDotInit()
{
	double generatorFrequency;
	unsigned short dotIndex;
	unsigned short dotCount = Reverse2(RegFile.mb_struct.dot_info.N);
	
	generatorFrequency = 0.0;
	generatorFrequency = Reverse4(generatorFrequency);	
    for (dotIndex = 0; dotIndex < dotCount; dotIndex ++)
    {
        RegFile.mb_struct.dot_info.DOT[dotIndex].frequency = generatorFrequency;     
    }
  
    cbi(FREQ_DDR, FREQ_PIN);
	sbi(FREQ_CONTROL_DDR, FREQ_CONTROL_PIN_S0);
	sbi(FREQ_CONTROL_DDR, FREQ_CONTROL_PIN_S1);

	sbi(TIMSK1, ICIE1);
}

SIGNAL(SIG_INPUT_CAPTURE1)
{
	switch(FREQ_DOT.State)
	{
		case START_FREQ_MEASURE:
				if(TIFR1 & (1 << TOV1))
				{
					FREQ_DOT.State = IDLE_FREQ_MEASURE;
					return;
				}
				FREQ_DOT.First_Capture = ICR1;
			break;
		case FREQ_MEASURE:
				if((FREQ_DOT.Count_Timer >= (N_Period_Measure - 1)) && (TIFR1 & (1 << TOV1)))
				{
					FREQ_DOT.State = IDLE_FREQ_MEASURE;
					return;
				}
				FREQ_DOT.Count_Capture ++;
                FREQ_DOT.Last_Capture = ICR1;
			break;
	}
}

double CoreFrequancyMeasure(void)
{
	double freq;

	switch(FREQ_DOT.State)
	{
			case FREQ_REDY_MEASURE:
				FREQ_DOT.Tick_Counter  = ((long)VOUT_TIMER_MAX_TIMER_VALUE - (long)FREQ_DOT.First_Capture);
				FREQ_DOT.Tick_Counter += ((long)VOUT_TIMER_MAX_TIMER_VALUE * (long)FREQ_DOT.Count_Timer);
				FREQ_DOT.Tick_Counter -= ((long)VOUT_TIMER_MAX_TIMER_VALUE - (long)FREQ_DOT.Last_Capture);

				freq = ((double)TIMER1_CLOCK * (double)FREQ_DOT.Count_Capture) / (double)FREQ_DOT.Tick_Counter;
			
				FREQ_DOT.First_Capture = 0;
				FREQ_DOT.Last_Capture = 0;
				FREQ_DOT.Count_Timer = 0;
				FREQ_DOT.Tick_Counter = 0;
				return freq;		
				break;
			default:
			return -1.0;
				break;
		}
	return 0;
}

inline void SetDOTChannel(int dotIndex)
{
	FREQ_CONTROL_PORT &= ~(3 << FREQ_CONTROL_PIN_S0);
	FREQ_CONTROL_PORT |= dotIndex << FREQ_CONTROL_PIN_S0;
	/*if(dotIndex == 0)
	{
		cbi(FREQ_CONTROL_PORT, FREQ_CONTROL_PIN_S0);
		cbi(FREQ_CONTROL_PORT, FREQ_CONTROL_PIN_S1);
	}
	else
	{
		sbi(FREQ_CONTROL_PORT, FREQ_CONTROL_PIN_S0);
		cbi(FREQ_CONTROL_PORT, FREQ_CONTROL_PIN_S1);
	}*/
}

inline void FrequancyMeasure(void)
{
	double freq = 0;
	double generatorFrequency;
	static int dotIndex = 0;
	static int FreqState = 0;
	
	if(FreqState == 0)
	{
		
		dotIndex ++;
		dotIndex &= 0x3;
		SetDOTChannel(dotIndex);
		FREQ_DOT.State = IDLE_FREQ_MEASURE;
		FreqState++;
	}

	if(FreqState == 1)
	{
		if(CoreFrequancyMeasure() == -1.0)
			return;
		FreqState++;
		FREQ_DOT.State = IDLE_FREQ_MEASURE;
	}

	if(FreqState == 2)
	{
	
		freq = CoreFrequancyMeasure();
		if(freq == -1.0)
			return;
		FreqState = 0;
	}

	generatorFrequency = Reverse4(RegFile.mb_struct.dot_info.DOT[dotIndex].frequency);
	if(generatorFrequency != 0)
		generatorFrequency = generatorFrequency + 0.25 * (freq - generatorFrequency);
	else
	generatorFrequency = freq;
	RegFile.mb_struct.dot_info.DOT[dotIndex].frequency = Reverse4(generatorFrequency);
}

#endif
