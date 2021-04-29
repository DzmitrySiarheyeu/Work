#pragma once

#include "util.h"
#include "errors_led.h"
#include "dot.h"

#define TIMER0_CLOCK_DIVIRDER_1     1
#define TIMER0_CLOCK_DIVIRDER_8     2
#define TIMER0_CLOCK_DIVIRDER_64    3
#define TIMER0_CLOCK_DIVIRDER_256   4
#define TIMER0_CLOCK_DIVIRDER_1024  5

//-------------SET--------------------

#define ROUT_TIMER_CLOCK_DIVIRDER   TIMER0_CLOCK_DIVIRDER_8

// AIN_U Divider's resistors
#define AIN_U_HIGH_R    100000
#define AIN_U_LOW_R     10000

// R25
#define AIN_I_R         1
// DA5  Amp koeficient
#define AIN_I_AMP_K     21//10.591

#define DELTA_R         0.5

#define ROUT_PWM_MIN         0
#define ROUT_PWM_MAX         255
#define ROUT_PWM_STEP        0.05

//  R ERROR INDICATOR 

#define LOW_R_ERROR_TOLERANCE	5	// in Ohms
#define HIGH_R_ERROR_TOLERANCE	0.2	// in percents 0 .. 1

//-------------SET--------------------
#define R_TYPE_90	0
#define R_TYPE_350	1
#define R_TYPE_800	2

// SetR return values
#define     SET_R_OK            0
#define     SET_R_ERROR         1   //can't increase R

#if (ROUT_TIMER_CLOCK_DIVIRDER == TIMER0_CLOCK_DIVIRDER_1)
	#define TIMER0_CLOCK 1
#endif
#if (ROUT_TIMER_CLOCK_DIVIRDER == TIMER0_CLOCK_DIVIRDER_8)
	#define TIMER0_CLOCK 8
#endif
#if (ROUT_TIMER_CLOCK_DIVIRDER == TIMER0_CLOCK_DIVIRDER_64)
	#define TIMER0_CLOCK 64
#endif
#if (ROUT_TIMER_CLOCK_DIVIRDER == TIMER0_CLOCK_DIVIRDER_256)
	#define TIMER0_CLOCK 256
#endif
#if (ROUT_TIMER_CLOCK_DIVIRDER == TIMER0_CLOCK_DIVIRDER_1024)
	#define TIMER0_CLOCK 1024
#endif

void ROutInit(void);
unsigned char ROutProcess();


