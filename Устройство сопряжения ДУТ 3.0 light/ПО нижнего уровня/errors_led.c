#include "errors_led.h"

volatile unsigned char FlashCount = 20;

volatile unsigned int Delay = BETWEN_FLASH_DELAY;
volatile unsigned char Flashed = 0;

inline void LedTimerInit(void)
{
   
}

inline void ErrorProcess(unsigned char dotError, unsigned char rOutError)
{
    if (dotError != ERROR_OK)
    {
        INDICATE_ERROR(dotError);
        return;
    }
    
    if (rOutError != ERROR_OK)
    {
        INDICATE_ERROR(rOutError);
        return;
    }
    
    INDICATE_ERROR(ERROR_OK);
}

void error_led_isr(void)
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
            LED_ON;
            Flashed = 0;
            Delay = BETWEN_GROUP_DELAY;
        }
        else
        {
            Delay = BETWEN_FLASH_DELAY;
        }
    }

}
