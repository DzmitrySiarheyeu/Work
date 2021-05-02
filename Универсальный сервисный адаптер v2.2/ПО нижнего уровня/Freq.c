#include "Freq.h"
#include "usb_adapt.h"
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/wdt.h>
#include <string.h>


FREQ_REG FREQ_DOT;

int test_fl = 0;

SIGNAL(TIMER1_CAPT_vect)
{
	switch(FREQ_DOT.State)
	{
		case IDLE_FREQ_MEASURE:
			break;
		case START_FREQ_MEASURE:
				if(TIFR1 & (1 << TOV1))
				{
					FREQ_DOT.State = IDLE_FREQ_MEASURE;
					return;
				}
				FREQ_DOT.Count_Capture = 0;
				*((unsigned char *)&FREQ_DOT.First_Capture    ) = ICR1L;
				*((unsigned char *)&FREQ_DOT.First_Capture + 1) = ICR1H;
				FREQ_DOT.PreState = START_FREQ_MEASURE;
			break;
		case FREQ_MEASURE:
				if(FREQ_DOT.PreState == IDLE_FREQ_MEASURE)
				{
					if(TIFR1 & (1 << TOV1))
					{
						FREQ_DOT.State = IDLE_FREQ_MEASURE;
						return;
					}
					FREQ_DOT.Count_Capture = 0;
					*((unsigned char *)&FREQ_DOT.First_Capture    ) = ICR1L;
					*((unsigned char *)&FREQ_DOT.First_Capture + 1) = ICR1H;
					FREQ_DOT.PreState = START_FREQ_MEASURE;
					FREQ_DOT.State = START_FREQ_MEASURE;	
				}
				if((FREQ_DOT.Count_Timer >= (N_Period_Measure - 1)) && (TIFR1 & (1 << TOV1)))
				{
					FREQ_DOT.State = IDLE_FREQ_MEASURE;
					FREQ_DOT.PreState = IDLE_FREQ_MEASURE;
					return;
				}
				FREQ_DOT.Count_Capture++;
				*((unsigned char *)&FREQ_DOT.Last_Capture    ) = ICR1L;
				*((unsigned char *)&FREQ_DOT.Last_Capture + 1) = ICR1H;
				FREQ_DOT.CapFlag = 1;
			break;
		case FREQ_REDY_MEASURE:
			break;
		default:
			break;
	}
}
SIGNAL(SIG_OVERFLOW1)
{
	switch(FREQ_DOT.State)
	{
		case START_FREQ_MEASURE:
				FREQ_DOT.Count_Timer = 0;
				FREQ_DOT.Count_Capture = 0;
				FREQ_DOT.State= FREQ_MEASURE;
			break;
		case FREQ_MEASURE:
				FREQ_DOT.Count_Timer++;
				if((FREQ_DOT.Count_Timer >= N_Period_Measure && FREQ_DOT.CapFlag) || FREQ_DOT.Count_Timer >= N2_Period_Measure)
				{
					FREQ_DOT.State = FREQ_REDY_MEASURE;
					TIMSK1 &= ~((1 << TOIE1) | (1 << ICIE1));
				}
				FREQ_DOT.CapFlag = 0;
			break;
		case FREQ_REDY_MEASURE: 
			break;
		case IDLE_FREQ_MEASURE: FREQ_DOT.State = START_FREQ_MEASURE;
			break;
		default:
			break;
	}
}


void FREQProcess(void)
{
	unsigned char i = 0;
	double TempValue = 0;
			switch(FREQ_DOT.State)
		{
			case FREQ_REDY_MEASURE:

				FREQ_DOT.Tick_Counter  = ((long)0xFFFF - (long)FREQ_DOT.First_Capture);
				FREQ_DOT.Tick_Counter += ((long)0xFFFF * (long)FREQ_DOT.Count_Timer);
				FREQ_DOT.Tick_Counter -= ((long)0xFFFF - (long)FREQ_DOT.Last_Capture);
				
				FREQ_DOT.buf[ FREQ_DOT.in ]  =  ((double)Fclktimer * (double)FREQ_DOT.Count_Capture) / (double)FREQ_DOT.Tick_Counter;
//				TempValue = FREQ_DOT.buf[ FREQ_DOT.in ];
				FREQ_DOT.in++;
				if(FREQ_DOT.in >= 10)
				{
					FREQ_DOT.in = 0;
				}

				for(i = 0; i < 10; i++)
				{
					TempValue += FREQ_DOT.buf[i];
				}

				TempValue = TempValue / 10;

   				MB_REG.Reg.Freq = ConvertFormat(TempValue);
				TIMSK1 |= (1 << TOIE1) | (1 << ICIE1);
				FREQ_DOT.First_Capture = 0;
				FREQ_DOT.Last_Capture = 0;
				FREQ_DOT.Count_Timer = 0;
				FREQ_DOT.Tick_Counter = 0;
				FREQ_DOT.State = IDLE_FREQ_MEASURE;
				FREQ_DOT.PreState = IDLE_FREQ_MEASURE;
				FREQ_DOT.CapFlag = 0;
								break;
			case START_FREQ_MEASURE:
				break;
			case FREQ_MEASURE:
				break;
			default:
				break;
		}
		
}
