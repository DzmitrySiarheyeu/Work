#include <string.h>
#include "freq.h"
#include "omnicomm.h"
#include "errors_led.h"
#include "settings.h"
#include "lpc11xx.h"
#include "gpio.h"
#include "fout.h"

/*******************************************/

volatile FREQ_REGISTR FREQ_DOT[4];

/*******************************************/

static void MatchEvent(int channel);
static void CaptureEvent(LPC_TMR_TypeDef *timer, int channel);
static int GetInterruptStatus(LPC_TMR_TypeDef *timer, InterruptType iType);
static void ClearPendingInterrupt(LPC_TMR_TypeDef *timer, InterruptType iType);
static float FrequencyCalculate(int channel);

/*******************************************/


void TIMER16_0_IRQHandler(void)
{

	if(GetInterruptStatus(LPC_TMR16B0, Capture))
	{
		ClearPendingInterrupt(LPC_TMR16B0, Capture);
		CaptureEvent(LPC_TMR16B0, 2);
	}
	
	if(GetInterruptStatus(LPC_TMR16B0, Match0))
	{	
		ClearPendingInterrupt(LPC_TMR16B0, Match0);		
		MatchEvent(2);

	}	

}

void TIMER16_1_IRQHandler(void)
{
	if(GetInterruptStatus(LPC_TMR16B1, Capture))
	{
		ClearPendingInterrupt(LPC_TMR16B1, Capture);
		CaptureEvent(LPC_TMR16B1, 1);
	}
	if(GetInterruptStatus(LPC_TMR16B1, Match0))
	{
		ClearPendingInterrupt(LPC_TMR16B1, Match0);
		MatchEvent(1);
	}	
}

void TIMER32_0_IRQHandler(void)
{
	if(GetInterruptStatus(LPC_TMR32B0, Capture))
	{
		ClearPendingInterrupt(LPC_TMR32B0, Capture);
		CaptureEvent(LPC_TMR32B0, 0);
	}
	if(GetInterruptStatus(LPC_TMR32B0, Match0))
	{
		ClearPendingInterrupt(LPC_TMR32B0, Match0);
		MatchEvent(0);
	}	
}

void TIMER32_1_IRQHandler(void)
{									
	if(GetInterruptStatus(LPC_TMR32B1, Capture))
	{
		ClearPendingInterrupt(LPC_TMR32B1, Capture);
		CaptureEvent(LPC_TMR32B1, 3);
	}
	if(GetInterruptStatus(LPC_TMR32B1, Match1))
	{
		MatchEvent(3);
		omnicomm_timer++;
		ClearPendingInterrupt(LPC_TMR32B1, Match1);
	}
//	if(GetInterruptStatus(LPC_TMR32B1, Match2))
//	{
//		omnicomm_timer++;
//		ClearPendingInterrupt(LPC_TMR32B1, Match2);
//	}
	if(GetInterruptStatus(LPC_TMR32B1, Match0))
	{
		f_out_proc();
		ClearPendingInterrupt(LPC_TMR32B1, Match0);
	}	
}

static int GetInterruptStatus(LPC_TMR_TypeDef *timer, InterruptType iType)
{
	if(timer->IR & iType)	
		return 1;
	return 0;
}

static int GetInterruptChStatus(LPC_TMR_TypeDef *timer, int channel)
{
	InterruptType iType;
	if(channel == 3)
		iType = Match1;
	else iType = Match0;

	if(timer->IR & iType)	
		return 1;
	return 0;
}

static void ClearPendingInterrupt(LPC_TMR_TypeDef *timer, InterruptType iType)
{
	timer->IR = iType;	
}


static void MatchEvent(int channel)
{
	switch(FREQ_DOT[channel].State)
	{
		case START_FREQ_MEASURE:
				FREQ_DOT[channel].Count_Timer++;
				if(FREQ_DOT[channel].Count_Timer >= N_Period_Measure)
				{
					FREQ_DOT[channel].State = FREQ_REDY_MEASURE;
					FREQ_DOT[channel].Count_Capture = 0;
					return;
				}
				FREQ_DOT[channel].Count_Capture = 0;
			break;
		case FREQ_MEASURE:
				FREQ_DOT[channel].Count_Timer++;
				if(FREQ_DOT[channel].Count_Timer >= 2*N_Period_Measure)
				{
					FREQ_DOT[channel].State = FREQ_REDY_MEASURE;
				}
			break;
		case IDLE_FREQ_MEASURE:
			FREQ_DOT[channel].State = START_FREQ_MEASURE;
			break;

	}
	if(channel == 0)
		error_led_isr();	
}

static void CaptureEvent(LPC_TMR_TypeDef *timer, int channel)
{
	switch(FREQ_DOT[channel].State)
	{
		case START_FREQ_MEASURE:
				if(GetInterruptChStatus(timer, channel))
				{
					FREQ_DOT[channel].First_Capture = 0;
					FREQ_DOT[channel].Last_Capture = 0;
					FREQ_DOT[channel].Count_Timer = 0;
					FREQ_DOT[channel].Tick_Counter = 0;
					FREQ_DOT[channel].Count_Capture = 0;
					FREQ_DOT[channel].State = IDLE_FREQ_MEASURE;
					return;
				}
				FREQ_DOT[channel].First_Capture = timer->CR0;
				FREQ_DOT[channel].State = FREQ_MEASURE;
				FREQ_DOT[channel].Count_Timer = 0;
			break;
		case FREQ_MEASURE:
				FREQ_DOT[channel].Last_Capture = timer->CR0;
				FREQ_DOT[channel].Count_Capture++;
				if((FREQ_DOT[channel].Count_Timer >= (N_Period_Measure - 1)))
				{
					if(GetInterruptChStatus(timer, channel))
					{
						FREQ_DOT[channel].First_Capture = 0;
						FREQ_DOT[channel].Last_Capture = 0;
						FREQ_DOT[channel].Count_Timer = 0;
						FREQ_DOT[channel].Tick_Counter = 0;
						FREQ_DOT[channel].Count_Capture = 0;
						FREQ_DOT[channel].State = IDLE_FREQ_MEASURE;
						return;
					}
					FREQ_DOT[channel].State = FREQ_REDY_MEASURE;
				}
			
			break;
	}
}



static float FrequencyCalculate(int channel)
{		 
	float freq = -1.0f;

	if(FREQ_DOT[channel].State == FREQ_REDY_MEASURE)
	{

		FREQ_DOT[channel].Tick_Counter  = TIMER_TOP - FREQ_DOT[channel].First_Capture;
		FREQ_DOT[channel].Tick_Counter += TIMER_TOP * FREQ_DOT[channel].Count_Timer;
		FREQ_DOT[channel].Tick_Counter -= TIMER_TOP - FREQ_DOT[channel].Last_Capture;

		freq = (float)TIMER_FREQ * (float)FREQ_DOT[channel].Count_Capture / (float)FREQ_DOT[channel].Tick_Counter;

		FREQ_DOT[channel].First_Capture = 0;
		FREQ_DOT[channel].Last_Capture = 0;
		FREQ_DOT[channel].Count_Timer = 0;
		FREQ_DOT[channel].Tick_Counter = 0;
		FREQ_DOT[channel].Count_Capture = 0;
		FREQ_DOT[channel].State = IDLE_FREQ_MEASURE;
	}

	return freq;
}


/*************************************************************************************************************************/

void FrequencyMeasure(void)
{
	float currFreq, prevFreq;

	int i;
	for(i = 0; i < 4; i++)
	{
		currFreq = FrequencyCalculate(i);
		if(currFreq == -1.0f)
			continue;

		prevFreq = Reverse4(RegFile.mb_struct.dot_info.DOT[i].frequency);
		if(prevFreq != 0)
			currFreq = prevFreq + FREQ_ALPHA * (currFreq - prevFreq);

		RegFile.mb_struct.dot_info.DOT[i].frequency = Reverse4(currFreq);
	}
}

void FrequencyInit(void)
{
	//-----------------------Fin_1------------------------//

	// Configure PIO1_5 as CT32B0_CAP
	LPC_IOCON->PIO1_5 = 0x02UL;
	// Enable clocking
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<9);

	// Configure prescaler
	LPC_TMR32B0->PR = TIMER_DIVIDER_HW;
	/*
	Counter freq = PCLK / (PR + 1)
	*/
	
	// Choose timer mode
	LPC_TMR32B0->CTCR = 0;
	// Configure match channel 0
    LPC_TMR32B0->MR0 = TIMER_TOP;
	// Interrupt and Reset on MR0
	LPC_TMR32B0->MCR = 3;
	// Interrupt on capture (rising edge)
	LPC_TMR32B0->CCR = 0x05UL;

	// Enable the TMR32B0 Interrupt
    NVIC_EnableIRQ(TIMER_32_0_IRQn);
	// Start the timer
	LPC_TMR32B0->TCR = 1;


	//-----------------------Fin_2------------------------//

	// Configure R_PIO1_0 as CT32B1_CAP
	LPC_IOCON->R_PIO1_0 |= 0x83UL;
	//LPC_IOCON->R_PIO1_0 |= 1 << 5;
	// Enable clocking
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<10);

	// Configure prescaler
	LPC_TMR32B1->PR = TIMER_DIVIDER_HW;
	/*
	Counter freq = PCLK / (PR + 1)
	*/
	
	// Choose timer mode
	LPC_TMR32B1->CTCR = 0;
	// Configure match channel 1
    LPC_TMR32B1->MR1 = TIMER_TOP;
	// Interrupt and Reset on MR0
	LPC_TMR32B1->MCR |= (1 << 3) | (1 << 4);
	// Interrupt on capture (rising edge)
	LPC_TMR32B1->CCR = 0x05UL;

	// Enable the TMR32B1 Interrupt
    NVIC_EnableIRQ(TIMER_32_1_IRQn);
	// Start the timer
	LPC_TMR32B1->TCR = 1;

	//-----------------------Fin_3------------------------//

	// Configure PIO0_2 as CT16B0_CAP

	LPC_IOCON->PIO0_2 = 0x02UL;
	// Enable clocking
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);

	// Configure prescaler
	LPC_TMR16B0->PR = TIMER_DIVIDER_HW;
	/*
	Counter freq = PCLK / (PR + 1)
	*/
	
	// Choose timer mode
	LPC_TMR16B0->CTCR = 0;
	// Configure match channel 0
    LPC_TMR16B0->MR0 = TIMER_TOP;
	// Interrupt and Reset on MR0
	LPC_TMR16B0->MCR = 3;
	// Interrupt on capture (rising edge)
	LPC_TMR16B0->CCR = 0x05UL;

	// Enable the TMR16B0 Interrupt
    NVIC_EnableIRQ(TIMER_16_0_IRQn);
	// Start the timer
	LPC_TMR16B0->TCR = 1;

	//-----------------------Fin_4------------------------//

	// Configure PIO1_8 as CT16B1_CAP
	LPC_IOCON->PIO1_8 = 0x01UL;
	// Enable clocking
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<8);

	// Configure prescaler
	LPC_TMR16B1->PR = TIMER_DIVIDER_HW;
	/*
	Counter freq = PCLK / (PR + 1)
	*/
	
	// Choose timer mode
	LPC_TMR16B1->CTCR = 0;
	// Configure match channel 0
    LPC_TMR16B1->MR0 = TIMER_TOP;
	// Interrupt and Reset on MR0
	LPC_TMR16B1->MCR = 3;
	// Interrupt on capture (rising edge)
	LPC_TMR16B1->CCR = 0x05UL;

	// Enable the TMR16B1 Interrupt
    NVIC_EnableIRQ(TIMER_16_1_IRQn);
	// Start the timer
	LPC_TMR16B1->TCR = 1;

}
