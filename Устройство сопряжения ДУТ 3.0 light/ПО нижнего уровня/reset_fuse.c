#include "reset_fuse.h"

volatile unsigned short ResetTimerCounter = 0;

inline void SettingsReset()
{
    RegFile.mb_struct.Configured = 0;
    eeprom_write_block(&RegFile, &EepromRegFile, sizeof(REGISTER_FILE));
    
    unsigned long baud = DEFAULT_BAUDRATE;

    eeprom_write_byte(&EEP_ID0, 99);
    eeprom_write_block(&baud, &EEP_Boudrate0, sizeof(baud));
    
    InitUART();
}

inline void ResetFuseProcess()
{
    static unsigned char  ResetFuseProcessing = 0;

    if (RESET_SHORTED && ResetTimerCounter > RESET_TIMER_COUNTER_DELAY && !ResetFuseProcessing )
    {
		ResetFuseProcessing = 1; 
    }
    
    if (!RESET_SHORTED)
    {
        ResetTimerCounter = 0;
		
		if ( ResetFuseProcessing )
		{
			ResetFuseProcessing = 0;
			cli();
			LED_OFF;
        	unsigned short i;
			for (i = 0; i < 300; i++)
			{
				_delay_ms(10);
				wdt_reset();
			}

        	SettingsReset();
        	LED_ON;            
			sei();
		}
    }
}
