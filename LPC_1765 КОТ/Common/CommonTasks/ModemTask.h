#ifndef MODEM_TASK_H
#define MODEM_TASK_H

#include "main.h"


#define		SMS_PERFORM			0
#define		MODEM_RESTART		1
#define		SMS_CHECK			7
#define		SIG_LEVEL_CHECK		SIG_CHECK_TIMER
#define		SMS_SEND   			4


typedef struct tagMessageModem 
{
    uint8_t ID;
    uint32_t par_1;
	uint32_t par_2;
	xQueueHandle queue;
    uint8_t *privat_data;
}MODEM_MESSAGE;

int PutMesToModem(uint8_t id, uint32_t p_1, uint32_t p_2, uint8_t *data, xQueueHandle queue, int timeout);
void ModemTask(void *pvParameters);

extern xQueueHandle modem_queue;
extern xTaskHandle hModemTask;


#endif

