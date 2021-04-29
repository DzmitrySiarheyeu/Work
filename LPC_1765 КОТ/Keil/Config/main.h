#ifndef MAIN_H
#define MAIN_H

#include "Config.h"
#include <stdint.h>
#include "DriversCore.h"

#define   WDT_VALUE    0x02AEA540

typedef struct tagMessageCommon
{
    uint8_t dst;
    uint8_t *privat_data;
    uint16_t data;
}COMMON_MESSAGE;

typedef struct tagResetTaskMes
{
    char *data;
}RESET_TASK_MES;

typedef struct tagProfile
{
    uint8_t def_prof;
	uint8_t cur_prof;
	uint8_t prof_0;
	uint8_t prof_1;
	uint8_t prof_2;
	uint8_t prof_3;
}PROFILE_INFO;

#define		SMS_RESET_MES			"SMS RESET"
#define		MODEM_RESET_MES			"MODEM RESET"
#define		USER_TASK_RESET_MES		"task U inv"
#define		MODEM_TASK_RESET_MES	"task M inv"
#define		RESET_TASK_RESET_MES	"task R inv"

#define		USER_TASK_WDT_CNT		0
#define		MODEM_TASK_WDT_CNT		1
#define		RESET_TASK_WDT_CNT		2

#define		USER_TASK_WDT_LIM		300
#define		MODEM_TASK_WDT_LIM		300
#define		RESET_TASK_WDT_LIM		300

typedef void(*CHECK_FUNK)(DEVICE_HANDLE handle);
typedef void(*SMS_FUNK)();
typedef void(*SYNC_TIME_FUNK)();
typedef void(*SIG_LEV_FUNK)();
typedef void(*DATA_SEND_FUNK)();

int PutMesToComQueue(uint8_t dst, uint8_t *privat_data, uint16_t data, uint32_t timeout);

//  Определения для общей очереди



#define KEY_OFF       0
#define KEY_ON_100ms  1
#define KEY_ON_2000ms 2

uint32_t GetNumericIP(char *buf);
uint16_t GetNumericPort(char *buf);

#endif


