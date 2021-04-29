#pragma once

#include "util.h"
#include "settings.h"
#include "usart.h"

#define RESET_FUSE_INIT sbi(RESET_PORT, RESET_PIN); cbi(RESET_DDR, RESET_PIN);

#define RESET_SHORTED   (!(RESET_READ_PIN & (1<<RESET_PIN)))

#define RESET_TIMER_COUNTER_DELAY 562

extern volatile unsigned short ResetTimerCounter;

void ResetFuseProcess();



