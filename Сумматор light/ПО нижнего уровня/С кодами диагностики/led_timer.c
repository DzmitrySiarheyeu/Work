#include "led_timer.h"

volatile unsigned char FlashCount = 20;

volatile unsigned char Delay = BETWEN_FLASH_DELAY;
volatile unsigned char Flashed = 0;

inline void LedTimerInit(void)
{
    TCCR2B |= 7;
    sbi(TIMSK2, TOIE2);
}


// Interrupt freq = F_CPU / (1024 * 255) ~ 56.25 Hz for 14745600 
ISR(TIMER2_OVF_vect, ISR_NOBLOCK)
{	
    if (Delay)
    {
        Delay --;
    }
    else
    {
        LED_TOGGLE;
        Flashed ++;
        
        if (Flashed >= FlashCount)
        {
            LED_OFF;
            Flashed = 0;
            Delay = BETWEN_GROUP_DELAY;
        }
        else
        {
            Delay = BETWEN_FLASH_DELAY;
        }
    }
}
