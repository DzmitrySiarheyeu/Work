#include "Config.h"
#if _STATS_
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <LPC17xx.h>
#include "All_Stats.h"
#include "DriversCore.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_clkpwr.h"


static int ReadSTATSText(DEVICE_HANDLE handle, void * dst, int count);
static int SeekSTATSText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int CreateSTATS(void);
static DEVICE_HANDLE OpenSTATS(void);
static void CloseSTATS(DEVICE_HANDLE handle);


void vGetFreeMem(uint32_t * xMaxBlock, uint16_t * xTotalMem);

STATSData  statsDate = {
    .DataPointText = NULL,
    };

const DEVICE_IO stats_device_io = {
    .DeviceID = STATS,
	.CreateDevice =	CreateSTATS,
	.CloseDevice = CloseSTATS,
    .OpenDevice = OpenSTATS,
    .ReadDevice = NULL,
    .WriteDevice = NULL,
    .SeekDevice = NULL,
    .ReadDeviceText = ReadSTATSText,
    .WriteDeviceText = NULL,
    .SeekDeviceText = SeekSTATSText,
    .DeviceIOCtl = NULL,
    .DeviceName = "STATS",
    };


DEVICE stats_device = {
    .device_io = (DEVICE_IO *)&stats_device_io,
    .flDeviceOpen = 0,
    .pDriverData = NULL,
    .pData = &statsDate
    };


int ReadSTATSText(DEVICE_HANDLE handle, void * pDst, int count)
{
	uint32_t max = 0;
	uint16_t total = 0;
    STATSData *pData = (STATSData *)(((DEVICE *)handle)->pData);
    char *BufferText;
	int sprintf_count;
	
	if(count >= 0)
	{
		memset(pDst, 0, count);
		BufferText = pvPortMalloc(300);
		if(BufferText != NULL)
		{ 
			vGetFreeMem(&max, &total);
            vTaskList((signed char *)BufferText);
			sprintf_count = strlen(BufferText);
			sprintf_count += sprintf(BufferText + sprintf_count, "\n Memory free max %5u\n", max);
			sprintf_count += sprintf(BufferText + sprintf_count, "Memory total %5u", total);

//             vTaskGetRunTimeStats((signed char *)BufferText);
			if((pData->DataPointText + count) > sprintf_count)
			{
				count = sprintf_count - pData->DataPointText;
			}
			memcpy(pDst, (char *)(((uint8_t *)BufferText) + pData->DataPointText), count);
			pData->DataPointText += count;
			vPortFree(BufferText);
            return count;
        }
	}
    return -1;
}


int SeekSTATSText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
        STATSData *pData = (STATSData *)(((DEVICE *)handle)->pData);
        switch(origin)
        {
            case 0:
                if(offset < MAX_STATS_DATA_TEXT_OFFSET)
                {
                    pData->DataPointText = offset;
                }
                else
                {
                    pData->DataPointText = 0;
                }
                break;
            case 1:
                if((pData->DataPointText + offset) < MAX_STATS_DATA_TEXT_OFFSET)
                {
                    pData->DataPointText = pData->DataPointText + offset;
                }
                else
                {
                    pData->DataPointText = 0;
                } 
                break;
            case 2:
                if(offset < MAX_STATS_DATA_TEXT_OFFSET)
                {
                    pData->DataPointText = MAX_STATS_DATA_TEXT_OFFSET - offset;
                }
                else
                {
                    pData->DataPointText = 0;
                } 
                break;
            default:
                pData->DataPointText = 0;
                return 0;                
        }
        return pData->DataPointText;
}


int CreateSTATS(void)
{
	DEBUG_PUTS("Stats Created\n");
    return DEVICE_OK;
}

DEVICE_HANDLE OpenSTATS(void)
{
    if(stats_device.flDeviceOpen == DEVICE_CLOSE)
	{
		 stats_device.flDeviceOpen = DEVICE_OPEN;
	}
	else
	{
		  DEBUG_PUTS("Stats RE_OPEN\n");
	}
	return (DEVICE_HANDLE)&stats_device;

} 

void CloseSTATS(DEVICE_HANDLE handle)
{
	if(stats_device.flDeviceOpen == DEVICE_OPEN)
	{
		 stats_device.flDeviceOpen = DEVICE_CLOSE;
	}
	else
	{
		  DEBUG_PUTS("Stats RE_CLOSE\n");
	}
}

void vConfigureTimerForRunTimeStats( void )
{

    TIM_TIMERCFG_Type TIM_ConfigStruct;

    TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_TICKVAL;
    TIM_ConfigStruct.PrescaleValue = 100000;
/*
    TIM_MatchConfigStruct.MatchChannel = 0;
    TIM_MatchConfigStruct.IntOnMatch   = FALSE;
    TIM_MatchConfigStruct.ResetOnMatch = FALSE;
    TIM_MatchConfigStruct.StopOnMatch  = FALSE;
    TIM_MatchConfigStruct.ExtMatchOutputType =TIM_EXTMATCH_TOGGLE;
    TIM_MatchConfigStruct.MatchValue   = 0;
*/

    TIM_Init(LPC_TIM1, TIM_TIMER_MODE,&TIM_ConfigStruct);

  //  TIM_ConfigMatch(LPC_TIM1,&TIM_MatchConfigStruct);

    TIM_ResetCounter(LPC_TIM1);

   NVIC_SetPriority(TIMER1_IRQn, ((0x01<<3)|0x01));
   NVIC_EnableIRQ(TIMER1_IRQn);

   TIM_Cmd(LPC_TIM1,ENABLE);


    CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER1, CLKPWR_PCLKSEL_CCLK_DIV_1);

}


uint32_t Get_Stat_tick(void)
{
    return LPC_TIM1->TC;
}

#endif
