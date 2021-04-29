#include "Config.h"
#if(_MODEM_)
#include "ppp/ppp.h"
#include "lwip/opt.h"
#include "lwIP_Port.h"



#include "MODEM_DRV.h"


#define PPP_AUTH_ENABLED        1

int             ppp_con_fd;
extern xSemaphoreHandle xMODEM_Sem;
extern MODEMData modemDate;
static uint8_t  *login;
static uint8_t  *password;


int ppp_new_con( void )
{
//    ePPPThreadControl ePPPThrCtlCur;
    //int             ppp_con_fd;
//    MODEM_INFO *pModemInfo = (MODEM_INFO *)(((DEVICE *)ppp_fd)->pDriverData);
    MODEMData *pModemData = &modemDate;

    login = pModemData->Login;
    password = pModemData->Password;

//    portTickType    xTicks;

    pppInit();

    if( PPP_AUTH_ENABLED )
    {
      pppSetAuth( PPPAUTHTYPE_PAP, (char *)login, (char *)password );
    }
    else
    {
      pppSetAuth( PPPAUTHTYPE_NONE, NULL, NULL );
    }

    vPortEnterCritical(  );
    ePPPThrCtl = CONNECTING;
    vPortExitCritical(  );
    ppp_con_fd = pppOverSerialOpen(NULL ,vPPPStatusCB, NULL );

	return 0;

}

void ClosePPPCon(void)
{
    pppClose(ppp_con_fd);
}


#endif

