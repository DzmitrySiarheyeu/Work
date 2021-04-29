#pragma once

#include "vout.h"

//-------------SET--------------------

#define     BETWEN_FLASH_DELAY  (200)
#define     BETWEN_GROUP_DELAY  (3000)

//-------------SET--------------------

void LedTimerInit(void);
void ErrorProcess(unsigned char dotError, unsigned char rOutError);

extern volatile unsigned char FlashCount;

#define ERROR_OK_LED                    0
#define ERROR_LOGOMETR              1
#define ERROR_DOT                   2

#define INDICATE_ERROR(x) FlashCount = 2 * x;

void error_led_isr(void);

