#include <stdint.h>
#include "vout.h"
#include "lpc11xx.h"
#include "gpio.h"
#include "freq.h"



//----------------- V OUT TIMER1-------------
inline void VOutInit()
{
	LPC_IOCON->R_PIO1_1 |= 0x83UL;
	LPC_TMR32B1->MR3 = TIMER_TOP;
	LPC_TMR32B1->MR0 = TIMER_TOP;
	LPC_TMR32B1->TC = TIMER_TOP-2;
	LPC_TMR32B1->PWMC |= (1 << 0);
}


inline void VOutProcess()
{
    float k = K_OUT;
    float U = 0;

	U = Reverse4(RegFile.mb_struct.U_max) * VolumePercent;

	LPC_TMR32B1->MR0 = (uint32_t)((float)TIMER_TOP - (float)TIMER_TOP * (U / (k * VCC)));
}

