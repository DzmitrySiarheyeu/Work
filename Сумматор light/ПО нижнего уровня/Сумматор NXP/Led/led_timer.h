#pragma once

//-------------SET--------------------

#define     BETWEN_FLASH_DELAY  10
#define     BETWEN_GROUP_DELAY  100

//-------------SET--------------------

inline void LedTimerInit(void);

extern volatile unsigned char FlashCount;

#define ERROR_OK                    0
#define ERROR_DOT_DISCONNECTED      1
#define ERROR_LOGOMETR_BREAK        2
#define ERROR_LOGOMETR_SHORT        3
#define ERROR_DOT_DISC_LOG_BREAK    4
#define ERROR_DOT_DISC_LOG_SHORT    5

#define INDICATE_ERROR(x) FlashCount = 2 * x;

