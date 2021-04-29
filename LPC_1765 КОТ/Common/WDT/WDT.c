#include "config.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_wdt.h"

#include "WDT.h"

WDT_COUNTER	WDT_cnt[WDT_COUNTERS_NUM];

void WDTCountersInc(void)
{
	int i = 0;
	for(i = 0; i < WDT_COUNTERS_NUM; i++)
	{
		if(WDT_cnt[i].fl_on) WDT_cnt[i].cnt++;	
	}
}

#if(_WDT_LPC_)

#include "lpc17xx_gpio.h"
#include "lpc17xx_wdt.h"

#include "WDT.h"

void Default_WDTHandler(void)
{
#if(_INT_WDT_)
    // Внутренний WDT
    if (WDT_ReadTimeOutFlag() > 0)
        WDT_ClrTimeOutFlag();

    WDT_Init(WDT_CLKSRC_PCLK, WDT_MODE_RESET);
    WDT_Start(WDT_TOUT_us);
#endif
	
#if(_EXT_WDT_)
	GPIO_SetDir(EXT_WDT_PORT, (1 << EXT_WDT_PIN), EXT_WDT_PORT);
	for(int i = 0; i < 1000; i++)
		GPIO_SetValue(EXT_WDT_PORT, (1 << EXT_WDT_PIN));
	for(int i = 0; i < 1000; i++)
		GPIO_ClearValue(EXT_WDT_PORT, (1 << EXT_WDT_PIN));
#endif
	while(1){};
}


void InitWDT(void)
{
	
#if(_INT_WDT_)
    // Внутренний WDT
    if (WDT_ReadTimeOutFlag() > 0)
        WDT_ClrTimeOutFlag();

    WDT_Init(WDT_CLKSRC_PCLK, WDT_MODE_RESET);
    WDT_Start(WDT_TOUT_us);
#endif

#if(_EXT_WDT_)
    // Внешний WDT
	uint32_t ext_wdt_pin_state;
    ext_wdt_pin_state = GPIO_ReadValue(EXT_WDT_PORT);
    GPIO_SetDir(EXT_WDT_PORT, (1 << EXT_WDT_PIN), EXT_WDT_PORT);
	// первый раз дергаем ножкой чтобы запустить микросхему WATCHDOG
	if(ext_wdt_pin_state & (1 << EXT_WDT_PIN))
        GPIO_ClearValue(EXT_WDT_PORT, (1 << EXT_WDT_PIN));
    else
        GPIO_SetValue(EXT_WDT_PORT, (1 << EXT_WDT_PIN));
#endif
}

void ResetWDT(void)
{
#if(_INT_WDT_)
    // Внутренний WDT
    WDT_Feed();
#endif

#if(_EXT_WDT_)
    // Внешний WDT
    uint32_t ext_wdt_pin_state;
    ext_wdt_pin_state = GPIO_ReadValue(EXT_WDT_PORT);
    if(ext_wdt_pin_state & (1 << EXT_WDT_PIN))
        GPIO_ClearValue(EXT_WDT_PORT, (1 << EXT_WDT_PIN));
    else
        GPIO_SetValue(EXT_WDT_PORT, (1 << EXT_WDT_PIN));
#endif
}

#endif // _WDT_

