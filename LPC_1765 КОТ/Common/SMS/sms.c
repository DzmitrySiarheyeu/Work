#include "Config.h"
#if(_SMS_)
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "sms.h"
#include "MODEM_DRV.h"
#include "main.h"
#include "User_Events.h"
#include "Events.h"
#include "SetParametrs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Security.h"

extern PROFILE_INFO ProfInfo;
void ProfileChange(void);

#if(_UART_CONTROL_TASK_)
	extern xQueueHandle UartControlQueue;
#endif
void sms_queue_proc(char *data, uint8_t max_data_len, uint8_t id, xQueueHandle q_handle);
void add_sms(SMS_HANDLE *new_sms);    ///  ƒобовл€ет новое сообщение в список сообщений
SMS_DATA sms_handling(char *buff, uint16_t count);
char * GetOurIPStr(void);

#ifdef SMS_TCP_CON
	int sms_con_tcp_proc(char *data);
#endif

#ifdef Pipelines_SMS
	uint8_t con[20];
#endif

#if(_AUTOGRAPH_)
	#include "Autograph.h"
#endif

// дл€ ответа из USER_UART
xQueueHandle SMSQueue;


SMS_INFO SMSInfo = {.buf = MODEM_INIT_STRING};
const SMS_HANDLE SMS_handles[] = {
#ifdef SMS_ALL_GPRS_SETTINGS
	{.ID = SMS_ALL_GPRS_SETTINGS, .name = "ALL_GPRS", .pNext = NULL}
#endif	
#ifdef SMS_TIME_SEND
	,{.ID = SMS_TIME_SEND, .name = "SENDTIME", .pNext = NULL}
#endif
#ifdef SMS_TIME_SAVE
	,{.ID = SMS_TIME_SAVE, .name = "SAVETIME", .pNext = NULL}
#endif
#ifdef SMS_TIME_SMS
	,{.ID = SMS_TIME_SMS, .name = "SMSTIME", .pNext = NULL}
#endif
#ifdef SMS_SERIAL
	,{.ID = SMS_SERIAL, .name = "SERIAL", .pNext = NULL}
#endif
#ifdef SMS_PASSWORD
	,{.ID = SMS_PASSWORD, .name = "PASSWORD", .pNext = NULL}
#endif
#ifdef SMS_PORT
	,{.ID = SMS_PORT, .name = "PORT", .pNext = NULL}
#endif
#ifdef SMS_IP
	,{.ID = SMS_IP, .name = "IP", .pNext = NULL}
#endif
#ifdef SMS_LOGGPRS_1
	,{.ID = SMS_LOGGPRS_1, .name = "LOGGPRS_1", .pNext = NULL}
#endif
#ifdef SMS_STRINIT_1
	,{.ID = SMS_STRINIT_1, .name = "APNGPRS_1", .pNext = NULL}
#endif
#ifdef SMS_PASSGPRS_1
	,{.ID = SMS_PASSGPRS_1, .name = "PASSGPRS_1", .pNext = NULL}
#endif
#ifdef SMS_LOGGPRS_2
	,{.ID = SMS_LOGGPRS_2, .name = "LOGGPRS_2", .pNext = NULL}
#endif
#ifdef SMS_STRINIT_2
	,{.ID = SMS_STRINIT_2, .name = "APNGPRS_2", .pNext = NULL}
#endif
#ifdef SMS_PASSGPRS_2
	,{.ID = SMS_PASSGPRS_2, .name = "PASSGPRS_2", .pNext = NULL}
#endif
#ifdef SMS_IDPROCE
	,{.ID = SMS_IDPROCE, .name = "IDPROCE", .pNext = NULL}
#endif
#ifdef SMS_SIM
	,{.ID = SMS_SIM, .name = "SIM", .pNext = NULL}
#endif
#ifdef SMS_BOOT_VERS
	,{.ID = SMS_BOOT_VERS, .name = "BOOT_VERSION", .pNext = NULL}
#endif
#ifdef SMS_VERS
	,{.ID = SMS_VERS, .name = "VERSION", .pNext = NULL}
#endif
#ifdef SMS_RESET
	,{.ID = SMS_RESET, .name = "RESET", .pNext = NULL}
#endif
#ifdef SMS_USSD
	,{.ID = SMS_USSD, .name = "USSD", .pNext = NULL}
#endif
#ifdef SMS_AN
	,{.ID = SMS_AN, .name = "AN_DATA", .pNext = NULL}
#endif
#ifdef SMS_FREQ
	,{.ID = SMS_FREQ, .name = "LLS_DATA", .pNext = NULL}
#endif
#ifdef SMS_COUNT
	,{.ID = SMS_COUNT, .name = "COUNT_DATA", .pNext = NULL}
#endif
#ifdef SMS_GPS
	,{.ID = SMS_GPS, .name = "GPS_DATA", .pNext = NULL}
#endif
#ifdef SMS_TEMPER
	,{.ID = SMS_TEMPER, .name = "TEMPER_DATA", .pNext = NULL}
#endif
#ifdef SMS_RING_UP
	,{.ID = SMS_RING_UP, .name = "RING_UP", .pNext = NULL}
#endif
#ifdef SMS_TIME_SYNC
	,{.ID = SMS_TIME_SYNC, .name = "TIME_SYNC", .pNext = NULL}
#endif
#ifdef SMS_TIME_DELTA
	,{.ID = SMS_TIME_DELTA, .name = "T_DELTA", .pNext = NULL}
#endif
#ifdef SMS_SIM_IP
	,{.ID = SMS_SIM_IP, .name = "SIM_IP", .pNext = NULL}
#endif
#ifdef SMS_AT_COM
	,{.ID = SMS_AT_COM, .name = "AT_COM", .pNext = NULL}
#endif
#ifdef SMS_D_ANGLE
	,{.ID = SMS_D_ANGLE, .name = "D_ANGLE", .pNext = NULL}
#endif
#ifdef SMS_D_SPEED
	,{.ID = SMS_D_SPEED, .name = "D_SPEED", .pNext = NULL}
#endif
#ifdef SMS_D_DISTANCE
	,{.ID = SMS_D_DISTANCE, .name = "D_DICTANCE", .pNext = NULL}
#endif
#ifdef SMS_FW_UPDATE
	,{.ID = SMS_FW_UPDATE, .name = "FW_UPDATE", .pNext = NULL}
#endif
#ifdef SMS_TCP_CON
	,{.ID = SMS_TCP_CON, .name = "TCP_CON", .pNext = NULL}
#endif
#ifdef SMS_NUM_USSD
	,{.ID = SMS_NUM_USSD, .name = "NUM_USSD", .pNext = NULL}
#endif
#ifdef SMS_AG_PASS
	,{.ID = SMS_AG_PASS, .name = "AG_PASS", .pNext = NULL}
#endif
#ifdef SMS_ALL_AG_SETTINGS
	,{.ID = SMS_ALL_AG_SETTINGS, .name = "ALL_AG", .pNext = NULL}
#endif

#ifdef Pipelines_SMS
	,{.ID = SMS_UST_P1, .name = "UST_P_1", .pNext = NULL}
	,{.ID = SMS_UST_S1, .name = "UST_S_1", .pNext = NULL}
	,{.ID = SMS_UST_P2, .name = "UST_P_2", .pNext = NULL}
	,{.ID = SMS_UST_S2, .name = "UST_S_2", .pNext = NULL}
	,{.ID = SMS_UST_P3, .name = "UST_P_3", .pNext = NULL}
	,{.ID = SMS_UST_S3, .name = "UST_S_3", .pNext = NULL}
	,{.ID = SMS_UST_P4, .name = "UST_P_4", .pNext = NULL}
	,{.ID = SMS_UST_S4, .name = "UST_S_4", .pNext = NULL}
#endif 

#ifdef SMS_S485
	,{.ID = SMS_S485, .name = "S485", .pNext = NULL}
#endif
#ifdef SMS_MBS
	,{.ID = SMS_MBS, .name = "MBS", .pNext = NULL}
#endif
#ifdef SMS_SNAP
	,{.ID = SMS_SNAP, .name = "SNAP", .pNext = NULL}
#endif
#ifdef SMS_TERM
	,{.ID = SMS_TERM, .name = "TERM", .pNext = NULL}
#endif
#ifdef SMS_RESET_CNT
	,{.ID = SMS_RESET_CNT, .name = "RESET_CNT", .pNext = NULL}
#endif
#ifdef SMS_ROAMING
	,{.ID = SMS_ROAMING, .name = "ROAMING", .pNext = NULL}
#endif
#ifdef SMS_MOVE_REC
	,{.ID = SMS_MOVE_REC, .name = "MOVE_REC", .pNext = NULL}
#endif
#ifdef SMS_SER_EXCH
	,{.ID = SMS_SER_EXCH, .name = "AG_SER_EXCH", .pNext = NULL}
#endif
#ifdef SMS_MODEM_EXT_POW
	,{.ID = SMS_MODEM_EXT_POW, .name = "MODEM_POW", .pNext = NULL}
#endif
#ifdef SMS_PROFILE
	,{.ID = SMS_PROFILE, .name = "PROFILE", .pNext = NULL}
#endif
#ifdef SMS_MOVE_SENS
	,{.ID = SMS_MOVE_SENS, .name = "MOVE_SENS", .pNext = NULL}
#endif
#ifdef SMS_AG_LOG_CNT
	,{.ID = SMS_AG_LOG_CNT, .name = "AG_LOG_CNT", .pNext = NULL}
#endif
#ifdef SMS_IMEI
	,{.ID = SMS_IMEI, .name = "IMEI", .pNext = NULL}
#endif
#ifdef SMS_GSM_POWER
	,{.ID = SMS_GSM_POWER, .name = "GSM_POWER", .pNext = NULL}
#endif
};

uint8_t get_sms_ID(char *name)
{
    int i;
    for(i = 0; i < sizeof(SMS_handles)/sizeof(SMS_HANDLE); i++)
    {
        if(strcmp(SMS_handles[i].name, name) == 0)
        {
            return SMS_handles[i].ID;
        }
    }
    return 0xFF;
}


void InitSMS(void)
{
	// очередь дл€ приема ответа от UserUart	
	SMSQueue = xQueueCreate(2, sizeof(COMMON_MESSAGE));
}



int sms_perform(uint8_t ID, char *data, uint8_t count)
{
	uint8_t len = 0;
	int err = 0;
   
	uint8_t *p_data = NULL;
	uint32_t time = 0;
	int k = 0;


#if (defined SMS_TIME_SYNC || defined SMS_RESET_CNT)
	DEVICE_HANDLE	hDev;
#endif


	if(data != 0)
		len = strlen((char *)data);
	else len = 0;

	if(*data != '?')
	{
	//	SetParameter(PARAMETRS_FILE_NAME, "MODEM", "InitString_1", data, len);
	    switch(ID)
	    {
#ifdef SMS_TIME_SEND
	        case SMS_TIME_SEND:
				time = atol((char *)data);
				if(SEND_DATA_MIN_TIME <= time*1000 && time*1000 <= SEND_DATA_MAX_TIME)
				{
					if(ProfInfo.cur_prof == HOME_STOP){
	            		if(SetParameter(PARAMETRS_FILE_NAME, "TIMERS", "Data_send_0", (char *)data) == -1) return -1;
					}
					else if(ProfInfo.cur_prof == HOME_MOVE){
						if(SetParameter(PARAMETRS_FILE_NAME, "TIMERS", "Data_send_1", (char *)data) == -1) return -1;
					}
					else if(ProfInfo.cur_prof == ROAMING_STOP){
						if(SetParameter(PARAMETRS_FILE_NAME, "TIMERS", "Data_send_2", (char *)data) == -1) return -1;
					}
					else{
						if(SetParameter(PARAMETRS_FILE_NAME, "TIMERS", "Data_send_3", (char *)data) == -1) return -1;
					}
				}
				else return -1;
	            break;
#endif
#ifdef SMS_TIME_SAVE
	        case SMS_TIME_SAVE:
				time = atol((char *)data);
				if(SAVE_DATA_MIN_TIME <= time*1000 && time*1000 <= SAVE_DATA_MAX_TIME)
				{
	            	if(SetParameter(PARAMETRS_FILE_NAME, "TIMERS", "Data_save", (char *)data) == -1) return -1;
				}
				else return -1;
	            break;
#endif
#ifdef SMS_TIME_SMS
	        case SMS_TIME_SMS:
				time = atol((char *)data);
				if(SMS_CHECK_MIN_TIME*1000 <= time*1000 && time*1000 <= SMS_CHECK_MAX_TIME)
				{
	            	if(SetParameter(PARAMETRS_FILE_NAME, "TIMERS", "Sms", (char *)data) == -1) return -1;
				}
				else return -1;
	            break;
#endif
	#ifdef SMS_PORT
		        case SMS_PORT:
		            if(SetParameter(AUTOGRAPH_FILE_NAME, "INFO", "Port", (char *)data) == -1) return -1;
		            break;
	#endif
#ifdef SMS_SERIAL
	#if(_AUTOGRAPH_)
	        case SMS_SERIAL:
				if(AutographInfo.change_serial_fl == 1){
	            	if(SetParameter(AUTOGRAPH_FILE_NAME, "INFO", "Serial", (char *)data) == -1) return -1;
				}
				else return -1;
	            break;
	#endif
#endif
#ifdef SMS_AG_PASS
	        case SMS_AG_PASS:
	            if(SetParameter(AUTOGRAPH_FILE_NAME, "INFO", "Password", (char *)data) == -1) return -1;
	            break;
#endif
#ifdef SMS_USSD
			case SMS_USSD:
	            if(ussd_send((char *)data, count) == -1) return -1;
						return 0; // не измен€ем параметры поэтому сразу на выход
#endif
#ifdef SMS_AN
			case SMS_AN:
				if(0 < len && len < 8) 
				{
					k = atoi((char *)data);
					if(k < SAVE_DATA_MIN_TIME) return -1;
					if(k > SAVE_DATA_MAX_TIME) return -1;
					if(ProfInfo.cur_prof == HOME_STOP){
	            		if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "AN_0", (char *)data) == -1) return -1;
					}
					else if(ProfInfo.cur_prof == HOME_MOVE){
						if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "AN_1", (char *)data) == -1) return -1;
					}
					else if(ProfInfo.cur_prof == ROAMING_STOP){
						if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "AN_2", (char *)data) == -1) return -1;
					}
					else{
						if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "AN_3", (char *)data) == -1) return -1;
					}
				}
				else return -1;
	            break;
#endif
#ifdef SMS_FREQ
			case SMS_FREQ:
				if(0 < len && len < 8)
				{
					k = atoi((char *)data);
					if(k < SAVE_DATA_MIN_TIME) return -1;
					if(k > SAVE_DATA_MAX_TIME) return -1;
					if(ProfInfo.cur_prof == HOME_STOP){
	            		if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "LLS_0", (char *)data) == -1) return -1;
					}
					else if(ProfInfo.cur_prof == HOME_MOVE){
						if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "LLS_1", (char *)data) == -1) return -1;
					}
					else if(ProfInfo.cur_prof == ROAMING_STOP){
						if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "LLS_2", (char *)data) == -1) return -1;
					}
					else{
						if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "LLS_3", (char *)data) == -1) return -1;
					}
				}
				else return -1;
	            break;
#endif
#ifdef SMS_COUNT
			case SMS_COUNT:
				if(0 < len && len < 8) 
				{
					k = atoi((char *)data);
					if(k < SAVE_DATA_MIN_TIME) return -1;
					if(k > SAVE_DATA_MAX_TIME) return -1;
					if(ProfInfo.cur_prof == HOME_STOP){
	            		if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "COUNT_0", (char *)data) == -1) return -1;
					}
					else if(ProfInfo.cur_prof == HOME_MOVE){
						if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "COUNT_1", (char *)data) == -1) return -1;
					}
					else if(ProfInfo.cur_prof == ROAMING_STOP){
						if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "COUNT_2", (char *)data) == -1) return -1;
					}
					else{
						if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "COUNT_3", (char *)data) == -1) return -1;
					}
				}
				else return -1;
	            break;
#endif
#ifdef SMS_D_ANGLE
			case SMS_D_ANGLE:
				if(0 < len && len < 4) 
				{
					k = atoi((char *)data);
					if(k < 1) return -1;
					if(k > 360) return -1;
					if(ProfInfo.cur_prof == HOME_STOP){
	            		if(SetParameter(AUTOGRAPH_FILE_NAME, "ANGLE", "D_0", (char *)data) == -1) return -1;
					}
					else if(ProfInfo.cur_prof == HOME_MOVE){
						if(SetParameter(AUTOGRAPH_FILE_NAME, "ANGLE", "D_1", (char *)data) == -1) return -1;
					}
					else if(ProfInfo.cur_prof == ROAMING_STOP){
						if(SetParameter(AUTOGRAPH_FILE_NAME, "ANGLE", "D_2", (char *)data) == -1) return -1;
					}
					else{
						if(SetParameter(AUTOGRAPH_FILE_NAME, "ANGLE", "D_3", (char *)data) == -1) return -1;
					}
				}
				else return -1;
	            break;
#endif
#ifdef SMS_D_SPEED
			case SMS_D_SPEED:
				if(0 < len && len < 4) 
				{
					k = atoi((char *)data);
					if(k < 1) return -1;
					if(k > 200) return -1;
					if(ProfInfo.cur_prof == HOME_STOP){
	            		if(SetParameter(AUTOGRAPH_FILE_NAME, "SPEED", "D_0", (char *)data) == -1) return -1;
					}
					else if(ProfInfo.cur_prof == HOME_MOVE){
						if(SetParameter(AUTOGRAPH_FILE_NAME, "SPEED", "D_1", (char *)data) == -1) return -1;
					}
					else if(ProfInfo.cur_prof == ROAMING_STOP){
						if(SetParameter(AUTOGRAPH_FILE_NAME, "SPEED", "D_2", (char *)data) == -1) return -1;
					}
					else{
						if(SetParameter(AUTOGRAPH_FILE_NAME, "SPEED", "D_3", (char *)data) == -1) return -1;
					}
				}
				else return -1;
	            break;
#endif
#ifdef SMS_D_DISTANCE
			case SMS_D_DISTANCE:
				if(1 < len && len < 5) 
				{
					k = atoi((char *)data);
					if(k < 50) return -1;
					if(k > 5000) return -1;
					if(ProfInfo.cur_prof == HOME_STOP){
	            		if(SetParameter(AUTOGRAPH_FILE_NAME, "DISTANCE", "D_0", (char *)data) == -1) return -1;
					}
					else if(ProfInfo.cur_prof == HOME_MOVE){
						if(SetParameter(AUTOGRAPH_FILE_NAME, "DISTANCE", "D_1", (char *)data) == -1) return -1;
					}
					else if(ProfInfo.cur_prof == ROAMING_STOP) {
						if(SetParameter(AUTOGRAPH_FILE_NAME, "DISTANCE", "D_2", (char *)data) == -1) return -1;
					}
					else {
						if(SetParameter(AUTOGRAPH_FILE_NAME, "DISTANCE", "D_3", (char *)data) == -1) return -1;
					}
				}
				else return -1;
	            break;
#endif
#ifdef SMS_GPS
			case SMS_GPS:
				if(0 < len && len < 8) 
				{
					k = atoi((char *)data);
					if(k < SAVE_DATA_MIN_TIME) return -1;
					if(k > SAVE_DATA_MAX_TIME) return -1;
					if(ProfInfo.cur_prof == HOME_STOP){
	            		if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "GPS_0", (char *)data) == -1) return -1;
					}
					else if(ProfInfo.cur_prof == HOME_MOVE){
						if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "GPS_1", (char *)data) == -1) return -1;
					}
					else if(ProfInfo.cur_prof == ROAMING_STOP){
						if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "GPS_2", (char *)data) == -1) return -1;
					}
					else{
						if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "GPS_3", (char *)data) == -1) return -1;
					}
				}
				else return -1;
	            break;
#endif
#ifdef SMS_TEMPER
			case SMS_TEMPER:
				if(0 < len && len < 8) 
				{
					k = atoi((char *)data);
					if(k < SAVE_DATA_MIN_TIME) return -1;
					if(k > SAVE_DATA_MAX_TIME) return -1;
					if(ProfInfo.cur_prof == HOME_STOP){
	            		if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "TEMPER_0", (char *)data) == -1) return -1;
					}
					else if(ProfInfo.cur_prof == HOME_MOVE){
						if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "TEMPER_1", (char *)data) == -1) return -1;
					}
					else if(ProfInfo.cur_prof == ROAMING_STOP){
						if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "TEMPER_2", (char *)data) == -1) return -1;
					}
					else{
						if(SetParameter(AUTOGRAPH_FILE_NAME, "S_TIME", "TEMPER_3", (char *)data) == -1) return -1;
					}
				}
				else return -1;
	            break;
#endif
#ifdef SMS_IP
	        case SMS_IP:
	            if(SetParameter(AUTOGRAPH_FILE_NAME, "INFO", "IP", (char *)data) == -1) return -1;
				if(SetParameter(PARAMETRS_FILE_NAME, "PING", "IP", (char *)data) == -1) return -1;
	            break;
#endif
#ifdef SMS_RESET
	        case SMS_RESET:
	            if(PutMesToComQueue(SYSTEM_RESTART, NULL, NULL, 5000) == 0) return -1;
	            break;
#endif
#ifdef SMS_SERIAL
	        /*case SMS_SERIAL:
	            if(SetParameter(AUTOGRAPH_FILE_NAME, "INFO", "Serial", data, len) == -1) return -1;
	            break;*/
#endif
#ifdef SMS_PASSWORD
	        case SMS_PASSWORD:
				if(len == 0) return -1;
	            if(SetParameter(PARAMETRS_FILE_NAME, "SECURITY", "Password", (char *)data) == -1) return -1;
	            break;
#endif
#ifdef SMS_RING_UP
			case SMS_RING_UP:
	            if(Ring_up(data, len) == -1) return -1;
	            break;
#endif
#ifdef SMS_LOGGPRS_1
	        case SMS_LOGGPRS_1:
	            if(SetParameter(PARAMETRS_FILE_NAME, "MODEM", "Login_1", (char *)data) == -1) return -1;
	            break;
#endif
#ifdef SMS_STRINIT_1
	        case SMS_STRINIT_1:
	            if(SetParameter(PARAMETRS_FILE_NAME, "MODEM", "InitString_1", (char *)data) == -1) return -1;
				break;
#endif
#ifdef SMS_PASSGPRS_1
	        case SMS_PASSGPRS_1:
	            if(SetParameter(PARAMETRS_FILE_NAME, "MODEM", "Password_1", (char *)data) == -1) return -1;
	            break;
#endif
#ifdef SMS_LOGGPRS_2
			case SMS_LOGGPRS_2:
	            if(SetParameter(PARAMETRS_FILE_NAME, "MODEM", "Login_2", (char *)data) == -1) return -1;
	            break;
#endif
#ifdef SMS_STRINIT_2
	        case SMS_STRINIT_2:
	            if(SetParameter(PARAMETRS_FILE_NAME, "MODEM", "InitString_2", (char *)data) == -1) return -1;
	            break;
#endif
#ifdef SMS_PASSGPRS_2
	        case SMS_PASSGPRS_2:
	            if(SetParameter(PARAMETRS_FILE_NAME, "MODEM", "Password_2", (char *)data) == -1) return -1;
	            break;
#endif
#ifdef SMS_VERS
			case SMS_VERS:
				memset(data, 0, count);
				memcpy((char *)data, (char *)BOOT_VERS_ADDR, 3);
				data[3] = ' ';
				memcpy((char *)&data[4], (char *)VERS_ADDR, 4);	 
	            break;
#endif
#ifdef SMS_IDPROCE
			case SMS_IDPROCE:
				memset(data, 0, count);
				GetProcessorID((char *)data);
	            break;
#endif
#ifdef SMS_SIM
			case SMS_SIM:
				if(0 > atoi((char *)data) && atoi((char *)data) > 2) return -1;	   // ” нас только 2 симки
				if(SetParameter(PARAMETRS_FILE_NAME, "MODEM", "Sim", (char *)data) == -1) return -1;
	            break;
#endif
#ifdef SMS_TIME_SYNC
			case SMS_TIME_SYNC:
				hDev = OpenDevice("Clock");
				err = DeviceIOCtl(hDev, 11, 0);
				CloseDevice(hDev);
				if(err == -1) return -1;
	            break;
#endif
#ifdef SMS_TIME_DELTA
	        case SMS_TIME_DELTA:
	            if(SetParameter(PARAMETRS_FILE_NAME, "INFO", "Hour_D", (char *)data) == -1) return -1;
	            break;
#endif
#ifdef SMS_AT_COM
			case SMS_AT_COM:
				if(At_send(data, count) == -1) return -1;
	            break;
#endif
#ifdef SMS_FW_UPDATE
			case SMS_FW_UPDATE:
				if(PutMesToComQueue(BOOT_REQUEST, 0, 0, 200) == 0) return -1;
	            break;
#endif
#ifdef SMS_TCP_CON
			case SMS_TCP_CON:
				if(sms_con_tcp_proc(data) == -1) return -1;
				PutEvenToQueue(E_CON_TCP_COM, 0, NULL, 5000);
	            break;
#endif
#ifdef SMS_NUM_USSD
			case SMS_NUM_USSD:
				if(SetParameter(PARAMETRS_FILE_NAME, "MODEM", "OwnNumUssd", (char *)data) == -1) return -1;
	            break;
#endif
#ifdef Pipelines_SMS
			case SMS_UST_P1:
				if(strlen((char *)data) > 10) return -1;
				sprintf((char *)con, "S0<%s", data);
				if(SetParameter(EVENTS_FILE_NAME, "0", "Con", (char *)con) == -1) return -1;
				sprintf((char *)con, "S0}%s", data);
				if(SetParameter(EVENTS_FILE_NAME, "1", "Con", (char *)con) == -1) return -1;
				break;
			case SMS_UST_S1:
				if(strlen((char *)data) > 10) return -1;
				sprintf((char *)con, "S1>%s", data);
				if(SetParameter(EVENTS_FILE_NAME, "2", "Con", (char *)con) == -1) return -1;
				sprintf((char *)con, "S1{%s", data);
				if(SetParameter(EVENTS_FILE_NAME, "3", "Con", (char *)con) == -1) return -1;
				break;
			case SMS_UST_P2:
				if(strlen((char *)data) > 10) return -1;
				sprintf((char *)con, "S2<%s", data);
				if(SetParameter(EVENTS_FILE_NAME, "4", "Con", (char *)con) == -1) return -1;
				sprintf((char *)con, "S2}%s", data);
				if(SetParameter(EVENTS_FILE_NAME, "5", "Con", (char *)con) == -1) return -1;
				break;
			case SMS_UST_S2:
				if(strlen((char *)data) > 10) return -1;
				sprintf((char *)con, "S3>%s", data);
				if(SetParameter(EVENTS_FILE_NAME, "6", "Con", (char *)con) == -1) return -1;
				sprintf((char *)con, "S3{%s", data);
				if(SetParameter(EVENTS_FILE_NAME, "7", "Con", (char *)con) == -1) return -1;
				break;
			case SMS_UST_P3:
				if(strlen((char *)data) > 10) return -1;
				sprintf((char *)con, "S4<%s", data);
				if(SetParameter(EVENTS_FILE_NAME, "8", "Con", (char *)con) == -1) return -1;
				sprintf((char *)con, "S4}%s", data);
				if(SetParameter(EVENTS_FILE_NAME, "9", "Con", (char *)con) == -1) return -1;
				break;
			case SMS_UST_S3:
				if(strlen((char *)data) > 10) return -1;
				sprintf((char *)con, "S5>%s", data);
				if(SetParameter(EVENTS_FILE_NAME, "10", "Con", (char *)con) == -1) return -1;
				sprintf((char *)con, "S5{%s", data);
				if(SetParameter(EVENTS_FILE_NAME, "11", "Con", (char *)con) == -1) return -1;
				break;
			case SMS_UST_P4:
				if(strlen((char *)data) > 10) return -1;
				sprintf((char *)con, "S6<%s", data);
				if(SetParameter(EVENTS_FILE_NAME, "12", "Con", (char *)con) == -1) return -1;
				sprintf((char *)con, "S6}%s", data);
				if(SetParameter(EVENTS_FILE_NAME, "13", "Con", (char *)con) == -1) return -1;
				break;
			case SMS_UST_S4:
				if(strlen((char *)data) > 10) return -1;
				sprintf((char *)con, "S7>%s", data);
				if(SetParameter(EVENTS_FILE_NAME, "14", "Con", (char *)con) == -1) return -1;
				sprintf((char *)con, "S7{%s", data);
				if(SetParameter(EVENTS_FILE_NAME, "15", "Con", (char *)con) == -1) return -1;
				break;
#endif

#ifdef 	SMS_S485
	#if _UART_CONTROL_TASK_
			case SMS_S485:
				sms_queue_proc(data, count, S485, UartControlQueue);
				break;
	#endif
#endif

#ifdef 	SMS_MBS
	#if _UART_CONTROL_TASK_
			case SMS_MBS:
				sms_queue_proc(data, count, MBS_RW, UartControlQueue);
				break;
	#endif
#endif

#ifdef 	SMS_SNAP
	#if _UART_CONTROL_TASK_
			case SMS_SNAP:
				sms_queue_proc(data, count, SNAPSHOT, UartControlQueue);
				break;
	#endif
#endif
#ifdef 	SMS_TERM
	#if _UART_CONTROL_TASK_
			case SMS_TERM:
				sms_queue_proc(data, count, UART_TERM, UartControlQueue);
				break;
	#endif
#endif
#ifdef 	SMS_RESET_CNT
			case SMS_RESET_CNT:
				hDev = OpenDevice("IMPULSE");
				err = DeviceIOCtl(hDev, RESET_DEVICE, 0);
				CloseDevice(hDev);
				if(err == -1) return -1;
				break;
#endif
#ifdef SMS_ROAMING
	        case SMS_ROAMING:
				k = atoi((char *)data);
				if(k < 0) return -1;
				if(k > 1) return -1;
	            if(SetParameter(PARAMETRS_FILE_NAME, "MODEM", "Roaming", (char *)data) == -1) return -1;
	            break;
#endif
#ifdef SMS_MOVE_SENS
	        case SMS_MOVE_SENS:
				if(strlen((char *)data) > 5) return -1;
	            if(SetParameter(PARAMETRS_FILE_NAME, "MoveSens", "S", (char *)data) == -1) return -1;
	            break;
#endif
#ifdef SMS_MOVE_REC
	        case SMS_MOVE_REC:
				k = atoi((char *)data);
				if(k < 0) return -1;
				if(k > 1) return -1;
	            if(SetParameter(AUTOGRAPH_FILE_NAME, "INFO", "M_REC", (char *)data) == -1) return -1;
	            break;
#endif
#ifdef SMS_SER_EXCH
	        case SMS_SER_EXCH:
				k = atoi((char *)data);
				if(k != 0) return -1;
	            if(SetParameter(AUTOGRAPH_FILE_NAME, "INFO", "SER_SMS", (char *)data) == -1) return -1;
	            break;
#endif
#ifdef SMS_MODEM_EXT_POW
			case SMS_MODEM_EXT_POW:
				if(strlen((char *)data) > 10) return -1;
	            if(SetParameter(PARAMETRS_FILE_NAME, "MODEM", "ExPow", (char *)data) == -1) return -1;
	            break;
#endif
#ifdef SMS_PROFILE
			case SMS_PROFILE:
				k = atoi((char *)data);
				if(k < HOME_STOP) return -1;
				if(k > ROAMING_MOVE) return -1;
	            ProfInfo.cur_prof = k;
				ProfileChange();
	            break;
#endif
#ifdef SMS_AG_LOG_CNT
			case SMS_AG_LOG_CNT:
				ResetAgLogCnt();
	            break;
#endif
			default:
				return -1;

	    }					   
		PutMesToComQueue(DEV_REINIT, NULL, NULL, 1000);
		if(data == NULL) return 0; 
	
		if(ID != SMS_VERS && ID != SMS_IDPROCE && ID != SMS_USSD && ID != SMS_AT_COM)
		{
			#if (SMS_S485 && SMS_MBS && SMS_SNAP)
				if(ID != SMS_S485 && ID != SMS_MBS && ID != SMS_SNAP && ID != SMS_TERM)
				{	 
					memset(data, 0, count);
                    sprintf(data, "SMS ID %d, name - %s: OK", ID, SMSInfo.name);
				}
			#elif SMS_S485
				if(ID != SMS_S485)
				{
					memset(data, 0, count);
					sprintf(data, "SMS ID %d, name - %s: OK", ID, SMSInfo.name);
				}
			#elif SMS_MBS
				if(ID != SMS_MBS)
				{
					memset(data, 0, count);
					sprintf(data, "SMS ID %d, name - %s: OK", ID, SMSInfo.name);
				}
			#elif SMS_SNAP
				if(ID != SMS_SNAP)
				{
					memset(data, 0, count);
					sprintf(data, "SMS ID %d, name - %s: OK", ID, SMSInfo.name);
				}
			#elif SMS_TERM
				if(ID != SMS_TERM)
				{
					memset(data, 0, count);
					sprintf(data, "SMS ID %d, name - %s: OK", ID, SMSInfo.name);
				}
			#else
				memset(data, 0, count);
				sprintf(data, "SMS ID %d, name - %s: OK", ID, SMSInfo.name);
			#endif
		}	   
	}
	else
	{
		memset(data, 0, count);
		switch(ID)
	    {
#ifdef SMS_TIME_SEND
			case SMS_TIME_SEND:
	            ReadSettingsFile(PARAMETRS_FILE_NAME);
				if(ProfInfo.cur_prof == HOME_STOP)
					p_data = (uint8_t *)GetPointToElement("TIMERS", "Data_send_0");
				else if(ProfInfo.cur_prof == HOME_MOVE)
					p_data = (uint8_t *)GetPointToElement("TIMERS", "Data_send_1");
				else if(ProfInfo.cur_prof == ROAMING_STOP)
					p_data = (uint8_t *)GetPointToElement("TIMERS", "Data_send_2");
				else
					p_data = (uint8_t *)GetPointToElement("TIMERS", "Data_send_3");
	            break;
#endif
#ifdef SMS_TIME_SAVE
			case SMS_TIME_SAVE:
	            ReadSettingsFile(PARAMETRS_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("TIMERS", "Data_save");
	            break;
#endif
#ifdef SMS_TIME_SMS
	        case SMS_TIME_SMS:
				ReadSettingsFile(PARAMETRS_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("TIMERS", "Sms");
	            break;
#endif
#ifdef SMS_PORT
	        case SMS_PORT:
				ReadSettingsFile(AUTOGRAPH_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("INFO", "Port");
	            break;
#endif
#ifdef SMS_AN
			case SMS_AN:
				ReadSettingsFile(AUTOGRAPH_FILE_NAME);
				if(ProfInfo.cur_prof == HOME_STOP)
					p_data = (uint8_t *)GetPointToElement("S_TIME", "AN_0");
				else if(ProfInfo.cur_prof == HOME_MOVE)
					p_data = (uint8_t *)GetPointToElement("S_TIME", "AN_1");
				else if(ProfInfo.cur_prof == ROAMING_STOP)
					p_data = (uint8_t *)GetPointToElement("S_TIME", "AN_2");
				else
					p_data = (uint8_t *)GetPointToElement("S_TIME", "AN_3");
	            break;
#endif
#ifdef SMS_FREQ
			case SMS_FREQ:
				ReadSettingsFile(AUTOGRAPH_FILE_NAME);
				if(ProfInfo.cur_prof == HOME_STOP)
					p_data = (uint8_t *)GetPointToElement("S_TIME", "LLS_0");
				else if(ProfInfo.cur_prof == HOME_MOVE)
					p_data = (uint8_t *)GetPointToElement("S_TIME", "LLS_1");
				else if(ProfInfo.cur_prof == ROAMING_STOP)
					p_data = (uint8_t *)GetPointToElement("S_TIME", "LLS_2");
				else
					p_data = (uint8_t *)GetPointToElement("S_TIME", "LLS_3");
	            break;
#endif
#ifdef SMS_COUNT
			case SMS_COUNT:
				ReadSettingsFile(AUTOGRAPH_FILE_NAME);
				if(ProfInfo.cur_prof == HOME_STOP)
					p_data = (uint8_t *)GetPointToElement("S_TIME", "COUNT_0");
				else if(ProfInfo.cur_prof == HOME_MOVE)
					p_data = (uint8_t *)GetPointToElement("S_TIME", "COUNT_1");
				else if(ProfInfo.cur_prof == ROAMING_STOP)
					p_data = (uint8_t *)GetPointToElement("S_TIME", "COUNT_2");
				else
					p_data = (uint8_t *)GetPointToElement("S_TIME", "COUNT_3");
	            break;
#endif
#ifdef SMS_GPS
			case SMS_GPS:
				ReadSettingsFile(AUTOGRAPH_FILE_NAME);
				if(ProfInfo.cur_prof == HOME_STOP)
					p_data = (uint8_t *)GetPointToElement("S_TIME", "GPS_0");
				else if(ProfInfo.cur_prof == HOME_MOVE)
					p_data = (uint8_t *)GetPointToElement("S_TIME", "GPS_1");
				else if(ProfInfo.cur_prof == ROAMING_STOP)
					p_data = (uint8_t *)GetPointToElement("S_TIME", "GPS_2");
				else
					p_data = (uint8_t *)GetPointToElement("S_TIME", "GPS_3");
	            break;
#endif
#ifdef SMS_D_ANGLE
			case SMS_D_ANGLE:
				ReadSettingsFile(AUTOGRAPH_FILE_NAME);
				if(ProfInfo.cur_prof == HOME_STOP)
					p_data = (uint8_t *)GetPointToElement("ANGLE", "D_0");
				else if(ProfInfo.cur_prof == HOME_MOVE)
					p_data = (uint8_t *)GetPointToElement("ANGLE", "D_1");
				else if(ProfInfo.cur_prof == ROAMING_STOP)
					p_data = (uint8_t *)GetPointToElement("ANGLE", "D_2");
				else
					p_data = (uint8_t *)GetPointToElement("ANGLE", "D_3");
	            break;
#endif
#ifdef SMS_D_SPEED
			case SMS_D_SPEED:
				ReadSettingsFile(AUTOGRAPH_FILE_NAME);
				if(ProfInfo.cur_prof == HOME_STOP)
					p_data = (uint8_t *)GetPointToElement("SPEED", "D_0");
				else if(ProfInfo.cur_prof == HOME_MOVE)
					p_data = (uint8_t *)GetPointToElement("SPEED", "D_1");
				else if(ProfInfo.cur_prof == ROAMING_STOP)
					p_data = (uint8_t *)GetPointToElement("SPEED", "D_2");
				else
					p_data = (uint8_t *)GetPointToElement("SPEED", "D_3");
	            break;
#endif
#ifdef SMS_D_DISTANCE
			case SMS_D_DISTANCE:
				ReadSettingsFile(AUTOGRAPH_FILE_NAME);
				if(ProfInfo.cur_prof == HOME_STOP)
					p_data = (uint8_t *)GetPointToElement("DISTANCE", "D_0");
				else if(ProfInfo.cur_prof == HOME_MOVE)
					p_data = (uint8_t *)GetPointToElement("DISTANCE", "D_1");
				else if(ProfInfo.cur_prof == ROAMING_STOP)
					p_data = (uint8_t *)GetPointToElement("DISTANCE", "D_2");
				else
					p_data = (uint8_t *)GetPointToElement("DISTANCE", "D_3");
	            break;
#endif
#ifdef SMS_IP
	        case SMS_IP:
				ReadSettingsFile(AUTOGRAPH_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("INFO", "IP");
	            break;
#endif
#ifdef SMS_SERIAL
	        case SMS_SERIAL:
				ReadSettingsFile(AUTOGRAPH_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("INFO", "Serial");
	            break;
#endif
#ifdef SMS_AG_PASS
	        case SMS_AG_PASS:
				ReadSettingsFile(AUTOGRAPH_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("INFO", "Password");
	            break;
#endif
#ifdef SMS_PASSWORD
	        case SMS_PASSWORD:
				ReadSettingsFile(PARAMETRS_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("SECURITY", "Password");
				strcpy((char *)data, (char *)p_data);
	            break;
#endif
#ifdef SMS_LOGGPRS_1
	        case SMS_LOGGPRS_1:
				ReadSettingsFile(PARAMETRS_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("MODEM", "Login_1");
	            break;
#endif
#ifdef SMS_STRINIT_1
	        case SMS_STRINIT_1:
				ReadSettingsFile(PARAMETRS_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("MODEM", "InitString_1");
				strcpy((char *)data, (char *)p_data);
	            break;
#endif
#ifdef SMS_PASSGPRS_1
	        case SMS_PASSGPRS_1:
				ReadSettingsFile(PARAMETRS_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("MODEM", "Password_1");
				strcpy((char *)data, (char *)p_data);
	            break;
#endif
#ifdef SMS_LOGGPRS_2
			case SMS_LOGGPRS_2:
				ReadSettingsFile(PARAMETRS_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("MODEM", "Login_2");
	            break;
#endif
#ifdef SMS_STRINIT_2
	        case SMS_STRINIT_2:
				ReadSettingsFile(PARAMETRS_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("MODEM", "InitString_2");
				strcpy((char *)data, (char *)p_data);
	            break;
#endif
#ifdef SMS_PASSGPRS_2
	        case SMS_PASSGPRS_2:
				ReadSettingsFile(PARAMETRS_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("MODEM", "Password_2");
	            break;
#endif
#ifdef SMS_SIM
			case SMS_SIM:
				ReadSettingsFile(PARAMETRS_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("MODEM", "Sim");
	            break;
#endif
#ifdef SMS_SIM_IP
			case SMS_SIM_IP:
				ReadSettingsFile(PARAMETRS_FILE_NAME);	 //  Ќужно, так как мы по любому в конце вызываем	ParametrsMemFree
				p_data = (uint8_t *)GetOurIPStr();
	            break;
#endif
#ifdef SMS_TIME_DELTA
	        case SMS_TIME_DELTA:
				ReadSettingsFile(PARAMETRS_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("INFO", "Hour_D");
	            break;
#endif
#ifdef SMS_NUM_USSD
			case SMS_NUM_USSD:
				ReadSettingsFile(PARAMETRS_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("MODEM", "OwnNumUssd");
	            break;
#endif

#ifdef SMS_BOOT_VERS
			case SMS_BOOT_VERS:
				memcpy((char *)data, (char *)BOOT_VERS_ADDR, 4);
				p_data = data;
	            break;
#endif
#ifdef SMS_PROFILE
			case SMS_PROFILE:
				sprintf((char *)data, "%1lu", ProfInfo.cur_prof);
				p_data = (uint8_t *)data;
	            break;
#endif
#ifdef SMS_ROAMING
	        case SMS_ROAMING:
				ReadSettingsFile(PARAMETRS_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("MODEM", "ROAMING");
	            break;
#endif
#ifdef SMS_MOVE_REC
	        case SMS_MOVE_REC:
				ReadSettingsFile(AUTOGRAPH_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("INFO", "M_REC");
	            break;
#endif
#ifdef SMS_SER_EXCH
	        case SMS_SER_EXCH:
				ReadSettingsFile(AUTOGRAPH_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("INFO", "SER_SMS");
	            break;
#endif
#ifdef SMS_MODEM_EXT_POW
	        case SMS_MODEM_EXT_POW:
				ReadSettingsFile(PARAMETRS_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("MODEM", "ExPow");
	            break;
#endif
#ifdef SMS_AG_LOG_CNT
	        case SMS_AG_LOG_CNT:
				GetAGLofgCnt(data);
				p_data = data;
	            break;
#endif
#ifdef SMS_IMEI
	        case SMS_IMEI:
				GetIMEI(data);
				p_data = data;
	            break;
#endif
#ifdef SMS_GSM_POWER
	        case SMS_GSM_POWER:
				sprintf(data, "GSM POWER - %d", GetSigLevel());
				p_data = data;
	            break;
#endif
#ifdef SMS_MOVE_SENS
	        case SMS_MOVE_SENS:
				ReadSettingsFile(PARAMETRS_FILE_NAME);
				p_data = (uint8_t *)GetPointToElement("MoveSens", "S");
	            break;
#endif

#ifdef Pipelines_SMS
			case SMS_UST_P1:
				ReadSettingsFile(EVENTS_FILE_NAME);
				p_data = ((uint8_t *)GetPointToElement("0", "Con") + 3);
				break;	
			case SMS_UST_S1:
				ReadSettingsFile(EVENTS_FILE_NAME);
				p_data = ((uint8_t *)GetPointToElement("2", "Con") + 3);
				break;
			case SMS_UST_P2:
				ReadSettingsFile(EVENTS_FILE_NAME);
				p_data = ((uint8_t *)GetPointToElement("4", "Con") + 3);
				break;
			case SMS_UST_S2:
				ReadSettingsFile(EVENTS_FILE_NAME);
				p_data = ((uint8_t *)GetPointToElement("6", "Con") + 3);
				break;
			case SMS_UST_P3:
				ReadSettingsFile(EVENTS_FILE_NAME);
				p_data = ((uint8_t *)GetPointToElement("8", "Con") + 3);
				break;
			case SMS_UST_S3:
				ReadSettingsFile(EVENTS_FILE_NAME);
				p_data = ((uint8_t *)GetPointToElement("10", "Con") + 3);
				break;
			case SMS_UST_P4:
				ReadSettingsFile(EVENTS_FILE_NAME);
				p_data = ((uint8_t *)GetPointToElement("12", "Con") + 3);
				break;
			case SMS_UST_S4:
				ReadSettingsFile(EVENTS_FILE_NAME);
				p_data = ((uint8_t *)GetPointToElement("14", "Con") + 3);
				break;
#endif
#ifdef SMS_ALL_AG_SETTINGS
			case SMS_ALL_AG_SETTINGS:
				{
					int len_t = 0;
					ReadSettingsFile(AUTOGRAPH_FILE_NAME);
					p_data = ((uint8_t *)GetPointToElement("INFO", "Serial"));
					len = strlen((char *)p_data);
					len_t += len;
					if(count < (len_t + len)) len = len_t - count;
					strncpy((char *)data, (char *)p_data, len);
					data[len_t++] = ';';

					p_data = ((uint8_t *)GetPointToElement("INFO", "IP"));
					len = strlen((char *)p_data);
					if(count < (len_t + len)) len = len_t - count;
					strncpy(&((char *)data)[len_t], (char *)p_data, len);
					len_t += len;
					data[len_t++] = ';';

					p_data = ((uint8_t *)GetPointToElement("INFO", "Port"));
					len = strlen((char *)p_data);
					if(count < (len_t + len)) len = len_t - count;
					strncpy(&((char *)data)[len_t], (char *)p_data, len);
					len_t += len;
					data[len_t++] = ';';

					p_data = ((uint8_t *)GetPointToElement("INFO", "Password"));
					len = strlen((char *)p_data);
					if(count < (len_t + len)) len = len_t - count;
					strncpy(&((char *)data)[len_t], (char *)p_data, len);
					len_t += len;
					data[len_t] = 0;
					ParametrsMemFree();
					return 0;

				}
#endif
#ifdef SMS_ALL_GPRS_SETTINGS
			case SMS_ALL_GPRS_SETTINGS:
				{
					int len_t = 0;
					ReadSettingsFile(PARAMETRS_FILE_NAME);
					p_data = ((uint8_t *)GetPointToElement("MODEM", "InitString_1"));
					len = strlen((char *)p_data);
					len_t += len;
					if(count < (len_t + len)) len = len_t - count;
					strncpy((char *)data, (char *)p_data, len);
					data[len_t++] = ';';

					p_data = ((uint8_t *)GetPointToElement("MODEM", "Login_1"));
					len = strlen((char *)p_data);
					if(count < (len_t + len)) len = len_t - count;
					strncpy(&((char *)data)[len_t], (char *)p_data, len);
					len_t += len;
					data[len_t++] = ';';

					p_data = ((uint8_t *)GetPointToElement("MODEM", "Password_1"));
					len = strlen((char *)p_data);
					if(count < (len_t + len)) len = len_t - count;
					strncpy(&((char *)data)[len_t], (char *)p_data, len);
					len_t += len;
					data[len_t] = 0;
					ParametrsMemFree();
					return 0;

				}
#endif
			default:
                sprintf(data, "SMS ID %d, name - %s: not supported", ID, SMSInfo.name);
				break;
	    }
		if(p_data != NULL)
		{
			len = strlen((char *)p_data);
			if(len > count) len = count - 1;
			strncpy((char *)data, (char *)p_data, len);
			data[len] = 0;
		}
		else sprintf(data, "SMS ID %d, name - %s: data is NULL", ID, SMSInfo.name);
		ParametrsMemFree();
	}	   
    return 0; 
}



SMS_DATA sms_handling(char *buff, uint16_t count)
{
    uint8_t i = 0, k = 0, j = 0;
	volatile uint8_t len = 0;
    SMS_INFO *pSms = (SMS_INFO *)&SMSInfo;
    SMS_DATA sms_data;

    sms_data.name = NULL;
    sms_data.arg = NULL;

    while(i < MAX_SMS_BUFF_LENGTH && pSms->buf[i] != ',') i++;

    if((i + PHONE_NUM_LENGTH - 1) >= MAX_SMS_BUFF_LENGTH) return sms_data;
    if(pSms->buf[++i + 1] != '+') return sms_data;

    memcpy(pSms->phon_num, (uint8_t *)&(pSms->buf[i]), PHONE_NUM_LENGTH - 1);
    pSms->phon_num[PHONE_NUM_LENGTH - 1] = 0x00;

    i += PHONE_NUM_LENGTH - 2; //  —мещаем указатель на первый символ после номера

    while(i < MAX_SMS_BUFF_LENGTH && pSms->buf[i] != 0x0a) i++;

    if(i >= MAX_SMS_BUFF_LENGTH) return sms_data;

    for(k = 5; k < MAX_SMS_BUFF_LENGTH; k++)
    {
        if(pSms->buf[k] == ',' || pSms->buf[k] == 0x0d) 
        {
            pSms->buf[k] = 0x00;
        }
        if(pSms->buf[k] == 0x0a) break;
    }

    for(k += 1; k < MAX_SMS_BUFF_LENGTH; k++)
    {
        if(pSms->buf[k] == ' ' || pSms->buf[k] == 0x0d) 
        {
            pSms->buf[k] = 0x00;
        }
        if(pSms->buf[k] == 0x0a) break;
    }

    
    len = strlen((char *)&(pSms->buf[++i]));	   // тут находитс€ пароль
    if(len != 8) return sms_data; 
    j = i;   //  запоминаем позицию с поролем

    i += len;
    len = strlen((char *)&(pSms->buf[++i]));
    if(len >= MAX_SMS_NAME_LENGTH) return sms_data; 
    k = i;  //  запоминаем позицию с именем команды
    strcpy(pSms->name, &(pSms->buf[i]));
    i += len;
    len = strlen((char *)&(pSms->buf[++i]));
    if(len > 40 || pSms->buf[i] == 0x0a || pSms->buf[i] == 0x0d) return sms_data; 

    sms_data.name = &(pSms->buf[k]);
    sms_data.arg = &(pSms->buf[i]);
		sms_data.pass = &(pSms->buf[j]);
		sms_data.count = MAX_SMS_BUFF_LENGTH - i;

    return sms_data;
}


//uint8_t ann[50] = MODEM_INIT_STRING;

int sms_proces(void)   //  обработка смс сообщений
{
    SMS_DATA sms_data;
    uint8_t ID;
    int err = 0;
    SMS_INFO *pSms = (SMS_INFO *)&SMSInfo;
	
    Sync_time_gsm();
    err = sms_check(pSms->index);  // смотрим есть ли сообщени€
   
    while(err && err != -1) // цикл пока присутствуют сообщени€ или до ошибки при проверки их наличи€
    {
		memset((char *)pSms->buf, 0, MAX_SMS_BUFF_LENGTH);
        if(sms_read(pSms->index, pSms->buf, MAX_SMS_BUFF_LENGTH) == -1) // „итаем сообщение с заданным индексам
        {  
            PUT_MES_TO_LOG("ERROR BY READING SMS", 2, 1);
            return -1;
        }
		
        PutMesToLog((char *)(pSms->buf), 2, 1);
        sms_data = sms_handling(pSms->buf, MAX_SMS_BUFF_LENGTH); //–азбор текстовой строки смс сообщени€
		//sms_perform(0x08, sms_data.arg, sms_data.count);
	   	
        if(pSms->phon_num[0] == 0)   //  ≈сли необычный номер - сообщение не в тему
        {
            sms_delete(pSms->index);
			PUT_MES_TO_LOG("SMS: Phone ERROR", 2, 1);
            vTaskDelay(1500);
        }
        else if(sms_data.name == 0)   //  ≈сли им€ не распозналось - сообщение не в тему
        {
            sms_delete(pSms->index);
			PUT_MES_TO_LOG("SMS: Id message ERROR", 2, 1);
            vTaskDelay(1500);
        }
        else
        {
            ID = get_sms_ID(sms_data.name);  // –аспознаем идентификатор сообщени€
            if(ID != SMS_IDPROCE)                //  ƒл€ получени€ ID процессора аунтификаци€ не нужна
            {
                if(SmsAccessRequest((char *)pSms->phon_num, (char *)sms_data.pass) != 1)
                {
                    sms_delete(pSms->index);
                    PUT_MES_TO_LOG("SMS: AUTH FAULT", 2, 1);
                    vTaskDelay(1500);
                    sms_send(pSms->phon_num, "SMS: AUTH FAULT");
                    err = sms_check(pSms->index);
                    continue;	
                }		
            }
			if(ID == 0XFF)                   
            {
				sms_delete(pSms->index);
				PUT_MES_TO_LOG("SMS: FAILED ID", 2, 1);
	            vTaskDelay(1500);
                sprintf(pSms->buf, "SMS: FAILED ID - %s", pSms->name);
	            sms_send(pSms->phon_num, pSms->buf);
	        }
			else
			{
		            err = sms_perform(ID, sms_data.arg, sms_data.count);  // ¬ыполн€ем действи€, которые предписывает сообщение
		            if(err == -1)
		            {
		                PUT_MES_TO_LOG("SMS DOES NOT PERFOME", 2, 1);
		                sms_delete(pSms->index);
		                vTaskDelay(1500);
                        sprintf(pSms->buf, "SMS ID %d, name - %s: not perfome", ID, pSms->name);
		                sms_send(pSms->phon_num, pSms->buf);
		            }
		            else
		            {
		                PUT_MES_TO_LOG("SMS PERFOME", 2, 1);
		                sms_delete(pSms->index);
		                vTaskDelay(1500);
						sms_send(pSms->phon_num, sms_data.arg);
		            }
			}	 
        }	   
        vTaskDelay(1000);
        err = sms_check(pSms->index); 
    } 	 			 
    return err;
}

void sms_queue_proc(char *data, uint8_t max_data_len, uint8_t id, xQueueHandle q_handle)
{
	COMMON_MESSAGE message;


	if(q_handle == NULL)
	{
//		if(data != NULL)
//			strcpy((char *)data, (char *)ERROR_SMS);
		return;
	}

	while(xQueueReceive(SMSQueue, &message, 0));


	message.dst = id;
	message.privat_data = (uint8_t *)data;
	message.data = max_data_len;

	xQueueSendToFront(q_handle, &message, 0);

	if(data == NULL) return;

/*	if(!xQueueReceive(SMSQueue, &message, 30000))
	{
		strcpy((char *)data, (char *)ERROR_SMS);	
	}
	else
	{
		strcpy((char *)data, (char *)message.privat_data);	
	}*/
}

#ifdef SMS_TCP_CON
//void tun_set_addr(uint32_t 	addr, uint16_t 	port);
int sms_con_tcp_proc(char *data)
{
	int len = strlen((char *)data);
	int i = 0;

	if(len > 25) return -1;
	
	for(i=0; i < len; i++)
	{
		if(data[i] == ' ' || data[i] == 0x0a ||  data[i] == 0x0d)
			data[i] = 0;	
	}
	
	DEVICE_HANDLE hTCP = OpenDevice("TCP_RD_1");
	uint16_t port = GetNumericPort(&data[strlen(data) + 1]);
	uint32_t ip	= GetNumericIP(data);

	DeviceIOCtl(hTCP, 0, (uint8_t *)&port);
	DeviceIOCtl(hTCP, 1, (uint8_t *)&ip);

	CloseDevice(hTCP);
	
	//tun_set_addr(GetNumericIP(data), atoi(&data[strlen(data) + 1]));
	return 0;
}
#endif

#endif	

