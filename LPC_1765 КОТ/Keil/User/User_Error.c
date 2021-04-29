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
			// ������������� ���� ������ �������������
			initErrorFlag = 1;
            PUT_MES_TO_LOG("�������: ������ ������", 0, 0);
            break;
        case ERR_MEM_IVENT_FILE_STR:
			// ������������� ���� ������ �������������
			initErrorFlag = 1;
            PUT_MES_TO_LOG("�������: ������ ������", 0, 0);
            break;
        case ERR_INCORRECT_SENS_FILE_STR:
			// ������������� ���� ������ �������������
			initErrorFlag = 1;
            PUT_MES_TO_LOG("�������: ������ �����", 0, 0);
            break;
        case ERR_INCORRECT_IVENT_FILE_STR:
			// ������������� ���� ������ �������������
			initErrorFlag = 1;
            PUT_MES_TO_LOG("�������: ������ �����", 0, 0);
            break;
        case ERR_FILE_STR:
            PUT_MES_TO_LOG("FAT: ������ �����", 0, 0);
            break;
        case ERR_FILE_MEM:
            PUT_MES_TO_LOG("FAT: ������ ������", 0, 0);
            break;
        case ERR_TIMER_FILE:
            PUT_MES_TO_LOG("������: ������ �����", 0, 0);
            break;
        case ERR_AG_FILE:
            PUT_MES_TO_LOG("AUTOGRAPH: file error", 0, 0);
            break;
    }

   GPIO_ClearValue(1, 1 << 19); //�������� ERROR_LED
}
