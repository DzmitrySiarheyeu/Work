#include "MODEM_DRV.h"

#if(_MODEM_)

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uart_lpc17xx.h"
#include "sms.h"

#include "SetParametrs.h"
#include "User_Error.h"

#if(_EVENTS_)
#include "User_Events.h"
#include "Events.h"
#endif


/* Local variables */
static uint8_t  sim = 0;
static char  mbuf[128];                /* Modem Command buffer                  */
static uint8_t  modem_st = IDLE;                /* Modem state                           */

static int ModemGPRSConnect (DEVICE_HANDLE handle);
static int ModemCheckOperator(DEVICE_HANDLE handle);
static int proc_gprs_connect (const char *init_string, const char *dial_num);
static int proc_oper_check(int pin_flag, const char *pin);
static int power_on(void);
static void restart_modem(void);
static void send_cmd (char *str);

static DEVICE_HANDLE OpenModem(void);
static void CloseModem(DEVICE_HANDLE handle);
static int ReadModem(DEVICE_HANDLE handle, void * dst, int count);
static int WriteModem(DEVICE_HANDLE handle, const void * src, int count);
static int SeekModem(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int ReadModemText(DEVICE_HANDLE handle, void * dst, int count);
static int WriteModemText(DEVICE_HANDLE handle, void * src, int count);
static int SeekModemText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int ModemIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData);


MODEMData /*__attribute__ ((section (".fast_bss")))*/ modemDate = {
    .Signal = 0,
    .ConnectCount = 0,
    .Dial_Num = MODEM_DIAL_NUMBER,
    .Init_String = MODEM_INIT_STRING,
    .Login = PPP_USER,
    .Password = PPP_PASS,
    .PinEn = PIN_ENABLE,
    .Pin = PIN_VALUE,
	.synk_flag = 0,
	.Cur_Roaming = 0,
    .DataPoint = 0,
	.imei = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .DataPointText = 0,
	.OwnNum = "Unknown",
	.PinRequest = 0
    };


const DEVICE_IO modem_device_io = {
    .DeviceID = MODEM,
	.CreateDevice =	CreateModem,
	.CloseDevice = CloseModem,
    .OpenDevice = OpenModem,
    .ReadDevice = ReadModem,
    .WriteDevice = WriteModem,
    .SeekDevice = SeekModem,
    .ReadDeviceText = ReadModemText,
    .WriteDeviceText = WriteModemText,
    .SeekDeviceText = SeekModemText,
    .DeviceIOCtl = ModemIOCtl,
    .DeviceName = "MODEM",
    } ;

DEVICE modem_device = {
    .device_io = (DEVICE_IO *)&modem_device_io,
    .flDeviceOpen = 0,
    .pDriverData = &modem_info,
    .pData = &modemDate
    } ;


DEVICE_HANDLE OpenModem(void)
{
	DEVICE_HANDLE hUART;
    hUART = OpenDevice(_MODEM_L_L_DRIVER_NAME_);
    if(hUART == NULL)
    {
      DEBUG_PUTS("MODEM: unable to open UART\n");
	  PUT_MES_TO_LOG("MODEM: unable to open UART", 0, 1);
      return NULL;
    }

    modem_info.pUart = hUART;
	
	if(modem_device.flDeviceOpen == DEVICE_CLOSE)
	{
		 modem_device.flDeviceOpen = DEVICE_OPEN;
	}
	else
	{
		  DEBUG_PUTS("MODEM RE_OPEN\n");
	}

	return (DEVICE_HANDLE)&modem_device;   
}

void CloseModem(DEVICE_HANDLE handle)
{
	MODEM_INFO *pMODEMInfo = (MODEM_INFO *)(((DEVICE *)handle)->pDriverData);

	CloseDevice(pMODEMInfo->pUart);
	//pMODEMInfo->pUart = 0;
	
	if(modem_device.flDeviceOpen == DEVICE_OPEN)
	{
		 modem_device.flDeviceOpen = DEVICE_CLOSE;
	}
	else
	{
		  DEBUG_PUTS("MODEM RE_CLOSE\n");
	}
}


int ReadModem(DEVICE_HANDLE handle, void * dst, int count)
{
 

        MODEMData *pData = (MODEMData *)(((DEVICE *)handle)->pData);
        if(count >= 0) 
        {
            memset(dst, 0, count);
            if((pData->DataPoint + count) > MAX_MODEM_DATA_OFFSET)
            {
                count = MAX_MODEM_DATA_OFFSET - pData->DataPoint;
            }
            memcpy(dst, (uint8_t *)(((uint8_t *)pData) + pData->DataPoint), count);
            pData->DataPoint += count;
            return count;
        }
		return 0;
}

int SeekModem(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
        MODEMData *pData = (MODEMData *)(((DEVICE *)handle)->pData);
        switch(origin)
        {
            case 0:
                if(offset < MAX_MODEM_DATA_OFFSET)
                {
                    pData->DataPoint = offset;
                }
                else
                {
                    pData->DataPoint = 0;
                }
                break;
            case 1:
                if((pData->DataPoint + offset) < MAX_MODEM_DATA_OFFSET)
                {
                    pData->DataPoint = pData->DataPoint + offset;
                }
                else
                {
                    pData->DataPoint = 0;
                } 
                break;
            case 2:
                if(offset < MAX_MODEM_DATA_OFFSET)
                {
                    pData->DataPoint = MAX_MODEM_DATA_OFFSET - offset;
                }
                else
                {
                    pData->DataPoint = 0;
                } 
                break;
            default:
                pData->DataPoint = 0;
                return -1;                
        }
        return pData->DataPoint;
}



int ReadModemText(DEVICE_HANDLE handle, void * pDst, int count)
{
	MODEMData *pData = (MODEMData*)(((DEVICE *)handle)->pData);
	int sprintf_count;
	char *BufferText;
	
	if(count >= 0)
	{
		memset(pDst, 0, count);
		BufferText = pvPortMalloc(300);
		if(BufferText != NULL)
		{ 
			sprintf_count = sprintf(BufferText, "Signal level - %3u\n", pData->Signal);
			sprintf_count += sprintf(BufferText + sprintf_count, "IMEI - %s\n", pData->imei);
			sprintf_count += sprintf(BufferText + sprintf_count, "DialNum - %s\n", pData->Dial_Num);
			sprintf_count += sprintf(BufferText + sprintf_count, "Init string - %s\n", pData->Init_String);
			sprintf_count += sprintf(BufferText + sprintf_count, "Login - %s, Password - %s\n", pData->Login, pData->Password);
			sprintf_count += sprintf(BufferText + sprintf_count, "PinEn - %3u; PinReq - %3u\n", pData->PinEn, pData->PinRequest);
			sprintf_count += sprintf(BufferText + sprintf_count, "Pin - %s\n", pData->Pin);
			sprintf_count += sprintf(BufferText + sprintf_count, "Modem state - %3u\n", modem_st);
			sprintf_count += sprintf(BufferText + sprintf_count, "Last command - %s\n", pData->last_command);
			sprintf_count += sprintf(BufferText + sprintf_count, "Error - %s\n", pData->error_string);
				
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
	return 0;
}

int SeekModemText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
//    MODEM_INFO *pMODEMInfo = (MODEM_INFO *)(((DEVICE *)handle)->pDriverData);
        MODEMData *pData = (MODEMData*)(((DEVICE *)handle)->pData);
        switch(origin)
        {
            case 0:
                if(offset < MAX_MODEM_DATA_TEXT_OFFSET)
                {
                    pData->DataPointText = offset;
                }
                else
                {
                    pData->DataPointText = 0;
                }
                break;
            case 1:
                if((pData->DataPointText + offset) < MAX_MODEM_DATA_TEXT_OFFSET)
                {
                    pData->DataPointText = pData->DataPointText + offset;
                }
                else
                {
                    pData->DataPointText = 0;
                } 
                break;
            case 2:
                if(offset < MAX_MODEM_DATA_TEXT_OFFSET)
                {
                    pData->DataPointText = MAX_MODEM_DATA_TEXT_OFFSET - offset;
                }
                else
                {
                    pData->DataPointText = 0;
                } 
                break;
            default:
                //xSemaphoreGive(pMODEMInfo->Sem_data);
                pData->DataPointText = 0;
                return 0;                 
        }
        return pData->DataPointText;
}

int WriteModemText(DEVICE_HANDLE handle, void * src, int count)
{
  return -1;
}

int WriteModem (DEVICE_HANDLE handle, const void * src, int count)
{
//    MODEM_INFO *pMODEMInfo = (MODEM_INFO*)(((DEVICE *)handle)->pDriverData);
        MODEMData *pData = (MODEMData *)(((DEVICE *)handle)->pData);
        if(count >= 0) 
        {
            if((pData->DataPoint + count) > MAX_MODEM_DATA_OFFSET)
            {
                count = MAX_MODEM_DATA_OFFSET - pData->DataPoint;
            }
            memcpy((uint8_t *)(((uint8_t *)pData) + pData->DataPoint), src, count);
            pData->DataPoint += count;
            return count;
        }
		return 0;
}

int ModemIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData)
{
    MODEMData *pmData = (MODEMData *)(((DEVICE *)handle)->pData);

    switch(code)
    {
        case DIAL_NUMBER:
            strcpy((char *)pmData->Dial_Num, (char *)pData);
            break;
        case INIT_STR:
            strcpy((char *)pmData->Init_String, (char *)pData);
            break;
        case LOGIN_CON:
            strcpy((char *)pmData->Login, (char *)pData);
            break;
        case PASSWORD_CON:
            strcpy((char *)pmData->Password, (char *)pData);
            break;
        case PIN_EN:
            if(*(uint8_t *)pData == '0') pmData->PinEn = 0;
            else pmData->PinEn = 1;
            break;
        case PIN:
            strcpy((char *)pmData->Pin, (char *)pData);
            break;
        case ALL_SETTINGS:
			memcpy((char *)pmData, (char *)pData, (sizeof(MODEMData) - 4));
            /*if(ModemIOCtl(handle, DIAL_NUMBER, pData))                  return -1;
            if(ModemIOCtl(handle, INIT_STR, (uint8_t *)&pData[10]))     return -1;
            if(ModemIOCtl(handle, LOGIN_CON, (uint8_t *)&pData[50]))    return -1;
            if(ModemIOCtl(handle, PASSWORD_CON, (uint8_t *)&pData[60])) return -1;
            if(ModemIOCtl(handle, PIN_EN, (uint8_t *)&pData[70]))       return -1;
            if(ModemIOCtl(handle, PIN, (uint8_t *)&pData[71]))          return -1;	*/
            break;

        case RESET_DEVICE:
            modem_st = ERR;
            break;
		case MODEM_AUTOMATE_CTL:
            modem_automate(handle);
            break;
        case POOL:
          //  modem_pool(handle);
            break;
        default:
            return -1;
    }
    return 0;
}


/*--------------------------- init_modem ------------------------------------*/

void init_modem (void) {
   /* Initializes the modem variables and control signals DTR & RTS. */
   memset(mbuf, 0, sizeof(mbuf));
   modem_st = ERR;
}

/*--------------------------- Набор номера и установка GPRS соединения ------------------------------------*/

int ModemGPRSConnect (DEVICE_HANDLE handle)
{
	MODEMData *pModemData = (MODEMData *)(((DEVICE *)handle)->pData);
        
	modem_st = proc_gprs_connect(pModemData->Init_String, pModemData->Dial_Num);
    
	if(modem_st == GPRS_ON)
    {
        PUT_MES_TO_LOG("GPRS CONNECT", 2, 1);
        return DEVICE_OK; 
    }
    if(modem_st == ERR)
		PUT_MES_TO_LOG("MODEM ERROR", 2, 1);
    if(modem_st == POWER_ON)
		PUT_MES_TO_LOG("MODEM POWER ON", 2, 1);
	
	delay(2000);
    return DEVICE_OK;
}

/*------------------------------Проверка регистрации-----------------------------*/
 void phone(void);
int ModemCheckOperator(DEVICE_HANDLE handle) {
	static int counter = 0;
   
	MODEM_INFO *pModemInfo = (MODEM_INFO *)(((DEVICE *)handle)->pDriverData);
	MODEMData *pModemData = (MODEMData *)(((DEVICE *)handle)->pData);

	imei();
	modem_st = CHECK_REG;
	modem_st = proc_oper_check(pModemData->PinEn, pModemData->Pin); 
	delay(30);
    if(modem_st == OPERATOR_REG)
    {
//		#if(_AUTOGRAPH_)
//			SetFilter_1_Bits(ANTENNA_1);
//		#endif
		op_name();
		sig_level();
		find_out_own_num();
			
		if(Sync_time_gsm() == -1 || sms_proces() == -1)
		{
			modem_st = POWER_ON;
			delay(10000);
			DeviceIOCtl(pModemInfo->pUart, FLUSH_DEVICE, NULL);
			counter++;
	        if(counter > 6)
	        {
	            modem_st = ERR;
	            counter = 0;
	         }
				return DEVICE_OK;
		}
		sim = 1;
        PUT_MES_TO_LOG("OPERATOR REGISTRATION", 2, 0);
        return DEVICE_OK;
	}
    if(modem_st == NO_SIM)
    {
		counter++;
        if(counter > 30)
		{
            modem_st = ERR;
            PUT_MES_TO_LOG("NO SIM", 2, 1);
            counter = 0;
		}
		sim = 0;
		return DEVICE_OK;
	}
    if(modem_st == NO_PIN)
    {
		counter++;
        if(counter > 30)
        {
            modem_st = ERR;
            counter = 0;
		}
        PUT_MES_TO_LOG("NO PIN", 2, 1);
		sim = 1;
        return DEVICE_OK;
	}
    return DEVICE_OK;
}

int ReadLineEx(char *buf, int len, uint32_t delay, int no_ok)
{
	uint32_t timeout = 0, count = 0;
	DeviceIOCtl(modem_info.pUart, GET_RX_TIMEOUT, (uint8_t *)&timeout);
	DeviceIOCtl(modem_info.pUart, SET_RX_TIMEOUT, (uint8_t *)&delay);
	memset(buf, 0, len);
	len--; //Последний символ в любом случае должен быть 0 - конец строки
	do
	{
		count += ReadDevice(modem_info.pUart, buf + count, len - count);
		if(count == 0)
				break;
		while(*buf == '\r' || *buf == '\n')
		{
			memmove(buf, buf + 1, len); //len - 1 уже учтено!!!!
			if(count > 0) count--;
		}
		// любая положительно выполнившаяся команда должна заканчиваться ОК
		if(count > 3 && memcmp("OK\r\n", &buf[count - 4], 4) == 0)
			break;
		// Ошибка
		if(count > 0 && memcmp("ERROR\r\n", buf, strlen("ERROR\r\n")) == 0)
			break;
		if(count > 0 && memcmp("+CME ERROR", buf, strlen("+CME ERROR")) == 0 && buf[count - 1] == '\n')
			break;
		// сообщение при включении питания
		if(count > 2 && memcmp("RDY", buf, strlen("RDY")) == 0)
			break;
		// Сообщение при переходе в режим передачи данных
		if(count > 0 && memcmp("CONNECT\r\n", buf, strlen("CONNECT\r\n")) == 0)
			break;
		// нам нужно просто строку без извратов
		if(no_ok)
		{
			if(count > 1 && memcmp("\r\n", &buf[count - 2], 2) == 0)
				break;
		}
		DeviceIOCtl(modem_info.pUart, GET_STATUS_DEVICE, (uint8_t *)&delay);
	}while(delay != UART_STATUS_TIMEOUT && count < len);
	DeviceIOCtl(modem_info.pUart, SET_RX_TIMEOUT, (uint8_t *)&timeout);
	return count;
}

char * ReadLine(uint32_t delay)
{
	ReadLineEx(mbuf, sizeof(mbuf), delay, 0);
	return mbuf;
}

int cmd_get_parametrEx(char *command, char *answer, uint16_t delay, char *buf, int len)
{
	char *temp = (char *)buf;
	int n;
	snprintf(modemDate.last_command, sizeof(modemDate.last_command) - 1, "%s", command);
	DeviceIOCtl(modem_info.pUart, FLUSH_DEVICE, NULL);
	send_cmd (command);
	ReadLineEx(buf, len, delay, 0);
	temp = strstr(buf, answer);
	if (temp)
	{
		temp = temp + strlen(answer);
		while(*temp == '\r' || *temp == '\n') temp++;
		memmove(buf, temp, strlen(temp) + 1);
		return strlen(temp);
	}
	snprintf(modemDate.error_string, 9, "%s", command);
	n = strlen(modemDate.error_string);
	if(buf[0] == 0)
		temp = "TIMEOUT";
	snprintf(modemDate.error_string + n, 30 - n, " %s", buf);
	return 0;
}

char *cmd_get_parametr(char *command, char *answer, uint32_t delay)
{
	if(cmd_get_parametrEx(command, answer, delay, mbuf, sizeof(mbuf)))
		return mbuf;
	return NULL;
}

int cmd_operat(char *command, char *answer, uint32_t delay)
{
	char *temp = (char *)mbuf;
	int n;
	
	snprintf(modemDate.last_command, sizeof(modemDate.last_command) - 1, "%s", command);
	DeviceIOCtl(modem_info.pUart, FLUSH_DEVICE, NULL);
	send_cmd (command);
	
	temp = ReadLine(delay);
	if (strstr (temp, answer) != 0)
	{
		return 0;
	}
	snprintf(modemDate.error_string, 9, "%s", command);
	n = strlen(modemDate.error_string);
	if(temp[0] == 0)
		temp = "TIMEOUT";
	snprintf(modemDate.error_string + n, 30 - n, " %s", temp);
	return -1;
}

inline void Modem_state_set(uint8_t state)
{
    modem_st = state;
}

uint8_t Modem_state_get(void)
{
	return modem_st;
}

uint8_t Sim_state_get(void)
{
	return sim;
}

/*--------------------------- modem_online ----------------------------------*/

uint8_t modem_online (void) {
   /* Checks if the modem is online. Return false when not. */
   if (modem_st == GPRS_ON || modem_st == ON_LINE || modem_st == PPP_ON) {
      return (__TRUE);
   }
   return (__FALSE);
}


/*--------------------------- Посылка команды модему --------------------------------------*/

static void send_cmd (char *str) {
   /* Send Command Mode strings to Modem */
   WriteDeviceLn(modem_info.pUart, str);
}


/*--------------------------- Процидура подключения по GPRS -------------------------------------*/

static int proc_gprs_connect (const char *init_string, const char *dial_num) {
	char *temp;
	int i;
	sprintf(mbuf, "AT+CGDCONT=1,\"IP\",\"%s\"\r", init_string);
	if(cmd_operat(mbuf, "OK\r", 20000) != 0)
		return ERR;
	/*if(cmd_operat("AT+CGATT=1\r\n", "OK\r", 50000) != 0)
		return ERR;*/
	// ждем подключения к GPRS сети
	for(i = 0; i < 10; i++)
	{
		temp = cmd_get_parametr("AT+CGATT?\r\n", "+CGATT: ",3000);
		if(temp == NULL)
			return ERR;
		if(temp[0] == '1')
		{
			sprintf(mbuf, "ATD%s\r", dial_num);
			if(cmd_operat(mbuf, "CONNECT\r", 30000) == 0)
			{
				MODEM_DTR_ON();
				return GPRS_ON;
			}
			if(cmd_operat("ATH\r", "OK\r", 30000) != 0)
				return ERR;
		}
		delay(10000); 
	}
   	return ERR;
}

/*--------------------------Процедура проверки регестрации--------------------------------------*/
static int proc_oper_check(int pin_flag, const char *pin) {
  
	char *pin_state;
	int i;
	MODEM_DTR_ON();
	if(cmd_operat("ATZ\r", "",3000) != 0) //Сбросить все настройки в значения по умолчанию
		return ERR;
	if(cmd_operat("ATE0V1\r", "OK\r",5000) != 0)//ATE0V1\r\r\nOK\r",5000) != 0) //E0 -  выключить эхо, V1 - отведы в виде строк (OK, ERROR), не коды (0, 1, 2)
		return ERR;
	if(cmd_operat("AT&D1\r\n", "OK\r",5000) != 0)
		return ERR;
	if(cmd_operat("AT+CMEE=1\r\n", "OK\r",5000) != 0) //CME ERROR включен, ошибки в виде кодов
		return ERR;
	if(cmd_operat("AT+CSMINS?\r\n", "+CSMINS: 0,1\r",5000) != 0) //стутс сим карты
		return NO_SIM;\
    
#if _TIME_SYNC_GSM_ && _TIME_SYNC_CLTS_ == 0 
    if(cmd_operat("AT+CLTS=1\r\n", "OK\r", 2000) != 0)
        return ERR;
#endif
    i = 0;
    do
    {
        pin_state = cmd_get_parametr("AT+CPIN?\r\n", "+CPIN: ",5000); //нужен ли пин?
        i++;
    }while(pin_state == NULL && i < 100 && memcmp(mbuf, "+CME ERROR: 14", sizeof("+CME ERROR: 14") - 1) == 0);
	if(memcmp(pin_state, "SIM PIN", sizeof("SIM PIN") - 1) == 0)	// Да нужен
	{
		modemDate.PinRequest = 1;
		if(pin_flag)
		{
			sprintf(mbuf, "AT+CPIN=%s\r", pin);
			if(cmd_operat(mbuf, "OK\r",20000) != 0)
				return ERR;
		}
		else return NO_PIN;	
	}
	else if(memcmp("READY", pin_state, strlen("READY")) != 0)
	{
		modemDate.PinRequest = 1;
		return ERR;
	}
	if(cmd_operat("AT+CMGF=1\r", "OK\r", 5000) != 0)
        return ERR;
	// Ждем регистрации.
	for(i = 0; i < 10; i++)
	{
		pin_state = cmd_get_parametr("AT+CREG?\r\n", "+CREG: ",5000);
		if(pin_state == NULL)
			return ERR;
		pin_state += 2; //пропускаем стутус ответов + запятая
		if(pin_state[0] == '1')
		{
			return OPERATOR_REG;
		}
		if(pin_state[0] == '5' && modemDate.Roaming == 1)
		{
			modemDate.Cur_Roaming = 1;
			#if(_EVENTS_)
					PutEvenToQueue(E_ROAMING, NULL, NULL, 100);
			#endif
			return OPERATOR_REG;
		}
		if(pin_state[0] != '2' && pin_state[0] != '0')
			return ERR;
		delay(10000); 
	}
	return ERR;
}

static LPC_GPIO_TypeDef *GPIO_GetPointer(uint8_t portNum)
{
	LPC_GPIO_TypeDef *pGPIO = NULL;

	switch (portNum) {
	case 0:
		pGPIO = LPC_GPIO0;
		break;
	case 1:
		pGPIO = LPC_GPIO1;
		break;
	case 2:
		pGPIO = LPC_GPIO2;
		break;
	case 3:
		pGPIO = LPC_GPIO3;
		break;
	case 4:
		pGPIO = LPC_GPIO4;
		break;
	default:
		break;
	}

	return pGPIO;
}

void modem_turn_off(void)
{
		LPC_GPIO_TypeDef *pGPIO = GPIO_GetPointer(MODEM_EXT_POW_PORT); 
		if(!(pGPIO->FIODIR & (1 << MODEM_EXT_POW_PIN)))
		{
			PUT_MES_TO_LOG("MODEM: ext pow dir break", 0, 1);
		}
		if(GET_MODEM_SATUS() == 0)  //уже выключен
		{
			return;
		}
		POWER_ON_KEY_DOWN();
		delay(2500);    //задержка для выключения
		POWER_ON_KEY_UP();
		delay(3000);    //задержка как для выключения
		if(GET_MODEM_SATUS() > 0) // Не выключился, не реагирует на PWRKEY
		{
			PUT_MES_TO_LOG("MODEM: Status 1 not off", 0, 1);
			delay(3000);    //задержка как для выключения
			
			if(GET_MODEM_SATUS() > 0) // Не выключился, не реагирует на PWRKEY
			{
			PUT_MES_TO_LOG("MODEM: unable power off", 0, 1);
			delay(1000);	// чтобы сохранилось сообщение в лог
			EXT_POW_KEY_DOWN();
			delay(1000);	// чтобы сохранилось сообщение в лог
			if(GPIO_ReadValue(MODEM_EXT_POW_PORT) & (1 << MODEM_EXT_POW_PIN))
			{
				PUT_MES_TO_LOG("MODEM: ext pow set break", 0, 1);
			}
			delay(1000);
			EXT_POW_KEY_UP(); // для немодифицированных приборов
			delay(1000);
		}
		else
			PUT_MES_TO_LOG("MODEM: Status 1 off - ok", 0, 1);
		}
}

void AwakeResetTask(int isr_fl, char *data);

int power_on(void)
{
	// до передергивания повер кей вывод статус может иметь некорректное значение
    /*POWER_ON_KEY_DOWN();
	delay(2500);    //задержка для включения
	POWER_ON_KEY_UP();
	delay(3000);    //задержка как для включения*/
	char *temp;
	uint32_t baud = 57600;
    if(GET_MODEM_SATUS() > 0)  // Включен?
    	return POWER_ON;
    DeviceIOCtl(modem_info.pUart, FLUSH_DEVICE, NULL);
	// Упс, случайно выключили, включаем
	POWER_ON_KEY_DOWN();
	delay(2500);    //задержка для включения
	POWER_ON_KEY_UP();
	// Ожидает сообщения "RDY" от модема. Как только считает "RDY" сразу выйдет (см. код функции)
	// Если RDY нету - значит неправильный или автобаудрате
	// В этом случае нужно выждать как раз секунд пять до отправки новой команды
	ReadLineEx(mbuf, sizeof(mbuf), 5000, 1);
	
	// Выключаем эхо
	if(cmd_operat("ATE0V1\r", "OK\r",3000) != 0)
	{
		// Возможно новый модем, автобаудрате поддерживается только на скоростях до 57600
		DeviceIOCtl(modem_info.pUart, SET_BAUDRATE, (uint8_t *)&baud);
		if(cmd_operat("ATE0V1\r", "OK\r",3000) != 0)
		{
			baud = 115200;
			DeviceIOCtl(modem_info.pUart, SET_BAUDRATE, (uint8_t *)&baud);
			return ERR;
		}
	}
	
	temp = cmd_get_parametr("AT+IPR?\r\n", "+IPR: ",3000);
	if(temp == NULL) // нормально не включились
		return ERR;
	if (memcmp ("115200", temp, strlen("115200")) != 0)
	{
		if(cmd_operat("AT+IPR=115200\r\n", "OK\r",5000) != 0)
			return ERR;
			
		if(cmd_operat("AT&W\r\n", "OK\r",5000) != 0)
			return ERR;
		PUT_MES_TO_LOG("MODEM: Save 115200", 0, 1);
	}
	return POWER_ON;
}


void restart_modem(void)    //  Перезагрузка модема
{
	modem_turn_off();
	modem_st = IDLE;
	memset((char *)&modemDate.Op_name, 0, 20);
	modemDate.Signal = 0;
//#if(_AUTOGRAPH_)
//	ClrFilter_1_Bits(ANTENNA_1);
//#endif

}

void modem_automate(DEVICE_HANDLE handle)
{
	LPC_GPIO_TypeDef *pGPIO = GPIO_GetPointer(MODEM_EXT_POW_PORT);
    switch (modem_st)
    {
        case IDLE:
            modem_st = power_on();
			if(modem_st == ERR)
			{
				PUT_MES_TO_LOG(modemDate.error_string, 0, 1); //"MODEM: unable power on", 0, 1);
				delay(1000);	// чтобы сохранилось сообщение в лог
				if(!(pGPIO->FIODIR & (1 << MODEM_EXT_POW_PIN)))
				{
					PUT_MES_TO_LOG("MODEM: ext pow dir break", 0, 1);
				}
				EXT_POW_KEY_DOWN();
				delay(1000);	// чтобы сохранилось сообщение в лог
				if(GPIO_ReadValue(MODEM_EXT_POW_PORT) & (1 << MODEM_EXT_POW_PIN))
				{
					PUT_MES_TO_LOG("MODEM: ext pow set break", 0, 1);
				}
				delay(1000);	
				EXT_POW_KEY_UP(); // для немодифицированных приборов
				delay(1000);
				if(!(GPIO_ReadValue(MODEM_EXT_POW_PORT) & (1 << MODEM_EXT_POW_PIN)))
				{
					PUT_MES_TO_LOG("MODEM: ext pow clear break", 0, 1);
				}
				delay(1000);
				AwakeResetTask(0, "Modem power ON reset");
				
			}
            break;
        case ERR:
            restart_modem();
            break;
        case POWER_ON:
		case NO_SIM:
		case NO_PIN:
            ModemCheckOperator(handle);
            break;
        case OPERATOR_REG:
            ModemGPRSConnect(handle);
            break;
		case TURN_OFF:
			modem_turn_off();
        default:
            break;
    }
    // PutMesToModemQueue(MODEM_AUTOMATE, NULL);
}

int take_modem_from_online(void)
{
	if(modem_online() == 1)
	{
		MODEM_DTR_OFF();
		// Не знаю зачем но очень надо
		delay(100);
		ReadLineEx(mbuf, sizeof(mbuf), 2000, 0);
		
		//delay(200);
		//DeviceIOCtl(pModemInfo->pUart, FLUSH_DEVICE, NULL);
		if(cmd_operat("AT+CPIN=?\r\n", "OK\r", 2000) == 0) //cmd_operat("+++", "OK\r", 2000) == 0)
		{
			return 1;
		}
	    else
	    {
			return -1;
	    }
	}
	return 0;
}


int give_modem_to_online(void)
{
	if(modem_online() == 1)
	{
		MODEM_INFO *pModemInfo = &modem_info;
		DeviceIOCtl(pModemInfo->pUart, FLUSH_DEVICE, NULL);
		if(cmd_operat("ATO\r", "CONNECT\r",15000) == 0)
		{
			MODEM_DTR_ON();
			delay(100);
			return 1;
		}
		else
		{
			PUT_MES_TO_LOG("PPP CLOSING: Modem does not return from command mode", 2, 0);
			return -1;
		}
	}
	return 0;
}


int sms_check(char *buff)
{
	uint8_t i = 0;
	static int counter = 0;
	char *buf;
   
	if(modem_st == NO_PIN) return -1;

	sig_level();
  
	//if(cmd_operat("AT+CMGF=1\r\n", "OK\r", 3000) != 0)
	//	return -1;
	//delay(100); // нужна пауза, без нее "AT+CMGL=\"ALL\"\r" всегда отвечает только "ОК" - нет смс
	buf = cmd_get_parametr("AT+CMGL=\"ALL\"\r", "", 10000);  //+CMGL: " , 3000);
	if(memcmp ("+CMGL: ", buf, strlen("+CMGL: ")) == 0) //buf != NULL)
	{
		buf = buf + strlen("+CMGL: ");
		counter = 0;
		if(buf[0] == 0x0a)
		{
		return 0;
		}
		while(i < 4 && buf[i] != ',' && buf[i] != ')') i++;
		if(i >= 4)
		{
			return -1;
		}

		buf[i] = 0x00;

		if(buff == 0)
		{
			return -1;
		}
		if(buf[0] == 0x20) memcpy(buff, (buf + 1), i + 1);	// Если перед числом стоит символ пробела не копируем его
        else memcpy(buff, buf, i + 1);
		i = atoi((char *)buff);
		//вычитываем все остальные данные пока они есть
		while(ReadLineEx(mbuf, sizeof(mbuf), 100, 1));
		return i;
	} 
	else if(memcmp ("OK\r", buf, strlen("OK\r")) == 0) //)cmd_operat("AT+CMGL=\"ALL\"\r", "OK\r" , 3000) == 0)
	{
			counter = 0; return 0;   //  Если овечает ОК нет смс можем идти дальше
	}else
	{
			counter++;
			if(counter > 10)
			{
				modem_st = ERR;
			}
			return -1;	
	}
}

int sms_read(char *index, char *buff, uint16_t count)
{
	if(modem_st == NO_PIN) return -1;
  //if(cmd_operat("AT+CMGF=1\r", "OK\r", 1000) != 0) return -1;

  send_cmd("AT+CMGR=");
  send_cmd(index);
	delay (200);
	count = cmd_get_parametrEx("\r", "+CMGR:", 10000, buff, count);
  return count;
}

int sms_delete(char *index)
{
	if(modem_st == NO_PIN) return -1;
	
	delay(1000);
   // if(cmd_operat("AT+CMGF=1\r", "OK\r", 500) != 0) return -1;

	//delay(1000);
    send_cmd("AT+CMGD=");
	delay(1000);
    send_cmd(index);
	delay(1000);
    if(cmd_operat("\r", "OK\r", 60000) != 0) return -1;
    return 0;
}

//char cmdstr[100];
//char pdustr[2*SMS_MAX_PDU_LENGTH+4]; 

int sms_send(char *phon_num, char *buff)
{
//    int pdu_len;
		//int pdu_len_except_smsc;
	if(modem_st == NO_PIN)
		return -1;
    char crl_z[3] = { 0x1A, 0, 0 };
	//int c = 0;
    //if(cmd_operat("AT+CMGF=1\r", "OK\r", 500) != 0) return -1;
		
		/*pdu_len = pdu_encode("375297770000", phon_num + 1, buff, (unsigned char *)mbuf, sizeof(mbuf));
		if (pdu_len < 0)
			return -1;

		const int pdu_len_except_smsc = pdu_len - 1 - mbuf[0];
		snprintf(cmdstr, sizeof cmdstr, "AT+CMGS=%d\r", pdu_len_except_smsc);

		int i;
		for (i = 0; i < pdu_len; ++i) sprintf(pdustr+2*i, "%02X", (unsigned char)mbuf[i]);
		sprintf(pdustr+2*i, "%c\r\n", 0x1A);   // End PDU mode with Ctrl-Z. 	

		send_cmd(cmdstr);
		delay(500);
		if(cmd_operat(pdustr, "+CMGS:", 60000) != 0)
					return -1; 
		//cmd_get_parametr(pdustr);
	*/
	send_cmd("AT+CMGS=");
    send_cmd(phon_num);
	send_cmd("\r");
    delay(100);
    send_cmd(buff);
	delay(100);
	if(cmd_operat(crl_z, "+CMGS:", 60000) != 0)
		return -1;
	return 0;
}



int ussd_send(char *buf, int count)
{
	int err = -1;

	if(modem_st == NO_PIN) return err;

	send_cmd("AT+CUSD=1,\""); //"ATD ");//+CUSD=1,");
	send_cmd(buf);
	send_cmd("\"");
	delay (200);
	if(cmd_get_parametrEx("\r", "", 15000, buf, count))
	{
		if(memcmp(buf, "OK\r\n", sizeof("OK\r\n") - 1) == 0)
			memmove(buf, buf + 4, count - 4);
		if(*buf == 0)
			ReadLineEx(buf, count, 15000, 1);
		if(memcmp(buf, "+CUSD: 0,", sizeof("+CUSD: 0,") - 1) == 0)
		{
			err = 0;
			memmove(buf, buf + sizeof("+CUSD: 0,") - 1, count - (sizeof("+CUSD: 0,") - 1));
		}
	}
	
	return err;
}

int At_send(char *buf, uint16_t len)
{
	send_cmd(buf);
	len = cmd_get_parametrEx("\r", "", 2000, buf, len);
	return len;
}

void op_name(void)
{
#if(OPERATOR_NAME == OPTION_ON)
	char *buf, *temp;
	
	if(modem_st == NO_PIN) return;

	delay(100);
	buf = cmd_get_parametr("AT+CSPN?\r", "+CSPN: \"", 30000);
	if(buf == NULL)
	{
		return;
	} 

	temp = strchr(buf, '"');
	if(temp)
		*temp = 0;

	memset((char *)&modemDate.Op_name, 0, sizeof(modemDate.Op_name));
	memcpy((char *)&modemDate.Op_name, buf, sizeof(modemDate.Op_name) - 1);
#else
	memset((char *)&modemDate.Op_name, 0, sizeof(modemDate.Op_name));
	memcpy((char *)&modemDate.Op_name, "NO", 2);
#endif
}

int Sync_time_gsm(void)
{
#if _TIME_SYNC_GSM_
	char * buf;
	int count = 0;
    DEVICE_HANDLE hClock = NULL;

	if(Modem_state_get() < OPERATOR_REG)
        return 0;
	if(modemDate.synk_flag == 0)
        return 0;
#if _TIME_SYNC_CLTS_ == 0
    buf = cmd_get_parametr("AT+CCLK?\r", "+CCLK: \"", 2000);
#else
     buf = cmd_get_parametr("AT+CLTS\r", "+CLTS: \"", 2000);
#endif
    if(buf == NULL || strlen(buf) < 20)
    {
        return -1;
    }
    // проверяем что уже пришли данные о времени из сети
    if(buf[0] < '1') // нулевой год, еще не синхронизированы
        return 0;
#if _TIME_SYNC_CLTS_ == 0
    buf[18] = 0x30;	// в этой команде местное время а не по гринвичу, поправку не вносим
    buf[19] = 0x30;
#endif
    
	hClock = OpenDevice("Clock");
	if(hClock == NULL)
	{
		return -1;
	} 

	count = DeviceIOCtl(hClock, 12, (uint8_t *)buf);

	CloseDevice(hClock);
    modemDate.synk_flag = 0;

	return count;

#else	
	return 0;
#endif
}

void sig_level(void)
{
#if(SIGNAL_LEVEL == OPTION_ON)
	char *buf;
	
	if(modem_st < 6) return;

	buf = cmd_get_parametr("AT+CSQ\r", "+CSQ: ", 5000);
	if(buf == NULL)
	{
		return;
	} 

	modemDate.Signal = (uint8_t)atoi((char *)buf);
	if(modemDate.Signal > 32)
		modemDate.Signal = 0;
#endif
}
 
int find_out_own_num(void)
{
#if(OWN_DIAL_NUMBER == OPTION_ON)
	int i = 0, len = 0;
	
	if(modem_st == NO_PIN) return -1;
	strcpy(mbuf, (char *)&modemDate.OwnNumUSSD); 
	if(ussd_send(mbuf, sizeof(mbuf)) != 0)
		return -1;
	len = strlen(mbuf);
	while(i < len)
	{
		if(mbuf[i] == '3')
		{
			if(strncmp(&mbuf[i], "375", 3) == 0)
			{
				memcpy((char *)&modemDate.OwnNum[1], &mbuf[i], 12);
				modemDate.OwnNum[0] = '+';
				break;	
			}
		}
		i++;
	}

	if(i < len) return 0;
	else
	{
		return -1;
	}
#else
	return 0;
#endif	
} 


void SmsSend(uint32_t num)
{
	if(num == 0) return;

	char *mes, *phone,
	*number = (char *)num;
	int len = 0;

	ReadSettingsFile(MESSAGES_FILE_NAME);
	if(GetSettingsFileStatus() == 0) 
		goto exit;


	mes = (char *)GetPointToElement("MES", number);
	phone = (char *)GetPointToElement("PHONE", "p");

	if(mes == 0 || phone == 0) 
		goto exit;

	len = strlen(mes);
	if(len <= 0 || len > MAX_MESSAGE_SIZE)
		goto exit;

	len = strlen(phone);
	if(len <= 5 || len > 30)
		goto exit;

	sms_send(phone, mes);
exit:	
	ParametrsMemFree();

}

#if(IMEI == OPTION_ON)
int GetIMEI(char *buf)
{
	if(modemDate.imei[0] == 0) return -1;
	memcpy(buf, modemDate.imei, 16);
	return 0;
}
#endif

void imei(void)
{
#if(IMEI == OPTION_ON)
	char *buf;
	if(modemDate.imei[0] != 0 ) return;
	if(modem_st < POWER_ON) return;

	//send_cmd("AT+GSN\r"); 
	buf = cmd_get_parametr("AT+GSN\r", "", 3000);
	if(buf == NULL || strlen(buf) < 23) // 23 = (15 imei) + "\r\n\r\nOK\r\n";
		return;
	// заглушка от *PSUTZ кода
	buf = buf + strlen(buf) - 23; // 19 = (15 imei) + "\r\nOK";
	memcpy(modemDate.imei, buf, 15);
	modemDate.imei[15] = 0;	// конец строки
#endif
}



#if(OPERATOR_NAME == OPTION_ON)
	uint8_t * GetOpName(void)
	{
		return 	(uint8_t *)(&modemDate.Op_name);
	}
#endif


#if(SIGNAL_LEVEL == OPTION_ON)
	uint8_t GetSigLevel(void)
	{
		return 	modemDate.Signal;
	}
#endif

#if(OWN_DIAL_NUMBER == OPTION_ON)
	uint8_t * GetSimNum(void)
	{
		return 	(uint8_t *)(&modemDate.OwnNum);
	}
#endif

uint8_t GetRoamingState(void)
{
	return 	modemDate.Cur_Roaming;	
}

float GetExtPowLim(void)
{
	return 	modemDate.ext_pow_lim;	
}


void SetModemSettings(uint8_t flag)
{						   
    uint8_t *p_data;
    
    if(GetSettingsFileStatus())
    {
		if(flag == 0)
		{
			p_data = (uint8_t *)GetPointToElement("MODEM", "Sim");
	        if(p_data == 0)
	        {
	             AddError(NULL);
	        }
	        else
			{
				modemDate.cur_sim = atoi((char *)p_data);
	
	
				if(modemDate.cur_sim == 1)
				{
					if(!IS_F_SIM_NO_PRESENT())		  //  Если первая симка вставлена 
						{F_SIM_ON(); modemDate.cur_sim = 1;}					   // подключаем ее
					else if(!IS_S_SIM_NO_PRESENT())	  //  Если с первой симкой не вышло, проверяем вторую и подключаем
						{S_SIM_ON(); modemDate.cur_sim = 2;}		
				}
	
				else if(modemDate.cur_sim == 2)
				{
					if(!IS_S_SIM_NO_PRESENT())
						{S_SIM_ON(); modemDate.cur_sim = 2;}
					else if(!IS_F_SIM_NO_PRESENT())
						{F_SIM_ON(); modemDate.cur_sim = 1;}	
				}
	
				else
				{
					if(!IS_F_SIM_NO_PRESENT())
						{F_SIM_ON(); modemDate.cur_sim = 1;}
					else if(!IS_S_SIM_NO_PRESENT())
						{S_SIM_ON(); modemDate.cur_sim = 2;}	
				}
			}
		}
		if(modemDate.cur_sim == 1) p_data = (uint8_t *)GetPointToElement("MODEM", "DialNum_1");
        else p_data = (uint8_t *)GetPointToElement("MODEM", "DialNum_2");
        if(p_data == 0)
        {
             AddError(NULL);
        }
        else strcpy((char *)modemDate.Dial_Num, (char *)p_data); 

#if(OWN_DIAL_NUMBER == OPTION_ON)
		p_data = (uint8_t *)GetPointToElement("MODEM", "OwnNumUssd");
        if(p_data == 0 || strlen((char *)p_data) == 0 || strlen((char *)p_data) > 19)
        {
             AddError(NULL);
        }
        else strcpy((char *)modemDate.OwnNumUSSD, (char *)p_data);
#endif
        
        if(modemDate.cur_sim == 1) p_data = (uint8_t *)GetPointToElement("MODEM", "InitString_1");
		else p_data = (uint8_t *)GetPointToElement("MODEM", "InitString_2");
        if(p_data == 0)
        {
             AddError(NULL);
			 strcpy((char *)modemDate.Init_String, "AT+CGDCONT=1,\"IP\",\"abc\"");
        }
        else{
			if(p_data[18] == '"' && p_data[19] == '"') strcpy((char *)modemDate.Init_String, "AT+CGDCONT=1,\"IP\",\"abc\"");
			else strcpy((char *)modemDate.Init_String, (char *)p_data);	
		}  

        if(modemDate.cur_sim == 1) p_data = (uint8_t *)GetPointToElement("MODEM", "Login_1");
		else p_data = (uint8_t *)GetPointToElement("MODEM", "Login_2");
        if(p_data == 0)
        {
             //AddError(NULL);
			 //strcpy((char *)modemDate.Login, "abc");
			 modemDate.Login[0] = 0;
        }
		else{
			if(strlen((char *)p_data) == 0) modemDate.Login[0] = 0;
			else strcpy((char *)modemDate.Login, (char *)p_data);	
		} 

        if(modemDate.cur_sim == 1) p_data = (uint8_t *)GetPointToElement("MODEM", "Password_1");
		else p_data = (uint8_t *)GetPointToElement("MODEM", "Password_2");
        if(p_data == 0)
        {
             //AddError(NULL);
			// strcpy((char *)modemDate.Password, "abc");
			 modemDate.Password[0] = 0;
        }
		else{
			if(strlen((char *)p_data) == 0) modemDate.Password[0] = 0;
			else strcpy((char *)modemDate.Password, (char *)p_data);	
		}

        if(modemDate.cur_sim == 1) p_data = (uint8_t *)GetPointToElement("MODEM", "PinEn_1");
		else p_data = (uint8_t *)GetPointToElement("MODEM", "PinEn_2");
        if(p_data == 0)
        {
             AddError(NULL);
        }
        else
		{
			if(*(uint8_t *)p_data == '0') modemDate.PinEn = 0;
            else modemDate.PinEn = 1;
		}

		p_data = (uint8_t *)GetPointToElement("MODEM", "Sync");
		if(p_data == 0)
        {
             AddError(NULL);
        }
		else modemDate.synk_flag = atoi((char *)p_data);

		//memset(modemDate.imei, 0, 16);
		p_data = (uint8_t *)GetPointToElement("MODEM", "IMEI");
		if(p_data != 0)
		{
			if(strlen((char *)p_data) == 15)
				memcpy(modemDate.imei, (char *)p_data, 15);
		}

        if(modemDate.cur_sim == 1) p_data = (uint8_t *)GetPointToElement("MODEM", "Pin_1");
		else p_data = (uint8_t *)GetPointToElement("MODEM", "Pin_2");
        if(p_data == 0)
        {
             AddError(NULL);
        }
        else strcpy((char *)modemDate.Pin, (char *)p_data);

		p_data = (uint8_t *)GetPointToElement("MODEM", "Roaming");
		if(p_data == 0)
        {
             modemDate.Roaming = 0;
        }
		else modemDate.Roaming = atoi((char *)p_data);

		p_data = (uint8_t *)GetPointToElement("MODEM", "ExPow");
		if(p_data == 0)
        {
             modemDate.ext_pow_lim = 8.0;
        }
		else modemDate.ext_pow_lim = atof((char *)p_data);
    }
    else AddError(NULL);  
}

#endif // _MODEM_
