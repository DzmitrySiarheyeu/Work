#include "Config.h"
#include "User_Timers.h"
#include "User_Error.h"
#include "MODEM_DRV.h"
#include "main.h"
#include "ping.h"
#include "SetParametrs.h"
#include "ModemTask.h"
#include "WDT.h"
#include <stdlib.h>
			
extern uint16_t WDT_Task_Flag;
extern xQueueHandle common_queue;
//extern xQueueHandle MBS_queue;
extern xQueueHandle events_queue;

void set_param(void);
void proc_led(void);
void wdt_proc(void);

TIMERS Led_Timer = 
{
    .ID = LED_TIMER,
    .time = 1000,
    .status = LED_TIMER_ON,
    .t_func = &proc_led,
    .xQueue = &common_queue,
    .xNextTimer = 0
};

TIMERS SIG_Check_Timer = 
{
    .ID = SIG_CHECK_TIMER,
    .time = 10000,
    .status = SIG_CHECK_ON,
    .t_func = NULL,
    .xQueue = &modem_queue,
    .xNextTimer = 0
};

/*TIMERS MBS_Timer = 
{
    .ID = MBS_TIMER,
    .time = 300,
    .status = MBS_TIMER_ON,
    .t_func = NULL,
    .xQueue = &MBS_queue,
    .xNextTimer = 0
}; */

// !!!!!Когда меняется время пула датчиков необходимо поменять время в датчике QFunc  !!!!!!
TIMERS Sensors_Pool_Timer = 
{
    .ID = SENSORS_POOL_TIMER,
    .time = 100,
    .status = SENSORS_POOL_TIMER_ON,
    .t_func = NULL,
    .xQueue = &common_queue,
    .private_data = NULL,
    .xNextTimer = 0
};

TIMERS Display_Update_Timer = 
{
    .ID = DISPLAY_UPDTAE_TIMER,
    .time = 1000,
    .status = DISPLAY_UPDATE_TIMER_ON,
    .t_func = NULL,
    .xQueue = &events_queue,
    .private_data = NULL,
    .xNextTimer = 0
};

TIMERS Clock_Pool_Timer = 
{
    .ID = CLOCK_POOL_TIMER,
    .time = 500,
    .status = CLOCK_POOL_TIMER_ON,
    .t_func = NULL,
    .xQueue = &common_queue,
    .private_data = NULL,
    .xNextTimer = 0
};

TIMERS Discrete_Pool_Timer = 
{
    .ID = DISCRETE_POOL_TIMER,
    .time = 100,
    .status = DISCRETE_POOL_TIMER_ON,
    .t_func = NULL,
    .xQueue = &common_queue,
    .private_data = NULL,
    .xNextTimer = 0
};

TIMERS Reset_To_Clock_Timer = 
{
    .ID = RESETE_TO_CLOCK_TIMER,
    .time = 60000,
    .status = RESETE_TO_CLOCK_TIMER_ON,
    .t_func = NULL,
    .xQueue = &events_queue,
    .xNextTimer = 0
};

TIMERS SMS_Timer = 
{
    .ID = SMS_TIMER,
    .time = 300000,
    .status = SMS_TIMER_ON,
    .t_func = NULL,
    .xQueue = &modem_queue,
    .private_data = NULL,
    .xNextTimer = 0
};

TIMERS PING_Timer = 
{
    .ID = PING_TIMER,
    .time = 60000,
    .status = PING_TIMER_ON,
    .t_func = NULL,
    .xQueue = &events_queue,
    .xNextTimer = 0
};

TIMERS WDT_Timer = 
	{
	    .ID = WDT_TIMER,
	    .time = 1000,
	    .status = WDT_TIMER_ON,
	    .t_func = &WDTCountersInc,
	    .xQueue = NULL,
	    .xNextTimer = 0
	};

/*
TIMERS TIME_SYNC_Timer = 
{
    .ID = TIME_SYNC_TIMER,
    .time = 3600000,
    .status = TIME_SYNC_TIMER_ON,
    .t_func = NULL,
    .xQueue = &common_queue,
    .private_data = NULL,
    .xNextTimer = 0
};	
*/
void ResetClockTimer(void)	
{
	Reset_To_Clock_Timer.counter = 0;	
}


void InitTimers(void)
{
#if(LED_TIMER_ON)
    add_timer(&Led_Timer);
#endif
#if(SENSORS_POOL_TIMER_ON)
    add_timer(&Sensors_Pool_Timer);
#endif
/*#if(MBS_TIMER_ON)
    add_timer(&MBS_Timer);
#endif*/
#if(DISPLAY_UPDATE_TIMER_ON)
    add_timer(&Display_Update_Timer);
#endif
#if(DISCRETE_POOL_TIMER_ON)
	add_timer(&Discrete_Pool_Timer);
#endif	
#if(CLOCK_POOL_TIMER_ON)
	add_timer(&Clock_Pool_Timer);
#endif
#if(RESETE_TO_CLOCK_TIMER_ON)
	add_timer(&Reset_To_Clock_Timer);
#endif
#if(SMS_TIMER_ON)
	add_timer(&SMS_Timer);
#endif
#if(SIG_CHECK_ON)
	add_timer(&SIG_Check_Timer);
#endif
//#if(TIME_SYNC_TIMER_ON)
//    add_timer(&TIME_SYNC_Timer);
//#endif
#if(PING_TIMER_ON)
    add_timer(&PING_Timer);
#endif
#if(_WDT_)
	#if(WDT_TIMER_ON)
	    add_timer(&WDT_Timer);
	#endif
#endif

    SetTimerParam();
}


void SetTimerParam(void)
{
    uint8_t *p_data;

    if(GetSettingsFileStatus())
    {
#if(PING_TIMER_ON) 
        p_data = (uint8_t *)GetPointToElement("TIMERS", "Ping");
        if(p_data == 0)
        {
             PING_Timer.time = PING_DEF_TIME;
			 AddError(ERR_TIMER_FILE);
        }
        else
        {
            PING_Timer.time = atol((char *)p_data);
			if(PING_Timer.time < PING_MIN_TIME) PING_Timer.time = PING_MIN_TIME;
			if(PING_Timer.time > PING_MAX_TIME) PING_Timer.time = PING_MAX_TIME;
        }											
#endif


#if(SMS_TIMER_ON) 
        p_data = (uint8_t *)GetPointToElement("TIMERS", "Sms");
        if(p_data == 0)
        {
            SMS_Timer.time = SMS_CHECK_DEF_TIME;
			AddError(ERR_TIMER_FILE);
        }
        else
        {
            SMS_Timer.time = atol((char *)p_data);
			if(SMS_Timer.time < SMS_CHECK_MIN_TIME) SMS_Timer.time = SMS_CHECK_MIN_TIME;
			if(SMS_Timer.time > SMS_CHECK_MAX_TIME) SMS_Timer.time = SMS_CHECK_MAX_TIME;
        }
#endif

	
    }
    else AddError(NULL);

}

