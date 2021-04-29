#include "Config.h"
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "DriversCore.h"
#include "WDT.h"



xTaskHandle USERHandle;
void UserTask(void *pvParameters);

int main( void )
{

	InitWDT();
	
	SystemInit();
	
    xTaskCreate( UserTask, (const signed char *)"USER", 128+64, NULL, DEVICE_TASK_PRIORITY_LOW + 1, &USERHandle);

    vTaskStartScheduler();
	return 0;
}


uint32_t GetNumericIP(char *buf)
{
    uint32_t numeric_ip;
    uint8_t i = 0, k = 0, j = 0;
    char *ch;
    
    ch = buf;

    while(ch[i] != 0)
    {
        if(ch[i] == '.')
        {
            ch[i] = 0x00;
            ((uint8_t *)&numeric_ip)[j] = atoi(ch + k);
            j++;
            k = i + 1;
            if(j == 3)
            {
                ((uint8_t *)&numeric_ip)[j] = atoi(ch + k);
                return numeric_ip;
            }
        }
        i++;
    }
    return 0xFFFFFFFF;
}

uint16_t GetNumericPort(char *buf)
{
    uint16_t port;
    uint8_t temp;

    port = atoi(buf);

    temp = (port >> 8);
    port = (port << 8) | temp;

    return port;
}


