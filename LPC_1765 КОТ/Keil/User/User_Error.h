#ifndef USER_ERROR_H
#define USER_ERROR_H

#define ERR_MEM_SENS_FILE_STR	1
#define ERR_MEM_IVENT_FILE_STR	2

#define ERR_INCORRECT_SENS_FILE_STR	3
#define ERR_INCORRECT_IVENT_FILE_STR	4
#define ERR_FILE_STR            5
#define ERR_FILE_MEM            6
#define ERR_TIMER_FILE            7
#define ERR_AG_FILE               8


void AddError(uint8_t text);


#endif
