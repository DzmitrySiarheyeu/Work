#include <stdint.h>
#include "LPC17XX.h"
#include "SensorCore.h"
#include "DataFunc.h"
#include "PhysFunc.h"
#include "User_Timers.h"
#include "DriversCore.h"
#include "main.h"
#include "VFS.h"
#include "ftpd.h"
#include "http_server.h"
#include "telnet.h"
#include "Events.h"
#include "User_Events.h"
#include "lwIP_Port.h"
#include "SetParametrs.h"
#include "display.h"
#include "log.h"
#include "Sms.h"
#include "Clock_DRV.h"
#include "Modem_DRV.h"
#include "tcp_redirector.h"

#include "lpc17xx_gpio.h"
#include "lpc17xx_wdt.h"
#include "Security.h"
#include "slipif.h"						
#include "User_Error.h"	 
#include "User_Events.h"
#include "User_ModBus.h"
#include "saver.h"
#include "wdt.h"
#include "ping.h"

#include "MBServer.h"
#include "MB_serial_port.h"
#include "MB_server_device.h"
#include "MB_serial_client.h"
#include "UartControlTask.h"
#include "ModemTask.h"

#include <LPC1765.h>

#if(_WDT_)
	#include "WDT.h"
#endif

void PowerOnLog(void);

#define portNVIC_SYSTICK_CTRL		( ( volatile unsigned long *) 0xe000e010 )

#define		USER_UART			0
#define		TCP_COM				1

/***************Указатели на функции**************/
CHECK_FUNK 			check_funk;
SMS_FUNK			sms_funk;
SYNC_TIME_FUNK		synk_time_func;
SIG_LEV_FUNK		sig_lev_func;		
/*************************************************/

/*************Функции из других модулей***********/
int ModemCheckSignal(DEVICE_HANDLE handle);
/*************************************************/

PROFILE_INFO ProfInfo = {.def_prof = 0, .cur_prof = 0, .prof_0 = 1, .prof_1 = 0, .prof_2 = 0, .prof_3 = 0};

void ResetTask( void *pvParameters );
#if _MB_TCP_
	void ModbusTCPTask( void *pvParameters );
#endif
#if _MBS_
	void MBS_Task( void *pvParameters );
#endif
#if(_MBC_)
	void MBC_Task( void *pvParameters );
#endif

/************Хендлы тасков************************/
#if _FTP_
    xTaskHandle FTPHandle;
#endif
#if(_HTTP_)
    xTaskHandle HTTPHandle;
#endif
#if(_TELNET_)
    xTaskHandle TelnetHandle;
#endif
#if(_ETH2COM_)
    xTaskHandle ETH2COMHandle;
    xTaskHandle COM2ETHHandle = NULL;
#endif
#if(_EVENTS_)
        xTaskHandle EventHandle;
#endif
#if(_MODEM_)
        xTaskHandle ModemHandle;
#endif
#if _MB_TCP_
        xTaskHandle ModBusHandle;
#endif
#if _UART_CONTROL_TASK_
        xTaskHandle UartControlHandle;		
#endif
#if(_MBC_)
 	xTaskHandle MBCHandle;
#endif

xTaskHandle ResetTaskHandle;
/*************************************************/

void Default_WDTHandler(void);
void TIMER0_IRQ(void) {Default_WDTHandler();};
void TIMER2_IRQ(void) {Default_WDTHandler();};
void TIMER3_IRQ(void) {Default_WDTHandler();};
void ADC_IRQ(void) {Default_WDTHandler();};
void CAN_IRQ(void) {Default_WDTHandler();};

struct UST_STR
{
	float IsolUst;
	float SignUst;
};

struct UST_STR UstStr[4] = {};

extern xQueueHandle timers_queue;

xQueueHandle common_queue;
xQueueHandle UartControlQueue = NULL;
uint8_t Key;					// состояние кнопки
uint8_t startInitFlag;		// задержка чтения датчиков, событий и выдачи данных
uint8_t simPresentFlag = 0;		// флаг есть/нет sim
uint8_t LCDContrast = 0xB0;		// дефолтовый контраст CLD
uint8_t initErrorFlag = 0;      // флаг ошибок инициализации
int MBPort = 502;
xQueueHandle user_message_queue;
xQueueHandle reset_task_queue = NULL;


int flag_pos = 0;

void common_queue_proc(void);

static void init_wdt_cnt(void);
void AwakeResetTask(int isr_fl, char *data);
void USBConfigureInterface(void * param);

void initLCDContrast(void)
{
	char *res;
	int contr;

	if(GetSettingsFileStatus())
	{
		res = (char *)GetPointToElement("DISPLAY", "Contrast");
		if(res == 0)	
			return;
			
		contr = atoi(res);
		if(contr > 255 || contr < 0)
			return;
		else
			LCDContrast = contr;	
	}	
}

void readUstValues(void)
{
	int i;
	for(i = 0; i < 4; i++)
	{
		UstStr[i].IsolUst = -1.0f;
		UstStr[i].SignUst = -1.0f;	
	}
	
	char *res;

	if(GetSettingsFileStatus())
	{
		if(GetPointToElement("0", "Con"))
		{
			res = (char *)GetPointToElement("0", "Con");
			if(res != 0)	
				UstStr[0].IsolUst = atof(&res[3]);
		}
		if(GetPointToElement("1", "Con"))
		{
			res = (char *)GetPointToElement("2", "Con");
			if(res != 0)	
				UstStr[0].SignUst = atof(&res[3]);
		}
		
		if(GetPointToElement("2", "Con"))
		{
			res = (char *)GetPointToElement("4", "Con");
			if(res != 0)	
				UstStr[1].IsolUst = atof(&res[3]);
		}
		if(GetPointToElement("3", "Con"))
		{
			res = (char *)GetPointToElement("6", "Con");
			if(res != 0)	
				UstStr[1].SignUst = atof(&res[3]);
		}
		
		if(GetPointToElement("4", "Con"))
		{
			res = (char *)GetPointToElement("8", "Con");
			if(res != 0)	
				UstStr[2].IsolUst = atof(&res[3]);
		}
		if(GetPointToElement("5", "Con"))
		{
			res = (char *)GetPointToElement("10", "Con");
			if(res != 0)	
				UstStr[2].SignUst = atof(&res[3]);
		}
		
		if(GetPointToElement("6", "Con"))
		{
			res = (char *)GetPointToElement("12", "Con");
			if(res != 0)	
				UstStr[3].IsolUst = atof(&res[3]);
		}
		if(GetPointToElement("7", "Con"))
		{
			res = (char *)GetPointToElement("14", "Con");
			if(res != 0)	
				UstStr[3].SignUst = atof(&res[3]);
		}	
	}	
}

float GetUstVal(int ch, int is)
{
	if(is == 0)
		return UstStr[ch].IsolUst;
	else
		return UstStr[ch].SignUst;
}



void ReInitDevSystem(void)
{
	vTaskSuspendAll();
	//vPortEnterCritical();

	ReadSettingsFile(PARAMETRS_FILE_NAME);

	SetModemSettings(0);
	MBCPortConfig();   // определяем порт для MB
	MBCMethodConfig(); // определяем, какой у нас клиент ModBus: TCP или удаленный COM
	ParametrsMemFree();	
	
	ResetWDT();

	ReadSettingsFile(EVENTS_FILE_NAME);
	DelAllEvents();
	InitEventCheck();
	readUstValues();
	ParametrsMemFree();

	ResetWDT();
   
	ReadSettingsFile(PARAMETRS_FILE_NAME);
	PingInit();
    ParametrsMemFree();	
	
	ResetWDT();  
	Init_clock();
	//vPortExitCritical();
	xTaskResumeAll();	
}


void init_VFS(void)
{
	VFS_reg(&drivers);
    VFS_reg(&sensors_VFS);
	VFS_reg(&fat_at45db);
	VFS_reg(&saver_fs);
	
	VFS_mount("/at45db/");
#if(INIT_ON)

    FRESULT Res = f_mkfs(0,1,_FLASH_FAT_SECTOR_SIZE_);

#endif
    
}

void user_system_init(void)
{
	GPIO_SetDir(1, 1 << 22, 1); //STATUS_LED
    GPIO_SetDir(1, 1 << 19, 1); //ERROR_LED
	GPIO_SetDir(0, 1 << 6, 1); // Выбор симки
#ifdef _RS_232_
	GPIO_SetDir(2, 1 << 5, 1); //SE_RS
	GPIO_ClearValue(2, 1 << 5);
#endif

    GPIO_SetValue(1, 1 << 19); //Тушим ERROR_LED
	GPIO_ClearValue(2, 1 << 5); // 

	InitDriverSystem();

/*******************Открытиу требуемых устройств**************************/

#if(_SENSORS_)
    	InitDataFunctions();
    	InitPhysFunctions();
    	ReadSettingsFile(SENSOR_FILE_NAME);
    	InitSensorSystem();
		ParametrsMemFree();
#endif

#if(_EVENTS_)
		ReadSettingsFile(EVENTS_FILE_NAME);
		InitEvents();
		readUstValues();
		InitEventCheck();
		ParametrsMemFree();
#endif


	ReadSettingsFile(PARAMETRS_FILE_NAME);
#if(_SECURITY_)
    SecurityInit();
#endif

	
	MBCPortConfig();   // определяем порт для MB
	MBCMethodConfig(); // определяем, какой у нас клиент ModBus: TCP или удаленный COM
	initLCDContrast(); // читаем уровень контраста LCD	


#if(_SMS_)
    	InitSMS();
#endif //_SMS_

#if(_WDT_)
        InitWDT();
#endif

#if(_LOG_)
	InitLog();
#endif
	Init_clock();
#if _TIMERS_
        InitTimers();
#endif
	
	
	InitModBusReg();

	vlwIPInit();
#if(_PING_)
	PingInit();
#endif

#if _CLOCK_
	Init_clock();
#endif

	ParametrsMemFree();
//	ppp_new_con();

/************************************************************************/


/***************Создание прочих тасков***********************************/
#if _UART_CONTROL_TASK_
		xTaskCreate( UartControlTask, "UartControl", configMINIMAL_STACK_SIZE, NULL, DEVICE_TASK_PRIORITY_LOW , &UartControlHandle);
		if(UartControlHandle == 0) {PUT_MES_TO_LOG("Ошибка UartControl task", 0, 0); DEBUG_PUTS("UartControl_TASK ERROR\n");}
		else UartControlQueue = xQueueCreate(10, sizeof(COMMON_MESSAGE));
#endif
#if(_USB_)
	  	USBConfigureInterface(NULL);
#endif
#if(_MODEM_)
	  	xTaskCreate( ModemTask, "MODEM", configMINIMAL_STACK_SIZE + 64, NULL, DEVICE_TASK_PRIORITY_LOW, &hModemTask);
		if(hModemTask == 0) {PutMesToLog("MODEM_TASK ERROR", 0, 0); DEBUG_PUTS("MODEM_TASK ERROR\n");}
#endif
#if(_FTP_)
        FTPHandle = (xTaskHandle) CreateMyFTP(vFTPServer, "FTP", 256-64, NULL, DEVICE_TASK_PRIORITY_LOW );
        if(FTPHandle == 0) {PUT_MES_TO_LOG("Ошибка FTP task", 0, 0); DEBUG_PUTS("FTP_TASK ERROR\n");}
#endif
#if(_HTTP_)
        //xTaskCreate( httpTask, "HTTP", configMINIMAL_STACK_SIZE+80, NULL, DEVICE_TASK_PRIORITY_LOW, &HTTPHandle);
				HTTPHandle = (xTaskHandle) CreateMyHTTP("HTTP", configMINIMAL_STACK_SIZE+108, NULL, DEVICE_TASK_PRIORITY_LOW );
        if(HTTPHandle == 0) {PUT_MES_TO_LOG("Ошибка HTTP task", 0, 0); DEBUG_PUTS("HTTP_TASK ERROR\n");}
#endif
#if(_TELNET_)
        xTaskCreate( TelnetTask, "Telnet", configMINIMAL_STACK_SIZE+64, NULL, DEVICE_TASK_PRIORITY_LOW, &TelnetHandle);
        if(TelnetHandle == 0) {PUT_MES_TO_LOG("Ошибка Telnet task", 0, 0); DEBUG_PUTS("Telnet_TASK ERROR\n");}
#endif
#if(_EVENTS_)
        xTaskCreate( events_task, "Event", configMINIMAL_STACK_SIZE+64, NULL, DEVICE_TASK_PRIORITY_LOW, &EventHandle);
        if(EventHandle == 0) {PUT_MES_TO_LOG("Event_TASK ERROR", 0, 0); DEBUG_PUTS("Event_TASK ERROR\n"); }
#endif
#if(_ETH2COM_)
        xTaskCreate( e2c_task, "ETH2COM", configMINIMAL_STACK_SIZE + 64, NULL, DEVICE_TASK_PRIORITY_LOW, &ETH2COMHandle);
        if(ETH2COMHandle == 0) {PUT_MES_TO_LOG("ETH2COM_TASK ERROR", 0, 0); DEBUG_PUTS("ETH2COM_TASK ERROR\n");}
#endif


xTaskCreate( ResetTask, "RESET", configMINIMAL_STACK_SIZE - 64, NULL, DEVICE_TASK_PRIORITY_LOW + 5, &ResetTaskHandle);
if(ResetTaskHandle == 0) {PutMesToLog("BOD_TASK ERROR\n", 0, 0); DEBUG_PUTS("BOD_TASK ERROR\n");} 

init_wdt_cnt();

// Клиент ModBus через удаленный COM
if(GetMBCMethod() == 1)
{
#if _MBC_
	xTaskCreate( MBC_Task, "MBC", configMINIMAL_STACK_SIZE, NULL, DEVICE_TASK_PRIORITY_LOW , &MBCHandle);
	if(MBC_Task == 0) {PUT_MES_TO_LOG("Ошибка MBC task", 0, 0); DEBUG_PUTS("BOD_TASK ERROR\n");}
#endif
}
// Клиент ModBus TCP (он же по умолчанию)
else
{
#if _MB_TCP_
	xTaskCreate( ModbusTCPTask, "MB_TCP", configMINIMAL_STACK_SIZE, &MBPort, DEVICE_TASK_PRIORITY_LOW , &ModBusHandle);
	if(ModBusHandle == 0) {PUT_MES_TO_LOG("Ошибка MBTCP task", 0, 0); DEBUG_PUTS("BOD_TASK ERROR\n");}
#endif
}

/************************************************************************/
}

uint16_t proba = 0;
int PoolClock(void);
void UserTask( void *pvParameters )
{
  common_queue = xQueueCreate(SIZE_COMMON_QUEUE, sizeof(COMMON_MESSAGE));
	
	user_message_queue = common_queue;
	user_system_init();
	PoolClock();
	PowerOnLog();

    for(;;)
    {
      	common_queue_proc();
				WDT_cnt[USER_TASK_WDT_CNT].cnt = 0;
    }
}

void program_reset(void)
{
   vTaskDelay(10000);
	 AwakeResetTask(0, SMS_RESET_MES);
	 vTaskDelay(10000);
}



#if(_MBC_)

MB_SER_PORT_INFO MBCInfo = {.useRS485 = 0};
//void TCPRedirInit(void);
void MBC_Task( void *pvParameters )
{

	uint8_t flag = 0, *mbBuf = 0, Check_flag = 0;
	DEVICE_HANDLE hDevice = OpenDevice(pvParameters);
	MBCInfo.mb_driver = hDevice;
	DeviceIOCtl(hDevice, TCP_REDIR_SET_PORT, (uint8_t *)&MBPort);
	DeviceIOCtl(hDevice, TCP_REDIR_LISTEN, 0);
	for(;;)
	{
		DeviceIOCtl(hDevice, DEVICE_VALID_CHECK, (void *)&flag);
		if(flag)
		{
			mbBuf = MBSerialReceive(&MBCInfo, _MD_ID_, &Check_flag);		  //Смотрим, не пришел ли пакет по МВ
	    	if(Check_flag)
	    	{
	        	MBClientProcess(&MBCInfo, mbBuf+1, Check_flag, _MD_ID_); // Обрабатываем пакет от МВ	
	        	Check_flag = 0;
	    	}
			DeviceIOCtl(hDevice, POOL, 0);
		}
		delay(500);
	}
}
#endif

void PowerOnLog(void)
{
    if(RSID & RSID_BODR_MASK)
    {
        PUT_MES_TO_LOG("Устройство включено\rпосле POWER OFF", 0, 0);
        RSID |= RSID_BODR_MASK;
    }
    if(RSID & RSID_WDTR_MASK)
    {
        PUT_MES_TO_LOG("Устройство включено\rпосле WDT RESET", 0, 0);
        RSID |= RSID_WDTR_MASK;
    }
    if(RSID & RSID_EXTR_MASK)
    {
        PUT_MES_TO_LOG("Устройство включено\rпосле EXTERNAL RESET", 0, 0);
        RSID |= RSID_EXTR_MASK;
    }

	GPIO_SetDir(2, 1 << 3, 0);	// SIM 2
	// если нет sim
	if((GPIO_ReadValue(2) & (1 << 3)) > 0)
	{
		simPresentFlag = 0;
		PUT_MES_TO_LOG("SIM-карта отсутствует", 0, 0); 	
	}
}

void PutMesToLog(char *mes, uint8_t type, uint8_t level)
{
	AddLogMessage((uint8_t *)mes, type, level);
}

int PutMesToComQueue(uint8_t dst, uint8_t *privat_data, uint16_t data, uint32_t timeout)
{
	
    COMMON_MESSAGE message;

    message.dst= dst;
    message.privat_data = privat_data;
    message.data = data;
    return xQueueSendToBack(common_queue, ( void * )&message, ( portTickType )timeout);
}

#define STATUS_LED_ON()   GPIO_SetValue(1, 1 << 22)
#define STATUS_LED_OFF()  GPIO_ClearValue(1, 1 << 22)

void proc_led(void)
{
    static int temp = 1;

    if(temp)
    {
        STATUS_LED_ON();
        temp = 0;
    }
    else
    {
        STATUS_LED_OFF();
        temp = 1;
    }
}

/*void go_to_boot(void)
{
	void (*boot_jump)(void);

	boot_jump = (void (*)(void))0x30;

	boot_jump();
}*/  

__asm void go_to_boot(void)
{
	 MOV R0, #0x30
	 LDR PC, [R0]
}


//void stats_display(void);
void QueueProcEvent(void);
void checkSIM(void);

void common_queue_proc(void)
{
    DEVICE_HANDLE hHandle;
	
	COMMON_MESSAGE message;
//	int message_counter = 0;
//    message_counter = uxQueueMessagesWaiting(common_queue);
//    if(message_counter == 50)
//    {
//        PoolClock();    
//    }

    if(!xQueueReceive(common_queue, (void *)&message, portMAX_DELAY))
    {
        return;
    }

    switch(message.dst)
    {
//		case RESETE_TO_CLOCK_TIMER:
//			reset_to_clock();
//			break;		
		
		case DISCRETE_POOL_TIMER:
			hHandle = OpenDevice("DISCRETE");
			DeviceIOCtl(hHandle, POOL, 0);	
			CloseDevice(hHandle);
			break;

//		case BUTTON:
//			ResetClockTimer();
//
//			if(startInitFlag == 2)
//				switch_menu((uint8_t)message.data);
//		  
//			break;

//		case DISPLAY_UPDTAE_TIMER:
//			DisplayBlinkFlag ^= 0xff;
//
//			if(startInitFlag == 2)
//				update_menu(0);
//					
//			break;

//		case SIG_CHECK_TIMER:
//			sig_lev_func = (SIG_LEV_FUNK)&sig_level;
//			break;

		case CLOCK_POOL_TIMER:
			 PoolClock();
			 break;

        case SENSORS_POOL_TIMER:
			PoolSensor();
			CheckEvents();

			// до завершения инициализации ничего не проверяем
			if(startInitFlag && !initErrorFlag)
			{
            //	CheckSensors();
				flag_pos = 1;
			//	QueueProcEvent();

			}
            break;
        case SYSTEM_RESTART:
            program_reset();
            break;
		case DEV_REINIT:
                ReInitDevSystem();
             break;
//		case SMS_TIMER:
//			sms_funk = (SMS_FUNK)&sms_proces;
//            break;
		case TIME_SYNC_TIMER:
		//	synk_time_func = (SYNC_TIME_FUNK)&Sync_time_gsm;
            break;
			
		case BOOT_REQUEST:
			go_to_boot();
			break;
//#if(_PING_)
//		case PING_TIMER:
//			PingPool();
//			break;
//#endif

        default:
            break;
    }
}

/*******************************************************************************************************************************/
/****************************************   Функции счетчиков wdt и перезагрузки процессора    *********************************/
/*******************************************************************************************************************************/

	void ResetTask( void *pvParameters )
	{
		reset_task_queue = xQueueCreate(3, sizeof(RESET_TASK_MES));
		RESET_TASK_MES mes;

		xQueueReceive(reset_task_queue, (void *)&mes, portMAX_DELAY);

		AddLogMessage((uint8_t *)mes.data, 0, 0);
		OpenDevice(_FLASH_L_L_DRIVER_NAME_);
		while(1);
	}


	void AwakeResetTask(int isr_fl, char *data)
	{
		portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

		RESET_TASK_MES mes;
		mes.data = data;

		WDT_cnt[RESET_TASK_WDT_CNT].fl_on = 1;
		
		if(reset_task_queue == 0)	// еще не создана (прерывания уже работают
				while(1);
		
		if(isr_fl == 0)
			xQueueSendToBack(reset_task_queue, ( void * )&mes, ( portTickType )0);
		else 
			xQueueSendToBackFromISR(reset_task_queue, ( void * )&mes, &xHigherPriorityTaskWoken);			
	}

	void wdt_cnt_match(int cnt_num)
	{
		if(cnt_num == RESET_TASK_WDT_CNT){
			AddLogMessage("task R inv", 0, 0);
			OpenDevice(_FLASH_L_L_DRIVER_NAME_);
			while(1);	
		}
		else{
			AwakeResetTask(0, WDT_cnt[cnt_num].mes);
		}	
	}

	void wdt_cnt_check(void)
	{
		int i = 0;
		for(i = 0; i < WDT_COUNTERS_NUM; i++){
			if(WDT_cnt[i].fl_on){
				if(WDT_cnt[i].cnt > WDT_cnt[i].lim)
					wdt_cnt_match(i);
			}
		}	
	}

	void init_wdt_cnt(void)
	{
		WDT_cnt[USER_TASK_WDT_CNT].cnt = 0;	
		WDT_cnt[USER_TASK_WDT_CNT].lim = USER_TASK_WDT_LIM;
		WDT_cnt[USER_TASK_WDT_CNT].fl_on = 1;
		strcpy(WDT_cnt[USER_TASK_WDT_CNT].mes, USER_TASK_RESET_MES);
		
		WDT_cnt[MODEM_TASK_WDT_CNT].cnt = 0;	
		WDT_cnt[MODEM_TASK_WDT_CNT].lim = MODEM_TASK_WDT_LIM;
		WDT_cnt[MODEM_TASK_WDT_CNT].fl_on = 1;
		strcpy(WDT_cnt[MODEM_TASK_WDT_CNT].mes, MODEM_TASK_RESET_MES);
		
		WDT_cnt[RESET_TASK_WDT_CNT].cnt = 0;	
		WDT_cnt[RESET_TASK_WDT_CNT].lim = RESET_TASK_WDT_LIM;
		WDT_cnt[RESET_TASK_WDT_CNT].fl_on = 0;
		strcpy(WDT_cnt[RESET_TASK_WDT_CNT].mes, RESET_TASK_RESET_MES);		
	}

#if(configUSE_IDLE_HOOK)
	void vApplicationIdleHook(void)
	{
		#if(_WDT_LPC_)
			ResetWDT();
		#endif
			wdt_cnt_check();
	}
#endif
	
void checkSIM(void)
{
	// если симки нет, а прошлый раз была
	if((GPIO_ReadValue(2) & (1 << 3)) > 0 && simPresentFlag == 1)
	{
		// пишем в архив
		simPresentFlag = 0;
		PUT_MES_TO_LOG("SIM-карта отсутствует", 0, 0); 	
	}
	// если симка есть, а прошлый раз не было
	else if((GPIO_ReadValue(2) & (1 << 3)) == 0 && simPresentFlag == 0)
	{
		// пишем в архив
		simPresentFlag = 1;
		PUT_MES_TO_LOG("SIM-карта вставлена", 0, 0); 
	}
}

#if(_SAVER_)
void Saver_user_init(void)
{
	saver_info.seg[FLAGS_SEG].base_addr = (SEG_RAM_START_SECTOR + 0)*_SAVER_FLASH_PAGE_SIZE_;
	saver_info.seg[FLAGS_SEG].size = FLAGS_SEG_SIZE;
	saver_info.seg[FLAGS_SEG].rec_len = FLAGS_SEG_REC_LEN;
	saver_info.seg[FLAGS_SEG].crc_flag = FLAGS_SEG_CRC;
	saver_info.seg[FLAGS_SEG].curr_addr = 0;
	saver_info.seg[FLAGS_SEG].fin_addr = 0;

	saver_info.seg[LOG_SEG].base_addr = (SEG_RAM_START_SECTOR + FLAGS_SEG_SIZE)*_SAVER_FLASH_PAGE_SIZE_;
	saver_info.seg[LOG_SEG].size = LOG_SEG_SIZE;
	saver_info.seg[LOG_SEG].rec_len = LOG_REC_LEN;
	saver_info.seg[LOG_SEG].crc_flag = LOG_REC_CRC;
	saver_info.seg[LOG_SEG].curr_addr = 0;
	saver_info.seg[LOG_SEG].fin_addr = 0;

	saver_info.seg[LOG_COUNTERS_SEG].base_addr = (SEG_RAM_START_SECTOR + LOG_SEG_SIZE + FLAGS_SEG_SIZE)*_SAVER_FLASH_PAGE_SIZE_;
	saver_info.seg[LOG_COUNTERS_SEG].size = LOG_COUNTERS_SEG_SIZE;
	saver_info.seg[LOG_COUNTERS_SEG].rec_len = LOG_COUNTERS_SEG_REC_LEN;
	saver_info.seg[LOG_COUNTERS_SEG].crc_flag = LOG_COUNTERS_SEG_CRC;
	saver_info.seg[LOG_COUNTERS_SEG].curr_addr = 0;
	saver_info.seg[LOG_COUNTERS_SEG].fin_addr = 0;
	
	int i;
	for(i = 0; i < SEGMENT_NUMBER; i++)	 // Пропускаем сегмент для архива, тк счетчики адресов извесны
	{
		Saver_addr_init(i);
	}
}

#endif // _SAVER_
