#ifndef CLOCK_DRV_H
#define CLOCK_DRV_H

#include "Config.h"

#if(_CLOCK_TYPE_ == _CLOCK_TYPE_M41T56_)
	#include "M41T56.h"
#elif(_CLOCK_TYPE_ == _CLOCK_TYPE_M41T81_)
	#include "M41T81.h"
#elif(_CLOCK_TYPE_ == _CLOCK_TYPE_GPS_)
	#include "GPS_Clock.h"
#elif(_CLOCK_TYPE_ == _CLOCK_TYPE_LPC17xx_RTC_)
	#include "rtc_lpc17xx.h"
#elif(_CLOCK_TYPE_ == _CLOCK_TYPE_STM32F1_RTC_)
	#include "rtc_stm32f10x.h"
#endif

#endif // CLOCK_DRV_H

