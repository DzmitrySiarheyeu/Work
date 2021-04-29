#include "config.h"

#if(_UART_CONTROL_TASK_)

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "UartControlTask.h"
#include "driverscore.h"
#include "uart_lpc17xx.h"
#include "SetParametrs.h"
#if(_MBC_)
	#include "User_ModBus.h"
#endif

//#include "User_ModBus.h"
#include "MB_serial_port.h"
#if(_MBS_)
	#include "MB_server_device.h"
#endif
#if _OMS_
	#include "OMN_server_device.h"
#endif
#if(_ZM_CAMERA_)
	#include "zm_camera.h"
#endif

#if(OMN_DEV == DEVICE_ON)
	void OMNDeviceSend(void);
#endif

/****************     ���������     ****************/
static void procS485(uint8_t *cmdBuf);
static void procMBS_RW(uint8_t *cmdBuf);
static void procTerminal(uint8_t *cmdBuf, uint8_t ans_max_len);

/**************** ������� ��������� ****************/
extern xQueueHandle UartControlQueue;
extern xQueueHandle SMSQueue;


/****************   ModBus ������   ****************/
#if(_MBS_)
	extern MB_SER_PORT_INFO MBSInfo;
	void PoolMBS(void);
#endif


/****************   UserUartTask    ****************/
void UartControlTask( void *pvParameters )
{
	COMMON_MESSAGE commMes;
	int err = 0;
#if(_MBS_)
	MBSBusSerPortInit(&MBSInfo);

	// ��������� ����� ����� �������� MBS
	commMes.dst = MBS_TIMER;
	xQueueSendToBack(UartControlQueue, &commMes, 0);
#endif


	for(;;)
	{
		if(!xQueueReceive(UartControlQueue, (void *)&commMes, portMAX_DELAY))
    	{
    		continue;
    	}

		delay(500);
	
		switch(commMes.dst)
        {
			case S485:
				// ������������
				procS485(commMes.privat_data);

				commMes.dst = 0;
				// �������� ��������� � �����������
				xQueueSendToBack(SMSQueue, &commMes, 0);	
				break;
#if(_MBS_)
			case MBS_RW:
				// ������������
				procMBS_RW(commMes.privat_data);

				commMes.dst = 0;
				// �������� ��������� � �����������
				xQueueSendToBack(SMSQueue, &commMes, 0);
				break;

			case UART_TERM:
				// ������������
				procTerminal(commMes.privat_data, (uint8_t)commMes.data);

				commMes.dst = 0;
				// �������� ��������� � �����������
				xQueueSendToBack(SMSQueue, &commMes, 0);
				break;
#endif
            case MBS_TIMER:
#if(OMN_DEV == DEVICE_ON)
						OMNDeviceSend();
#endif
#if(_MBC_)
				RegModBusPool();
#endif
#if(_MBS_)
                PoolMBS();
#endif
#if(_OMS_)
				PoolOMS();
#endif
				commMes.dst = MBS_TIMER;
				xQueueSendToBack(UartControlQueue, &commMes, 0);
                break;

#if(_ZM_CAMERA_)
			case SNAPSHOT:
				// ������������
				err = ZMCameraSnap("BCD0", (char *)commMes.privat_data);
			//	if(commMes.privat_data != 0)
			//	{
			//		if(err == 0)
			//			memcpy(commMes.privat_data, "OK", 3);
			//		else memcpy(commMes.privat_data, "ERROR", 6);
	
					commMes.dst = 0;
					// �������� ��������� � �����������
					xQueueSendToBack(SMSQueue, &commMes, 0);
			//	}
				break;
#endif
			
            default:
                break;
        }
	}
}

static void procS485(uint8_t *cmdBuf)
{
	int arrayLen, ansLen, tOut, baudRate, len = 0, colonCnt = 0, readLen;
	char *ansBuf, *array = (char *)cmdBuf;
	DEVICE_HANDLE hUart;
	uint32_t def_timeout = 0;
	
	// ����� �������, ����� ������, ����-���, ��������
	sscanf((const char *)cmdBuf, "= %d:%d:%d:%d", &arrayLen, &ansLen, &tOut, &baudRate);

	if(arrayLen <= 0 || ansLen <= 0 || tOut < 0 || baudRate <= 0)
		goto error_exit;
	
	// ���� ��� ������
	// ������ ����� ����� ���������� ���������
	while(colonCnt != 4)		
	{
		while(*array != ':' && *array != 0)	
			array++;

		if(*array == 0)
			break;

		array++;
	    colonCnt++;
	}

	if(colonCnt != 4)
		goto error_exit;

	// ������ ���� ������������� � ����������� ����	(����� 1 ���� �������� � ������ 2 hex-�����)
	while(len < arrayLen)
	{
		char hexStr[3];

		// �� 2 hex-�����
		memcpy(hexStr, array + len * 2, 2);
		hexStr[2] = 0;

		// ������� 1 ����
		int hexNumber; 
		sscanf(hexStr, "%X", &hexNumber);

		// ������ ��� ��� �� �����
		cmdBuf[len++] = (uint8_t )hexNumber;
	}

	// ������������� UART
	hUart = OpenDevice(UART_CONTROL_DRIVER_NAME);
	if(!hUart)
		goto error_exit;

	DeviceIOCtl(hUart, GET_RX_TIMEOUT, (uint8_t *)&def_timeout);
	// �������� ��������
	DeviceIOCtl(hUart, SET_BAUDRATE, (uint8_t *)&baudRate);
	// �������� ����-���
	DeviceIOCtl(hUart, SET_RX_TIMEOUT, (uint8_t *)&tOut);

	// ����� ��� ������
	ansBuf = (char *)pvPortMalloc(ansLen);
	if(!ansBuf)
	{
		CloseDevice(hUart);
		goto error_exit;
	}

#if _MB_DE_CONTROL_
    	SET_DE485();
#endif
	// �����
	WriteDevice(hUart, cmdBuf, arrayLen);
#if _MB_DE_CONTROL_
		CLR_DE485();
#endif
	// ������
	readLen = ReadDevice(hUart, ansBuf, ansLen);

	// ���������� �����
	DeviceIOCtl(hUart, SET_DEF_BAUDRATE, 0);
	DeviceIOCtl(hUart, SET_RX_TIMEOUT, (uint8_t *)&def_timeout);

	CloseDevice(hUart);

	// ��������� �����
	sprintf((char *)cmdBuf, "S485 = %d:", readLen);
	// ���������� �������� ������
	len = 0;
	while(len != readLen)
	{
		char hexStr[3];

		sprintf(hexStr, "%02X", (uint8_t)ansBuf[len++]);
		strcat((char *)cmdBuf, hexStr);
	}

	vPortFree(ansBuf);
	return;

error_exit:
	
 	strcpy((char *)cmdBuf, "ERROR");
}


#if(_MBS_)
static void procMBS_RW(uint8_t *cmdBuf)
{
	int baud, addr, id, res;
	MB_RW_STRUCT rwStruct;	// ������/������ ����� MBS
	DEVICE_HANDLE hMBS, hUart;

	// ������ ��� ������ ���������, id, ������ � ��������
	char searchTemplate[sizeof(":%d:%d:%d:%d;") + 1] = ":%d:%d:%d";

	// ������ ���������� "= "
	char *buf = (char *)(cmdBuf + 2);

	// ���� ��� �������� ('r'/'w')
	int len = strchr(buf, ':') - buf;
	if(len == 0)
		goto error_exit;

	// ���������� ��������
	if(!strncmp(buf, "r", len))
		rwStruct.opType = 'r';	// ������
	else if(!strncmp(buf, "w", len))	
		rwStruct.opType = 'w'; // ������
	else
		goto error_exit;

	// ���������� ��� ������� + ":"
	buf += 2;

	// ���� ��� �������� ('u'/'l'/'f')
	len = strchr(buf, ':') - buf;
	if(len == 0)
		goto error_exit;

	// ���������� ��� ��������
	if(!strncmp(buf, "u", len))
		rwStruct.valType = 'u';	// unsigned
	else if(!strncmp(buf, "l", len))	
		rwStruct.valType = 'l';   // long
	else if(!strncmp(buf, "f", len))	
		rwStruct.valType = 'f';   // float
	else
		goto error_exit;

	// ���������� ��� ��������
	buf += 1;

	// ����������� ������ ��� ������
	if(rwStruct.opType == 'r')
	{
		strcat(searchTemplate, ";");
	}
	else
	{
		switch(rwStruct.valType)
		{
			case 'u':
			case 'l':
				strcat(searchTemplate, ":%d;");
				break;
			case 'f':
				strcat(searchTemplate, ":%f;");
				break;
		}	
	}

	// ���� baud, id, ����� � �������� (���� ����)
	sscanf(buf, searchTemplate, &baud, &id, &addr, rwStruct.valBuf);

	if(addr < 0 || id < 0 || id > 255 || baud <= 0)
		goto error_exit;

	rwStruct.addr = (uint16_t)addr;
	rwStruct.id = (uint8_t)id;

	// ������������� UART
	hUart = OpenDevice(UART_CONTROL_DRIVER_NAME);
	if(!hUart)
		goto error_exit;

	// �������� ��������
	DeviceIOCtl(hUart, SET_BAUDRATE, (uint8_t *)&baud);

	hMBS = OpenDevice("MBS");
	if(!hMBS)
	{
		CloseDevice(hUart);
		goto error_exit;
	}

	// ��������� �������� ������/������
	res = DeviceIOCtl(hMBS, MB_SERVER_RW, (uint8_t *)&rwStruct);
	CloseDevice(hMBS);

	// ���������� �����
	DeviceIOCtl(hUart, SET_DEF_BAUDRATE, 0);
	CloseDevice(hUart);
	
	if(res < 0)	
		goto error_exit;

	// �����
	if(rwStruct.opType == 'w')
	{
		strcpy((char *)cmdBuf, "MBS = OK");
	}
	else
	{
		switch(rwStruct.valType)
		{
			case 'u':
				sprintf((char *)cmdBuf, "MBS = 0x%04X", *(uint16_t *)rwStruct.valBuf);
				break;
			case 'l':
				sprintf((char *)cmdBuf, "MBS = 0x%08X", *(uint32_t *)rwStruct.valBuf);
				break;
			case 'f':
				sprintf((char *)cmdBuf, "MBS = %f", *(float *)rwStruct.valBuf);
				break;
		}	
	}

	return;

error_exit:
	
 	strcpy((char *)cmdBuf, "ERROR");
}

void procTerminal(uint8_t *cmdBuf, uint8_t ans_max_len)
{
	int baud = 0, tout = 0, len = 0, tempLen = 0, defTout, readLen;
	uint8_t *ansBuf;
	DEVICE_HANDLE hUart;

	// ������ ���������� "= "
	char *buf = (char *)(cmdBuf + 2);
	char *temp = buf;

	// ���� baudrate
	while(*temp != ':' && *temp != 0)
		temp++;
	if(*temp == 0 || temp == buf)
		goto error_exit;
	
    *temp = 0;
	// baud
	baud = atoi(buf);
	if(baud <= 0)
		goto error_exit;
	
	// ��� ������ ���� timeout	
	buf = ++temp;	
	// ���� timeout
	while(*temp != ':' && *temp != 0)
		temp++;
	if(*temp == 0 || temp == buf)
		goto error_exit;

	*temp = 0;
	// timeout
	tout = atoi(buf);
	if(tout < 0)
		goto error_exit;

	// ��� ������ ���� �����	
	buf = ++temp;	
	// ���� �����
	while(*temp != ':' && *temp != 0)
		temp++;
	if(*temp == 0 || temp == buf)
		goto error_exit;

	*temp = 0;
	len = atoi(buf) / 2;
	if(len <= 0)
		goto error_exit;

	// ��� ������ ���� ��� ������ ��� ��������
	buf = ++temp;	
	tempLen = 0;
	// ������ ���� ������������� � ����������� ����	(����� 1 ���� �������� � ������ 2 hex-�����)
	while(tempLen < len)
	{
		char hexStr[3];

		// �� 2 hex-�����
		memcpy(hexStr, temp, 2);
		hexStr[2] = 0;

		// ������� 1 ����
		int hexNumber; 
		sscanf(hexStr, "%X", &hexNumber);

		// ������ ��� ��� �� �����
		*(buf + tempLen) = (uint8_t )hexNumber;
		tempLen++;
		temp += 2;
	}

	// ������������� UART
	hUart = OpenDevice(UART_CONTROL_DRIVER_NAME);
	if(!hUart)
		goto error_exit;

	DeviceIOCtl(hUart, FLUSH_DEVICE, 0);
	// ������ ������� �������														  	
	DeviceIOCtl(hUart, GET_RX_TIMEOUT, (uint8_t *)&defTout);
	// ������������� �������
	DeviceIOCtl(hUart, SET_RX_TIMEOUT, (uint8_t *)&tout);
	// ������������� baud
	DeviceIOCtl(hUart, SET_BAUDRATE, (uint8_t *)&baud);

#if _MB_DE_CONTROL_
   	SET_DE485();
#endif

	// ����� ������
	WriteDevice(hUart, buf, len);

#if _MB_DE_CONTROL_
	CLR_DE485();
#endif

	// ����� ��� ������
	ansBuf = (uint8_t *)pvPortMalloc(ans_max_len / 2 - 1);
	if(!ansBuf)
	{
		CloseDevice(hUart);
		goto error_exit;
	}
	
	// ������ �����
	readLen = ReadDevice(hUart, ansBuf, ans_max_len / 2 - 1);

	// ������������� ������ �������
	DeviceIOCtl(hUart, SET_RX_TIMEOUT, (uint8_t *)&defTout);
	// ������������� ������ baud
	DeviceIOCtl(hUart, SET_DEF_BAUDRATE, 0);

	CloseDevice(hUart);

	// ��������� �������� ������ � ������ hex-��������
	len = 0;
	cmdBuf[0] = 0;
	while(len != readLen)
	{
		char hexStr[3];

		sprintf(hexStr, "%02X", (uint8_t)ansBuf[len++]);
		strcat((char *)cmdBuf, hexStr);
	}

	vPortFree(ansBuf);
	return;

error_exit:
	
 	strcpy((char *)cmdBuf, "ERROR");	
}

#endif //_MBS_

void InitUartControl(void)
{
	uint32_t uart_baudrate = 19200;
	uint8_t *p_data;
	if(GetSettingsFileStatus())
    {
		p_data = (uint8_t *)GetPointToElement("USER_UART", "BaudRate");
		if(p_data == 0 || strlen((char *)p_data) > 6 || strlen((char *)p_data) < 1)
        {
             uart_baudrate = 19200;
        }
		else uart_baudrate = atoi((char *)p_data);
	}

	DEVICE_HANDLE	hDev = OpenDevice(UART_CONTROL_DRIVER_NAME);
	DeviceIOCtl(hDev, SET_BAUDRATE, (uint8_t *)&uart_baudrate);
	CloseDevice(hDev);
}

#endif //
