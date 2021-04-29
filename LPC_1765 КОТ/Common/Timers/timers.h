#include "Config.h"

#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "semphr.h"

typedef void (*TIMER_FUNC)(void);

typedef struct tagMessage 
{
    uint8_t TimerID;
    uint8_t *privat_data;
    uint8_t data;
}TIMERS_MESSAGE;

typedef struct tagSysTimer
{
    uint8_t ID;
    volatile uint32_t time;
    volatile uint32_t counter;
    volatile uint8_t status;
    TIMER_FUNC t_func;
    uint16_t private_data;
    xQueueHandle *xQueue;
    struct tagSysTimer *xNextTimer;
}TIMERS;

extern TIMERS *pFirstTimer;
extern xQueueHandle timers_queue;

void add_timer(struct tagSysTimer *SysTimer);
void load_timer(struct tagSysTimer *SysTimer, uint32_t load_value);
void start_timer(struct tagSysTimer *SysTimer);
void stop_timer(struct tagSysTimer *SysTimer);
void reset_timer(TIMERS *SysTimer);
void inc_timers(void);
uint32_t get_timer_status(TIMERS *SysTimer);
uint32_t get_timer_value(TIMERS *SysTimer);
uint32_t get_rese_timer_value(TIMERS *SysTimer);


#endif

