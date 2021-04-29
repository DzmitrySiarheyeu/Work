#include "Config.h"



#include "lwipopts.h"
#include "main.h"
#include "ppp/ppp.h"
#include "lwip/tcpip.h"
#include "lwip/inet.h"
#include "lwIP_Port.h"
#include "MODEM_DRV.h"

#include "FreeRTOS.h"
#include "task.h"
#if _LOG_
	#include "Log.h"
#endif

#include "User_Timers.h"

#if SEND_DATA_TIMER_ON
	extern TIMERS Send_Data_Timer;
#endif

ePPPThreadControl ePPPThrCtl;

extern DEVICE_HANDLE hPPP;

char    *_inet_ntoa( u32_t n );
#if(_DATA_TEST_)
extern xTaskHandle DataHandle;
#endif
#if(_HTTP_)
extern xTaskHandle HTTPHandle;
#endif
#if(_FTP_)
extern xTaskHandle FTPHandle;
#endif
#if(_TEST_)
extern xTaskHandle TestHandle;
#endif
#if(_TELNET_)
extern xTaskHandle TelnetHandle;
#endif
#if(_ETH2COM_)
extern xTaskHandle ETH2COMHandle;
#endif
#if(_Telnet_ || _HTTP_ || _FTP_ || _ETH2COM_)
struct ip_addr ip_adr;
#endif

extern uint8_t aaa;

static struct ppp_addrs *ppp_addrs;

void vPPPStatusCB( void *ctx, int err, void *arg )
{
    ePPPThreadControl ePPPThrCtlNew;

    switch ( err )
    {

        /* No error. */
    case PPPERR_NONE:
        ePPPThrCtlNew = CONNECTED;
        ppp_addrs = arg;
        DEBUG_PUTS("lwIP: vPPPStatusCB: new PPP connection established\r\n" );

		Modem_state_set(PPP_ON);

		start_timer(&Send_Data_Timer);


			


       // DEBUG_PRINTF_ONE("lwIP: vPPPStatusCB: our IP address = %s\r\n",  inet_ntoa( ppp_addrs->our_ipaddr ) );
       // DEBUG_PRINTF_ONE("lwIP: vPPPStatusCB: his IP address = %s\r\n",  inet_ntoa( ppp_addrs->his_ipaddr ) );
       // DEBUG_PRINTF_ONE("lwIP: vPPPStatusCB:  netmask = %s\r\n",  inet_ntoa( ppp_addrs->netmask ) );

		ip_adr =  ppp_addrs->our_ipaddr;

        #if(_TIMERS_)
            load_timer(&Led_Timer, 500);
        #endif

#if(_LOG_)
        PUT_MES_TO_LOG("PPP CONNECT", 2, 0);
#endif

//        #if(_TELNET_)
//
//        	vTaskResume(TelnetHandle);
//
//        #endif

        break;
    default:
        ePPPThrCtlNew = DISCONNECT;

		Modem_state_set(ERR);


#if(_LOG_)
        PutMesToLog("PPP DISCONNECT", 2, 0);
#endif
        load_timer(&Led_Timer, 1000);
#if SEND_DATA_TIMER_ON
        stop_timer(&Send_Data_Timer);
#endif

		//////////////////////////////////////////////////////xSemaphoreTake(ModemSem, 0);

        DEBUG_PRINTF_ONE("lwIP: vPPPStatusCB: PPP connection died ( err = %d )\r\n", err );

        break;
    }
    vPortEnterCritical(  );
    ePPPThrCtl = ePPPThrCtlNew;
    vPortExitCritical(  );
}

void
vlwIPInit( void )
{
    tcpip_init( NULL, NULL );
}

char * GetOurIPStr(void)
{
	 return inet_ntoa(*((struct in_addr *)&(ppp_addrs->our_ipaddr)));
}



