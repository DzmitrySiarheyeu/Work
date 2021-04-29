#include "Config.h"
#if(_MODEM_)

#include <string.h>
#include "ModemTask.h"
#include "MODEM_DRV.h"
#include "ppp/ppp.h"
#include "lwip/tcpip.h"
#include "lwip/inet.h"
#include "WDT.h"
#include "ping.h"
#include "sms.h"
#include "PPPOverDevTask.h"
#include "User_Timers.h"
#if(_GPS_)
	#include "GPS_DRV.h"
#endif
#include "SensorCore.h"

xQueueHandle modem_queue = NULL;
xTaskHandle hModemTask;	
void AwakeResetTask(int isr_fl, char *data);
void ResetGPSData(void);

static struct ppp_addrs *ppp_addrs = 0;


void vModemPPPStatusCB( void *ctx, int err, void *arg );

#if (defined (FM1100) || defined (_FM_2200_) || defined (_FM_3200_))
	static uint8_t check_usb_con(void)
	{
		uint8_t flag = 0;
		DEVICE_HANDLE hUSB = OpenDevice("USBCDC");
		DeviceIOCtl(hUSB, DEVICE_VALID_CHECK, (void *)&flag);
		CloseDevice(hUSB);
		return flag;
	}
#endif
//void USBConfigureInterface(void * param);
int usb_con_flag = 0;		 
extern int sup_flag;
//void PSTN_Response_Available(void) ;
//int USB_Pstn_Init(void) ;
//void USBInterfaceDown(void);


void ModemTask(void *pvParameters)
{
	uint8_t m_s = 0;
	uint8_t sms_cnt = 0;
	MODEM_MESSAGE 	message;
	char  *login = modemDate.Login;
	char  *password = modemDate.Password;
	int ppp_pd = -1, err = 0;
	int loop_fl = 1;
	int modem_online_fl = 0;
	DEVICE_HANDLE hModem = OpenDevice("MODEM");
	SENSOR_HANDLE pSensor = NULL;
	float ext_pow = 0, pow_lim = 0.0;
	modem_queue = xQueueCreate(5, sizeof(MODEM_MESSAGE));
	sys_sem_new(&modem_info.sem, 1);
	pppInit();

	for(;;)
	{

	WDT_cnt[MODEM_TASK_WDT_CNT].cnt = 0;

#if (!_FM_2200_ && !defined (FM1100) && !defined (_FM_3200_))

				
#else

				if(check_usb_con() == 1){	
					if(hDevPPPTask == NULL){
						delay(2000);
						xTaskCreate( DevPPPTask, "DevPPP", configMINIMAL_STACK_SIZE + 64, "USBCDC", DEVICE_TASK_PRIORITY_LOW, &hDevPPPTask);
					}
				}
#endif

			 
		 //DeviceIOCtl(hModem, MODEM_AUTOMATE_CTL, NULL);			 //  Отрабатывает автомат модема
		 m_s = Modem_state_get();
	
		 switch(m_s)
		 {
		 	case GPRS_ON:
				if(login[0] == 0 || password[0] == 0)
					pppSetAuth( PPPAUTHTYPE_PAP, NULL, NULL );
				else														  //  Если только подключились к GPRS запускаем PPP соединение
					pppSetAuth( PPPAUTHTYPE_PAP, (char *)login, (char *)password );
				ppp_pd = pppOverSerialOpen(modem_info.pUart ,vModemPPPStatusCB, NULL, modem_info.sem);
				if(ppp_pd >= 0)  
				{
					Modem_state_set(ON_LINE);
					m_s = ON_LINE;
					ResetPing();
				}
				else // Капец, такого не может быть, перезагрузка устройства
				{
						PUT_MES_TO_LOG("PPP: unable ppp open", 0, 1);
						AwakeResetTask(0, MODEM_RESET_MES);
				}
				break;
			case PPP_ON:													  // Если PPP подключается или уже установлено.
			case ON_LINE:
				do{
					err = PPP_Pool(ppp_pd, PPP_RX_SIZE/2);
				}while(err > 0);
				if(err == -1)
				{
					//pppSigHUP(ppp_pd);
					ppp_pd = -1; 
					//modem_online_fl = take_modem_from_online();
					//if(modem_online_fl == -1)
					Modem_state_set(ERR);
				}
				if(IsPingOk() == 0)	  Modem_state_set(ERR);                      //  Если пинг не проходит разрываем соединение.
				break;
			default:
				if(ppp_pd >= 0)
				{
					pppSigHUP(ppp_pd);
					ppp_pd = -1;
				}
		 } 

	
		 //if(xQueueReceive(modem_queue, (void *)&message, 0))
		
		pow_lim = GetExtPowLim();
		pSensor = OpenSensorProc(SUPPLY_SENSOR);
		if(ReadSensor(pSensor , (uint8_t *)&ext_pow, 1) == -1)	
			ext_pow = 100;
		if(IS_QNaN(ext_pow))
			ext_pow = 0;

		if(ext_pow < pow_lim)
		{
			if(ppp_pd >= 0){
				pppSigHUP(ppp_pd);
				ppp_pd = -1;
			}
			if(Modem_state_get() != TURN_OFF)
			{
				PUT_MES_TO_LOG("MODEM OFF: Power bad", 2, 0);
			}
			Modem_state_set(TURN_OFF);
	#if(_GPS_)
			GPS_RST_ON();
			ResetGPSData();
	#endif
			delay(1000);
		}
		else if(ext_pow > (pow_lim + 0.7))
		{
			if(m_s == TURN_OFF)
			{
				PUT_MES_TO_LOG("MODEM ON: Power good", 2, 0);
				Modem_state_set(IDLE);
		#if(_GPS_)
				GPS_RST_OFF();
		#endif
			}
		} 		

		//if(uxQueueMessagesWaiting(modem_info.sem) == 0)
		//	PUT_MES_TO_LOG("MODEM SEM ERROR",0,0);
		
		xSemaphoreTake(modem_info.sem, portMAX_DELAY);
		 
		DeviceIOCtl(hModem, MODEM_AUTOMATE_CTL, NULL);			 //  Отрабатывает автомат модема
		loop_fl = 1;
		 while(xQueueReceive(modem_queue, (void *)&message, 0))
		 {
			modem_online_fl = take_modem_from_online();
			if(modem_online_fl == -1)
			{
				if(ppp_pd >= 0)
				{
					pppSigHUP(ppp_pd);
					ppp_pd = -1;
					Modem_state_set(ERR);
				}
				break;
			}
			switch(message.ID)
			{
				case SMS_PERFORM:
					if(sms_perform(message.par_1, (char *)message.privat_data, message.par_2) == -1){
						if(message.queue != NULL){
							if(message.privat_data != NULL)
								strcpy((char *)message.privat_data, "ERR SMS_PERFORM");
							xQueueSendToBack(message.queue, ( void * )&message, 0);
						}
					}
					else{
						if(message.queue != NULL)
							xQueueSendToBack(message.queue, ( void * )&message, 0);
					}
					break; 
				case MODEM_RESTART:
					DeviceIOCtl(hModem, RESET_DEVICE, NULL);
					AwakeResetTask(0, MODEM_RESET_MES);
					break;
				case SMS_SEND:
					if(m_s < 6 || m_s == TURN_OFF)	{
						xQueueSendToBack(modem_queue, ( void * )&message, 0);
						loop_fl = 0;
						break;
					}
					else
						SmsSend(message.par_1);
					break;
				case SMS_CHECK:
					if(m_s >= 6 && m_s != TURN_OFF)
						if(sms_proces() == -1){
							if(++sms_cnt < 10){ 
								xQueueSendToBack(modem_queue, ( void * )&message, 0);
							}
							else{
								sms_cnt = 0;
								Modem_state_set(ERR);
							}
							loop_fl = 0;
						}
					break;
				case SIG_LEVEL_CHECK:
						sig_level();
					break;
/*				case GPRS_TURN_OFF:
					Modem_state_set(PPP_CLOSING);
					break;
				case GPRS_TURN_ON:
					Modem_state_set(PPP_CLOSING);
					break; */
			}
			
			if(loop_fl == 0) break;	
		}
		if(modem_online_fl == 1)
		{
			if(give_modem_to_online() != 1)
			{
				if(ppp_pd >= 0)
				{
					pppSigHUP(ppp_pd);
					ppp_pd = -1;
					Modem_state_set(ERR);
				}
			}
			modem_online_fl = 0;
		}
		xSemaphoreGive( modem_info.sem );
	}
}


int PutMesToModem(uint8_t id, uint32_t p_1, uint32_t p_2, uint8_t *data, xQueueHandle queue, int timeout)
{
    MODEM_MESSAGE message;

    message.ID = id;
	message.par_1 = p_1;
	message.par_2 = p_2;
    message.privat_data = data;
	message.queue = queue;
    return xQueueSendToBack(modem_queue, ( void * )&message, (portTickType)timeout);
}


void vModemPPPStatusCB( void *ctx, int err, void *arg )
{
	ppp_addrs = arg;
    if(err == PPPERR_NONE)							   
	{

        DEBUG_PUTS("lwIP: vModemPPPStatusCB: new PPP connection established\r\n" );

		//Modem_state_set(PPP_ON);
#if(SEND_DATA_TIMER_ON)
		start_timer(&Send_Data_Timer);
#endif
#if(SIG_CHECK_ON)
	//	load_timer(&SIG_Check_Timer, 30000);
#endif
        load_timer(&Led_Timer, 500);

		PutMesToComQueue(SEND_DATA_TIMER, NULL, NULL, 5000);

        PUT_MES_TO_LOG("PPP CONNECT", 2, 0);


    }				   
	else
	{
		///Modem_state_set(ERR);


        PutMesToLog("PPP DISCONNECT", 2, 0);
        load_timer(&Led_Timer, 1000);
#if(SIG_CHECK_ON)
	//	load_timer(&SIG_Check_Timer, 10000);
#endif
#if(SEND_DATA_TIMER_ON)
        stop_timer(&Send_Data_Timer);
#endif
		ppp_addrs = 0;
        DEBUG_PRINTF_ONE("lwIP: vModemPPPStatusCB: PPP connection died ( err = %d )\r\n", err );

	}
}

void vlwIPInit( void )
{
    tcpip_init( NULL, NULL );
}


char ip_buf[100];

char * GetOurIPStr(void)
{
	if(ppp_addrs != 0){
		strcpy(ip_buf, inet_ntoa(*((struct in_addr *)&(ppp_addrs->our_ipaddr))));
		strcat(ip_buf, "\r\n");
#ifndef _PIPELINES_
		strcat(ip_buf, inet_ntoa(*((struct in_addr *)&(ppp_addrs->his_ipaddr))));
		strcat(ip_buf, "\r\n");
		strcat(ip_buf, inet_ntoa(*((struct in_addr *)&(ppp_addrs->netmask))));
		strcat(ip_buf, "\r\n");
		strcat(ip_buf, inet_ntoa(*((struct in_addr *)&(ppp_addrs->dns1))));
		strcat(ip_buf, "\r\n");
		strcat(ip_buf, inet_ntoa(*((struct in_addr *)&(ppp_addrs->dns2))));
		strcat(ip_buf, "\r\n");
#endif
		return ip_buf;
	}
		//return inet_ntoa(*((struct in_addr *)&(ppp_addrs->our_ipaddr)));
	else return 0;
}

#endif

