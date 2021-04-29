#include "Config.h"

#include <string.h>
#include "PPPOverDevTask.h"
#include "ppp/ppp.h"
#include "DriversCore.h"
#include "User_Timers.h"
#include "Timers.h"
#include "SetParametrs.h"

xTaskHandle hDevPPPTask;
DEVICE_HANDLE hDevice = 0;

static uint8_t dev_valid(void)
{
	uint8_t flag = 0;
	DeviceIOCtl(hDevice, DEVICE_VALID_CHECK, (void *)&flag);
	return flag;
}

static void vDevPPPStatusCB( void *ctx, int err, void *arg )
{
    if(err == PPPERR_NONE)
	{

        DEBUG_PUTS("lwIP: vDevPPPStatusCB: new PPP connection established\r\n" );

    }				   
	else
	{

        DEBUG_PRINTF_ONE("lwIP: vDevPPPStatusCB: PPP connection died ( err = %d )\r\n", err );
	}	
}
			 
uint8_t buff_r[20];
static int modem_immitation(void)
{
	WriteDevice(hDevice, "CLIENT\r", 7);
	vTaskDelay(3000);

	ReadDevice(hDevice, buff_r, 20);

	if(strncmp((char *)buff_r, "CLIENTSERVER", 12) == 0)
	{
		memset((char *)buff_r, 0, 20);
		return 0;
	}
	else
		return -1;
}


void DevPPPTask(void *pvParameters)
{
	int ppp_pd = -1;
	hDevice = OpenDevice(pvParameters);

	for(;;)
	{
		if(dev_valid() == 0)
		{
			if(ppp_pd >= 0)				//  Если сессия еще целая
			{							//  Закрываем ее полностью
				pppSigHUP(ppp_pd);
				ppp_pd = -1;
			}
				DeviceIOCtl(hDevice, DEVICE_PRE_CLOSE, 0);
				CloseDevice(hDevice);
				hDevice = 0;
#ifndef _FM_2200_
				ReadSettingsFile(PARAMETRS_FILE_NAME);
				SetTimerParam();
				ParametrsMemFree();
#endif
				vTaskDelete(&hDevPPPTask);
				
		}
#ifndef _FM_2200_
		load_timer(&SMS_Timer, 1200000);
	#ifndef _PIPELINES_
		load_timer(&Send_Data_Timer, 1200000);
	#endif
#endif
		if(ppp_pd < 0)
		{
			if(modem_immitation() == 0)
			{
				pppSetAuth( PPPAUTHTYPE_NONE, NULL, NULL );
				ppp_pd = pppOverSerialOpen(hDevice ,vDevPPPStatusCB, NULL, 0);	
			}
			
		}
		else 
		{
			if(PPP_Pool(ppp_pd, 400) == -1)	  // Если ppp разорвалось из вне
			{
				pppSigHUP(ppp_pd);			  // Полностью закрываем сессию
				ppp_pd = -1;
			}
		}

		DeviceIOCtl(hDevice, POOL, 0);
	}

}


