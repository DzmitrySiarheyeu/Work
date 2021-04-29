#include "Config.h"
#if(_SENSORS_)
#ifndef DATAFUNC_H
#define DATAFUNC_H

#include <stdint.h>

#define SLOW_FREQ_DATA     1
#define     CLOCK_SENS_FUNC_PARAM   "0;4294967296;0.00028;0;"


#define	SENS_SAVE_TIME		((1000 / SENSORS_POOL_TIMER_INTERVAL)*SAVE_SENSOR_DATA_TIME)

#define LENGTH_DATA_FUNC_BUF     50
#define MAX_DATA_FUNC_NAME       10

#define EXP_BUF        (1*4)
#define EXP_DATA       (1*4)
#define ADC_EXP_DATA   (2*4 + 1)
#define DISCRETE_DATA  (6)
#define DISCRETE_BUF   (2+2+2)

#define IMP_BUFFER    (1*4)

typedef struct tagMean
{
    uint32_t CurrentValue;
    uint16_t in;
    uint32_t array[];
}MEANINFO;

typedef struct tagDataFuncList
{
    uint8_t NumberOfItems;
    struct tagDataFunc *pFirstFunc;
}DATA_FUNC_LIST;

typedef struct 
{
    uint8_t not_first_call;
    uint8_t pool_flag;
	uint8_t ch_num;
	uint8_t min;
	uint8_t sec;
	float	hour_value;
	uint32_t save_counter;
} __attribute__ ((__packed__)) CL_COUNT_INFO;

typedef struct 
{
	uint32_t current_counter;
	uint32_t save_time_counter;
	uint32_t previous_counter;
} __attribute__ ((__packed__)) IMP_COUNT_INFO;

typedef struct tagDataFunc
{
    SENSOR_PROC_FUNC *Addr;
    INIT_PROC *Init_Proc;
    char Name[MAX_DATA_FUNC_NAME];
    struct tagDataFunc *pNext;
    DATA_FUNC_LIST *pFuncList;
}DATA_FUNC;

#define       EQUAL     0
#define       MORE      1
#define       LESS      2
#define       E_MORE    3
#define       E_LESS    4

void InitDataFunctions(void);
SENSOR_PROC_FUNC * GetDataFuncAddr(char *name);
INIT_PROC * GetInitDataFuncAddr(char *name);
void GetDataFuncName(SENSOR_PROC_FUNC addr, char *data);
/*
void SetFlagClockSens(void);
void ClrFlagClockSens(void);
void ChangeFlagClockSens(void);
uint8_t GetFlagClockSens(void);
*/
#endif
#endif



