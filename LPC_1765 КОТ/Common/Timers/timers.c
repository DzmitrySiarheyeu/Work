#include "Config.h"


#include "Timers.h"

//��. FREERTOS -> port.c -> void xPortSysTickHandler( void )
 
TIMERS *pFirstTimer = 0;
xQueueHandle timers_queue;

void add_timer(TIMERS *SysTimer)
{
    if(pFirstTimer == 0) //���� ��������� ������ ������
    {
        pFirstTimer = SysTimer;
        pFirstTimer->xNextTimer = 0;
    }
    else 
    {
        SysTimer->xNextTimer = pFirstTimer;
        pFirstTimer = SysTimer;
    }
}

uint32_t get_timer_value(TIMERS *SysTimer)
{
    return SysTimer->counter;
}

uint32_t get_timer_status(TIMERS *SysTimer)
{
    return SysTimer->status;
}


uint32_t get_rese_timer_value(TIMERS *SysTimer)
{
    return SysTimer->time;
}

//��������� ������
void start_timer(TIMERS *SysTimer)
{
    SysTimer->status = 1;
    SysTimer->counter = 0;

}

//������������� ������
void stop_timer(TIMERS *SysTimer)
{
    SysTimer->status = 0;
}

//���������� ����� �������� � ��
void load_timer(TIMERS *SysTimer, uint32_t load_value)
{
    if(load_value > 0) 
    {
        SysTimer->time = load_value;
        SysTimer->counter = 0;
    }
}

//���������� ������
void reset_timer(TIMERS *SysTimer)
{
    SysTimer->counter = 0;
}

//������������� �������



void inc_timers(void)
{
    TIMERS *pTimer;
    volatile TIMERS_MESSAGE message;
    //portBASE_TYPE Flag = pdTRUE;
	static signed portBASE_TYPE xHigherPriorityTaskWoken;

    pTimer = pFirstTimer;

    while(pTimer != 0)
    {
        if(pTimer->status == 1)
        {
            pTimer->counter++;
            if(pTimer->time != 0)
            { 
                //���� ������ �������� �� ��������
                if(pTimer->counter >= pTimer->time)
                {
                    //���������� � 0
                    pTimer->counter = 0;
                    //����� ID �������
                    message.TimerID = pTimer->ID;
                    //��������� ��������� � �������
                    if(pTimer->t_func)
                    {
                        (pTimer->t_func)();
                    }
                    else if(*(pTimer->xQueue))
                    {
                        xQueueSendToFrontFromISR(*(pTimer->xQueue), ( void * )&message, &xHigherPriorityTaskWoken);
                    }
                }
            }
        }
        pTimer = pTimer->xNextTimer;
    }
}

void vApplicationTickHook(void)
{
    inc_timers();
}

