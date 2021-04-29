#ifndef USER_TIMERS_H
#define USER_TIMERS_H
#include "Timers.h"

#define     PPP_DATA_NO_DATA     20
#define     WDT_RESET_TIMER      21


#define     MAX_NUMBER_TIMERS    10


extern void QueueTimersProc(void);
void InitTimers(void);
void InitEventTimers(void);
void ResetClockTimer(void);
void SetTimerParam(void);

extern TIMERS Led_Timer;
extern TIMERS Event_Timer;
extern TIMERS SIG_Check_Timer;
extern TIMERS SMS_Timer;
extern xQueueHandle timers_queue;

#endif


