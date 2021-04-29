#include <stdint.h>
#include "fout.h"
#include <math.h>
#include "lpc11xx.h"
#include "gpio.h"
#include "freq.h"

// задержки дел€тс€ на 2 группы больша€-которых несколько и  хвост последн€€ 
// (больша€ -  условно, они почти равны с хвостом)

typedef struct tagOutputSettings{
	uint16_t big;				 
	uint16_t count_big;			
	uint16_t hvost;				
}OUTPUT_SETTINGS;

#define OFF		0
#define ON		1

static volatile OUTPUT_SETTINGS newOutSettings, intOutSettings;
static volatile unsigned char isFreqSettingsValide = OFF;
static unsigned char startFlag = ON;


void FOutInit(void)
{
	INIT_F_OUT_PIN();
	LPC_TMR32B1->MR0 = TIMER_TOP;
	LPC_TMR32B1->MCR |= (1 << 0);
	LPC_TMR32B1->EMR = (1 << 4);
	isFreqSettingsValide = OFF;
}

// расчЄт задержек
void set_Freq(float freq)
{
	uint32_t counts;
		  
	//freq = 2000;

	while (isFreqSettingsValide);
	freq = freq*2;
	counts = ((OUT_FRTBL_TO_FR/freq) - 1.0);
		
	if (counts == 0)
		return;	

	uint32_t tmp_hvost;	

	newOutSettings.count_big = (counts/TIMER_TOP)+1;	

	newOutSettings.big = counts/newOutSettings.count_big;

	newOutSettings.count_big--;

	tmp_hvost=counts-(((uint32_t)newOutSettings.big)*((uint32_t)newOutSettings.count_big));

	while (tmp_hvost > TIMER_TOP){
		tmp_hvost-=newOutSettings.big;
		//newOutSettings.count_big++;
	}



	newOutSettings.hvost=tmp_hvost;


	isFreqSettingsValide = ON;
	startFlag = OFF;
	
}

//static uint8_t fl = 0;
void f_out_proc(void)
{
	static uint16_t count_big = 0;

	if(startFlag)
		return;
	if 	(count_big){
		LPC_TMR32B1->MR0 += intOutSettings.big;
		if(LPC_TMR32B1->MR0 > TIMER_TOP) LPC_TMR32B1->MR0 = LPC_TMR32B1->MR0 - TIMER_TOP - 1;
		count_big--;
	}
	else 
	{
		//fl ^= 0x01;
		SET_F_OUT_PIN(/*fl*/);	// инверси€ ноги
		//count_big 	= intOutSettings.count_big;
		LPC_TMR32B1->MR0 += intOutSettings.hvost;
		if(LPC_TMR32B1->MR0 > TIMER_TOP) LPC_TMR32B1->MR0 = LPC_TMR32B1->MR0 - TIMER_TOP - 1;

		if (isFreqSettingsValide)
		{
			intOutSettings.count_big 	= newOutSettings.count_big;
			intOutSettings.big			= newOutSettings.big;
			intOutSettings.hvost		= newOutSettings.hvost;
			isFreqSettingsValide = OFF;      	
		}
		count_big 	= intOutSettings.count_big;	
	}
}

////----------------- R OUT TIMER0-----------
//
//
//// задержки дел€тс€ на 2 группы больша€-которых несколько и  хвост последн€€ 
//// (больша€ -  условно, они почти равны с хвостом)
//	
//volatile	uint32_t big=0;				// больша€ задержка
//volatile	uint32_t big_tmp=0;			// больша€ задержка 
//volatile	uint32_t count_big=0;			// количество больших задержек
//volatile	uint32_t count_big_tmp=0;		// тоже самое, декрементируетс€ в прерывании
//	
//volatile	uint32_t hvost;				// последн€€ задержка
//volatile	uint32_t hvost_tmp;			// последн€€ задержка
//
//void FOutInit(void)
//{
//	INIT_F_OUT_PIN();
//	LPC_TMR32B1->MR0 = TIMER_TOP;
//	LPC_TMR32B1->MCR |= (1 << 0);
//}
//
//// расчЄт задержек
//void set_Freq(float freq){
//
////	LPC_TMR32B1->MR0 = TIMER_FREQ /	(2 * freq);
////	LPC_TMR32B1->MCR |= (3 << 4);	
//	freq = 200;
//	uint32_t tmp_hvost;
//	uint32_t counts;
//	uint32_t tmp_count_big;
//	uint32_t tmp_big;
//
//	counts = ((OUT_FRTBL_TO_FR/(freq * 2)) - 1.0);
//
//	tmp_count_big = (counts/TIMER_TOP)+1;	
//	
//	tmp_big=counts/tmp_count_big;
//
//	tmp_count_big--;
//
//	tmp_hvost=counts-(uint32_t)(tmp_big*(uint32_t)tmp_count_big);
//
//	while (tmp_hvost > TIMER_TOP){
//		tmp_hvost-=tmp_count_big;	   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//		tmp_big++;
//	}
//
//	tmp_big--;
//
//	big = tmp_big;
//	count_big = tmp_count_big;	
//	hvost=tmp_hvost;
//}
//
//
//static uint8_t fl = 0;
//
//void f_out_proc(void)
//{
//
//	if 	(count_big_tmp){
//		LPC_TMR32B1->MR0 = big_tmp;
//		count_big_tmp--;
//	}
//	else 
//	{
//		fl ^= 0x01;
//		SET_F_OUT_PIN(fl);	// инверси€ ноги
//		LPC_TMR32B1->MR0 = hvost_tmp;
//
//		count_big_tmp=count_big;
//		big_tmp=big;
//		hvost_tmp=hvost;
//	}
//}
