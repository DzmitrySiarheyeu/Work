#ifndef CLOCK_H
#define CLOCK_H
#include "Config.h"
#if _CLOCK_RTC_ 
#include "DriversCore.h"


#define MAX_CLOCK_DATA_TEXT_OFFSET    65
#define MAX_CLOCK_DATA_OFFSET         8

typedef struct tagCLOCK_INFO
{
	uint32_t    	DataPoint;
}CLOCK_INFO;

typedef struct __attribute__ ((__packed__)) tagCLOCKData
{
    uint8_t		Sec;
    uint8_t		Min;
    uint8_t		Hour;
	uint8_t		DOW; // день недели [0;6]
    uint8_t		Date; // день мес€ца
	uint8_t		Day; // день в году
    uint8_t		Month;
    uint8_t		Year;
	uint8_t     Century;
	uint64_t	TimeStamp;
    uint32_t    DataPointText;
	int    		Hour_delta;
	uint8_t 	sync_flag;
	uint8_t 	Res_Hour;
	uint8_t 	Res_Min;
	uint8_t 	reset_flag;
}CLOCKData;



/*--------------------”казатели на данные в структуре данных------------------------*/

#define     SEC_D	      0
#define     MIN_D	      1
#define     HOUR_D	      2
#define     DOW_D         3
#define     DATE_D        4
#define     DAY_D	      5
#define     MONTH_D       6
#define     YEAR_D        7
#define     CONTROL_D     8

#define SET_CALIB_DIR	  0
#define SET_CALIB_VAL	  1
#define GET_CALIB_DIR	  2
#define GET_CALIB_VAL	  3
#define SET_GPREG0	  	  4
#define GET_GPREG0	  	  5
#define SET_GPREG1  	  6
#define GET_GPREG1	  	  7
#define TIME_SYNC_GPS     11
#define TIME_SYNC_GSM     12
#define TL_TIME_STAMP     13

int PoolClock(void);
void Init_clock(void);
void SaveClockDate(void);

#endif  // _CLOCK_

#endif // CLOCK_H

