#include <stdint.h>
#include "errors_led.h"
#include "gpio.h"

volatile unsigned char FlashCount = 20;

volatile unsigned int Delay = BETWEN_FLASH_DELAY;
volatile unsigned char Flashed = 0;

inline void LedTimerInit(void)
{
}

inline void ErrorProcess(unsigned char dotError, unsigned char rOutError)
{
    if (dotError != ERROR_OK_LED)
    {
        INDICATE_ERROR(dotError);
        return;
    }
    
    if (rOutError != ERROR_OK_LED)
    {
        INDICATE_ERROR(rOutError);
        return;
    }
    
    INDICATE_ERROR(ERROR_OK_LED);
}

void error_led_isr(void)
{
	static int fl = 0x00;	
    if (Delay)
    {
        Delay --;
    }
    else
    {
		fl ^= 0x01;
        SET_LED_STATE(fl);

        Flashed ++;
        
        if (Flashed >= FlashCount)
        {
            LED_ON;
			fl = 0x01;
            Flashed = 0;
            Delay = BETWEN_GROUP_DELAY;
        }
        else
        {
            Delay = BETWEN_FLASH_DELAY;
        }
    }

}
