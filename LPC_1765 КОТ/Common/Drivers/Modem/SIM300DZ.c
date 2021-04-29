#include "MODEM_DRV.h"

#if(_MODEM_)
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uart_lpc17xx.h"

#include "SetParametrs.h"
#include "User_Error.h"

MODEM_INFO modem_info = { .pUart = NULL};

int CreateModem(void)
{
	CONFIG_STATUS();
	CONFIG_STATUS_PIN_MODE();
	CONFIG_POWER_ON();
	POWER_ON_KEY_UP();

	CONFIG_F_SIM();
	CONFIG_S_SIM();
	CONFIG_SS();

	CONFIG_DTR();
	MODEM_DTR_OFF();

#if EXT_POW_OFF
	CONFIG_EXT_POW_ON();
	EXT_POW_KEY_UP();
#endif
 /*
    if(IS_S_SIM_NO_PRESENT())
    {
        F_SIM_ON();
    }
    else if(IS_F_SIM_NO_PRESENT())
    {
        S_SIM_ON();
    }
 */
    init_modem();

	ReadSettingsFile(PARAMETRS_FILE_NAME);
    SetModemSettings(0);
	ParametrsMemFree();
	
	DEBUG_PUTS("MODEM Created\n");
	return DEVICE_OK;
}

#endif

