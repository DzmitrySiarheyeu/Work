#ifndef CLOCK_H
#define CLOCK_H
#include "Config.h"
#if _CLOCK_ 
#include "DriversCore.h"


#define MAX_CLOCK_DATA_TEXT_OFFSET    74
#define MAX_CLOCK_DATA_OFFSET         0x17
#define ADR_CLOCK                     0xD0


typedef struct tagCLOCK_INFO
{
      DEVICE_HANDLE *pI2C;
	  uint32_t    	DataPoint;
}CLOCK_INFO;

typedef struct __attribute__ ((__packed__)) tagCLOCKData
{
    uint8_t		tenth_Sec;
    uint8_t		Sec;
    uint8_t		Min;
    uint8_t		Hour;
    uint8_t		Day;
    uint8_t		Date;
    uint8_t		Month;
    uint8_t		Year;
    uint8_t     Century;
	uint64_t	TimeStamp;
    uint32_t    DataPointText;
	int    		Hour_delta;
	uint8_t 	Res_Hour;
	uint8_t 	Res_Min;
	uint8_t 	reset_flag;
}CLOCKData;


/*--------------------”казатели на данные в структуре данных------------------------*/

#define     SEC_D	      1
#define     MIN_D	      2
#define     HOUR_D	      3
#define     DAY_D	      4
#define     DATE_D            5
#define     MONTH_D           6
#define     YEAR_D            7	  
#define     CENTURY_D         8
#define     CONTROL_D         8

#define     POOL_CLOCK        9

#define     TIME_SYNC_GPS     11
#define     TIME_SYNC_GSM     12
#define     TL_TIME_STAMP     13


int PoolClock(void);
void Init_clock(void);



#endif  // _CLOCK_

#endif // CLOCK_H

