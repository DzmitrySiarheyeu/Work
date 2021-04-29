#ifndef WDT_H
#define WDT_H

#include "config.h"


typedef struct tagWDTCounters
{
	uint32_t 		cnt;        //  —четчик
	uint32_t		lim;
	uint8_t			fl_on;
	char			mes[15];
}WDT_COUNTER;

extern WDT_COUNTER	WDT_cnt[WDT_COUNTERS_NUM];
void WDTCountersInc(void);

#if(_WDT_LPC_)   

void InitWDT(void);
void ResetWDT(void);

#endif // _WDT_

#endif // WDT_H

