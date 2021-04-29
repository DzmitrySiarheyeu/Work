#pragma once

#include "util.h"
#include "dot.h"

#define TIMER1_CLOCK_DIVIRDER_1     1
#define TIMER1_CLOCK_DIVIRDER_8     2
#define TIMER1_CLOCK_DIVIRDER_64    3
#define TIMER1_CLOCK_DIVIRDER_256   4
#define TIMER1_CLOCK_DIVIRDER_1024  5

//-------------SET--------------------

#define VOUT_TIMER_CLOCK_DIVIRDER   TIMER1_CLOCK_DIVIRDER_1

#define VOUT_TIMER_MAX_TIMER_VALUE  65535

#define K_4_5   3
#define K_12    3
#define K_20    3

//-------------SET--------------------

#define U_TYPE_4_5	0
#define U_TYPE_12	1
#define U_TYPE_20	2

void VOutInit();

void VOutProcess();

#if (VOUT_TIMER_CLOCK_DIVIRDER == TIMER1_CLOCK_DIVIRDER_1)
    #define TIMER1_CLOCK    F_CPU / 1
#endif

#if (VOUT_TIMER_CLOCK_DIVIRDER == TIMER1_CLOCK_DIVIRDER_8)
    #define TIMER1_CLOCK    F_CPU / 8
#endif

#if (VOUT_TIMER_CLOCK_DIVIRDER == TIMER1_CLOCK_DIVIRDER_64)
    #define TIMER1_CLOCK    F_CPU / 64
#endif

#if (VOUT_TIMER_CLOCK_DIVIRDER == TIMER1_CLOCK_DIVIRDER_256)
    #define TIMER1_CLOCK    F_CPU / 256
#endif

#if (VOUT_TIMER_CLOCK_DIVIRDER == TIMER1_CLOCK_DIVIRDER_1024)
    #define TIMER1_CLOCK    F_CPU / 1024
#endif
