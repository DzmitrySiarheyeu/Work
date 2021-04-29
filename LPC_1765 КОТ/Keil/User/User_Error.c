#include <stdint.h>
#include "User_Error.h"
#include "main.h"
#include "Config.h"
#include "FreeRTOS.h"

#include "lpc17xx_gpio.h"

extern uint8_t initErrorFlag;

void AddError(uint8_t text)
{
    switch(text)
    {
        case ERR_MEM_SENS_FILE_STR:
			// устанавливаем флаг ошибки инициализации
			initErrorFlag = 1;
            PUT_MES_TO_LOG("Датчики: ошибка памяти", 0, 0);
            break;
        case ERR_MEM_IVENT_FILE_STR:
			// устанавливаем флаг ошибки инициализации
			initErrorFlag = 1;
            PUT_MES_TO_LOG("События: ошибка памяти", 0, 0);
            break;
        case ERR_INCORRECT_SENS_FILE_STR:
			// устанавливаем флаг ошибки инициализации
			initErrorFlag = 1;
            PUT_MES_TO_LOG("Датчики: ошибка файла", 0, 0);
            break;
        case ERR_INCORRECT_IVENT_FILE_STR:
			// устанавливаем флаг ошибки инициализации
			initErrorFlag = 1;
            PUT_MES_TO_LOG("События: ошибка файла", 0, 0);
            break;
        case ERR_FILE_STR:
            PUT_MES_TO_LOG("FAT: ошибка файла", 0, 0);
            break;
        case ERR_FILE_MEM:
            PUT_MES_TO_LOG("FAT: ошибка памяти", 0, 0);
            break;
        case ERR_TIMER_FILE:
            PUT_MES_TO_LOG("Таймер: ошибка файла", 0, 0);
            break;
        case ERR_AG_FILE:
            PUT_MES_TO_LOG("AUTOGRAPH: file error", 0, 0);
            break;
    }

   GPIO_ClearValue(1, 1 << 19); //Зажигаем ERROR_LED
}
