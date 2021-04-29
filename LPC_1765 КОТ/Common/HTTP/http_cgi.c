#include "config.h"
#if(_HTTP_ == DEVICE_ON)

#include <stdlib.h>
#include <string.h>

#include "FreeRTOSConfig.h"
#include "main.h"
#include "ModemTask.h"

#include "api.h"
#include "http_server.h"
#include "http_cgi.h"
#include "security.h"
#include "cookie.h"

#include "SetParametrs.h"
#include "SensorCore.h"
#include "clock_drv.h"
#include "modem_drv.h"

#if (!defined (FM1100) && !defined (_FM_2200_))

	#ifndef _PIPELINES_
		#include "gps_drv.h"
		#include "owi_drv.h"
	#else
		#include "MB_server_device.h"
	#endif
#else
	#include "gps_drv.h"
#endif



#include "ff.h"    
#include "VFS.h"
#include "FreeRTOS.h"
#include "task.h"
#include "SMS.h"

void escape(char *text);

/*list of cgi's for this process*/
CGI_CMD CGI_CMD_ARRAY[]  = 
{
	CGI_LOGIN_PROCESS,
	CGI_SENSOR_PROCESS,
	CGI_CONFIG_PROCESS,
	CGI_INFO_PROCESS,
	CGI_FILE_PROCESS,
	CGI_DATETIME_PROCESS,
	CGI_REINIT_PROCESS
#if (!defined (FM1100) && !defined (_FM_2200_))
	#ifndef _PIPELINES_
	
		,CGI_CAMERA_PROCESS,
		CGI_TERMINAL_PROCESS,
		CGI_CANSNIFFER_PROCESS,
		CGI_DISP_PROCESS
	
	#else 
		,CGI_PIPELINES_PROCESS
	#endif
#endif
};

// ���� ��� ������� cookies
extern uint8_t Use_Cookies;
// ������� ��� ����� � ��������� uart
extern xQueueHandle HTTPQueue;
// ����� � �������� uart �������� ����� �������� SMS, ������� �������� ��������� �� ������
extern xQueueHandle modem_queue;

// �������� ������ � IP �����
char * GetOurIPStr(void);

/**
 * Implements a CGI call
 *
 * @param CGI name
 * @param argument to be used for CGI called function 
 * @return returned value by function linked to execute by POST request
 */
int 
CGI_parser(char *name, char *request)
{
     uint8_t i;

     for(i=0;i<CGI_MAX_COMMANDS;i++)
     {
        if(!strncmp(name, CGI_CMD_ARRAY[i].command, strlen(CGI_CMD_ARRAY[i].command)))//contains array
        {
			// ������������ MIME-��� ��������� ������:
			// 1 - ������ �����
			// 2 - �������� XML
            return CGI_CMD_ARRAY[i].func(request);//execute function if matches
        }
     }
     //CGI: no replacement was found!!!
     return 0;
}

/**************************************************************************************/
/*************************************** LOGIN ****************************************/
/**************************************************************************************/

static int
LOGIN_process(char *request)
{
	char login[MAX_LOGIN_LEN + 1], passw[MAX_PASSW_LEN + 1];
	memset(login, 0, sizeof(login));
	memset(passw, 0, sizeof(passw));

#if(_SECURITY_ == OPTION_OFF)
	
	// �������� ������������ ��� - ������ �������������� �� ��������� ��������
	strcpy(request, LOGIN_REDIR_STRING);
	return 1;

#endif
	
	// ������, ������ ����� ����������
	char *temp = strchr((char *)request, ' ');
	if(temp == 0)
	{
		strcpy(request, LOGIN_REDIR_STRING);
		return 1;	
	}

	// ������������ ������ � �����������
	*temp = '\0';
	temp = request;

	// ���� ������ � �����
	char *loginPos = strstr(temp, LOGIN_PARAM), *passwPos = strstr(temp, PASSW_PARAM);
	int loginLen = 0, passwLen = 0;
	
	if(loginPos == 0 || passwPos == 0)			
	{
		// �� ����� - ���������� ������
		strcpy(request, CGI_FAIL_RESP);
		return 1;	
	}

	// �����
	loginPos += sizeof(LOGIN_PARAM) - 1;
	while(*loginPos != 0 && *loginPos != '&' && loginLen < MAX_LOGIN_LEN)
		login[loginLen++] = *loginPos++;

	// ������
	passwPos += sizeof(PASSW_PARAM) - 1;
	while(*passwPos != 0 && *passwPos != '&' && passwLen < MAX_PASSW_LEN)
		passw[passwLen++] = *passwPos++;
	
	// ���������
	if(NetAccessRequest(login, passw))
	{
		Use_Cookies = 1;

		// ��������� ������ � ����� � coockie
		SetCookies(login, passw);

		// �������� ���������������	
		strcpy(request, LOGIN_REDIR_STRING);
	}
	else
		// �� ������� - ���������� ������
		strcpy(request, CGI_FAIL_RESP);
	
	return 1;	
} 

/**************************************************************************************/
/************************************** SENSORS ***************************************/
/**************************************************************************************/


#if(_IMP_FUNC_ == OPTION_ON)
	void reset_sens_counters(uint8_t ch);
#endif
#if(_CLOCK_SENS_FUNC_ == OPTION_ON)
	void reset_counters(uint8_t ch);
#endif



static int
SENSOR_process(char *request)
{
	char *sensor, *sensorEnd, *resp, *clear;
	uint8_t lastSensor = 0;

	// ������, ������ ����� ����������
	char *temp = strchr((char *)request, ' ');
	HTTP_ASSERT(temp == 0, sensor_fail, CGI_FAIL_RESP);

	// ������������ ������ � �����������
	*temp = '\0';
	// ����� ��� ������
	resp = temp + 1;
	temp = request;

	// ���� ������ ��� ������� ���������
	clear = strstr(temp, SENSORS_CL_PARAM);	
	// ���� ���� ������
	if(clear != 0)
	{
		clear += sizeof(SENSORS_CL_PARAM) - 1;

		int num = -1;
		num = atoi(clear);	
		HTTP_ASSERT((num < 0) || (num > 7), sensor_fail, CGI_FAIL_RESP);

	    if(num >= 4 )
		{
			num -= 4;
			#if(_IMP_FUNC_ == OPTION_ON)
				reset_sens_counters(num);
			#endif
		}
		else
		{
			#if(_CLOCK_SENS_FUNC_ == OPTION_ON)
				reset_counters(num);
			#endif
		}

		strcpy(request, CGI_OK_RESP);
		return 1;
	}

	// ���� �������
	sensor = strstr(temp, SENSORS_PARAM);		
	HTTP_ASSERT(sensor == 0, sensor_fail, CGI_FAIL_RESP);

	sensor += sizeof(SENSORS_PARAM) - 1;

	// ��������� XML
	strcpy(resp, SENSOR_XML_RESP_HEAD);
	sensorEnd = sensor;

	while(!lastSensor)
	{
		// ���� ��� �������, ������� ���� ���������
		// ���� ����� ������ (���� ����� - ����� ������) ��� ����������� - ';'
		while(*sensorEnd != 0 && *sensorEnd != ';')	
			sensorEnd++;

		if(*sensorEnd == 0)
			// ����������� �����
			lastSensor = 1;	

		// ������������ ��� �������
		*sensorEnd = 0;

		// �������� �������
		sprintf(&resp[strlen(resp)], XML_ELEM_BEGIN, sensor);
		
		SENSOR_HANDLE hSensor = OpenSensorProc(sensor);
		if(!hSensor)
			// �� ����� - �������
			strcat(resp, "---");	
		else
		{
			float val;
			// ������ ������
			ReadSensor(hSensor, &val, 1);	

			// ������� �������� ������� � XML
			sprintf(&resp[strlen(resp)], "%0.3f", val);
		}

		// ��������� �������
		sprintf(&resp[strlen(resp)], XML_ELEM_END, sensor);
			
		// ��������� � ���������� �������
		sensor = ++sensorEnd;		
	}

	// ����� XML
	strcat(resp, SENSOR_XML_RESP_TAIL);
	strcpy(request, resp);

	// xml type
	return 2;

sensor_fail:

	strcpy(request, errMes);

	// text type
	return 1;
} 

/**************************************************************************************/
/*************************************** OPTION ***************************************/
/**************************************************************************************/

//static int
//OPTION_process(char *request)
//{
////#define OPT_GET 0
////#define OPT_SET 1
//
//	char /*method[4],*/ option[MAX_SMS_NAME_LENGTH + 1], value[MAX_SMS_BUFF_LENGTH / 2 + 1];
////	memset(method, 0, sizeof(method));
//	memset(option, 0, sizeof(option));
//	memset(value, 0, sizeof(value));
//
//	// ������, ������ ����� ����������
//	char *temp = strchr((char *)request, ' ');
//	if(temp == 0)
//	{
//		strcpy(request, LOGIN_REDIR_STRING);
//		return 1;	
//	}
//
//	// ������������ ������ � �����������
//	*temp = '\0';
//	temp = request;
//
//	// �������� (��������� ��� ������ ���������), ��� ��������� � ��� ��������
//	char /**methodPos = strstr(req, OPT_METHOD_PARAM), */*optionPos = strstr(temp, OPTION_NAME_PARAM), *valuePos = strstr(temp, OPTION_VAL_PARAM);
//	int /*methodLen = 0, */optionLen = 0, valueLen = 0;
//	
//	if(/*methodPos == 0 || */optionPos == 0 || valuePos == 0)			
//	{
//		// �� ����� - ���������� ������
//		strcpy(request, OPTION_ERR_RESP);
//		return 1;	
//	}
//
//	// ��������
////	methodPos += sizeof(OPT_METHOD_PARAM) - 1;
////	while(*methodPos != 0 && *methodPos != '&' && methodLen < 4)
////		method[methodLen++] = *methodPos++;
//
////	int meth;
////	if(!strcmp(method, "set") && valuePos != 0)
////		meth = OPT_SET;
////	else if(!strcmp(method, "get"))
////		meth = OPT_GET;
////	else
////	{
////		// ����������� �����
////		strcpy(buf, OPTION_ERR_RESP);
////		return 1;	
////	}
//
//	// ��� ���������
//	optionPos += sizeof(OPTION_NAME_PARAM) - 1;
//	while(*optionPos != 0 && *optionPos != '&' && optionLen < MAX_SMS_NAME_LENGTH)
//		option[optionLen++] = *optionPos++;
//
//	// ���� ����� ���������� ��������, ���� ��� ��������
////	if(meth == OPT_SET)
//	{
//		// �������� ���������
//		valuePos += sizeof(OPTION_VAL_PARAM) - 1;
//		while(*valuePos != 0 && *valuePos != '&' && valueLen < (MAX_SMS_BUFF_LENGTH / 2 + 1))
//			value[valueLen++] = *valuePos++;
//	}
////	else
////	{
////		value[0] = '?';
////		value[1] = 0;	
////	}
//		
//	// �������� ID �������	
//	uint8_t ID = get_sms_ID((uint8_t *)option);			
//
//	// ��� ����� �������
//	if(ID == 0xff)
//	{
//		strcpy(request, OPTION_ERR_RESP);
//		return 1;	
//	}
//
//	MODEM_MESSAGE message;
//
//	message.ID = SMS_PERFORM;
//	message.par_1 = ID;
//	message.par_2 = sizeof(value);
//	message.privat_data = (uint8_t *)value;
//	message.queue = http_queue; 
//
//	xQueueSendToBack(modem_queue, ( void * )&message, 10);
//
//	int res = xQueueReceive(http_queue, (void *)&message, 30000);
//
//	if(res == 0)
//	{
//		strcpy(request, OPTION_ERR_RESP);
//		return 1;	
//	}
////	else if(meth == OPT_SET)
////		strcpy(buf, OPTION_OK_RESP);	
////	else
//		sprintf(request, OPTION_RESP_TEMPLATE, value);
//	
//	return 1;
//}

/**************************************************************************************/
/*************************************** CONFIG ***************************************/
/**************************************************************************************/

static int
CONFIG_process(char *request)
{
	char *fileName, *topName, *elemName, *valName, *delete;
	uint8_t lastParam = 0;

	// ������, ������ ����� ����������
	char *temp = strchr((char *)request, ' ');
	HTTP_ASSERT(temp == 0, config_fail, CGI_FAIL_RESP);

	// ������������ ������ � �����������
	*temp = '\0';
	temp = request;

	// ����, �������, delete, �������:��������  
	fileName = strstr(request, CONFIG_FILE_PARAM);  
	topName = strstr(request, CONFIG_TOP_PARAM); 
	delete = strstr(request, CINFIG_DEL_PARAM); 
	elemName = strstr(request, CONFIG_ELEMS_PARAMS); 
	// �� ��������� ������� �������� ���, �.�. ��� ������� delete �������� �� �����		  
	HTTP_ASSERT(fileName == 0 || topName == 0, config_fail, CGI_FAIL_RESP);

	// ���������� ������ ���������
	fileName += sizeof(CONFIG_FILE_PARAM) - 1;
	topName += sizeof(CONFIG_TOP_PARAM) - 1;

	// ���� �������� ���������
	temp = fileName;
	while(*temp != 0 && *temp != '&')
		temp++;
	*temp = 0;	

	temp = topName;
	while(*temp != 0 && *temp != '&')
		temp++;
	*temp = 0;

	// ���� ���� �������� delete, ������ ���� ������� ������� � ���, ��� � ��� �������
	if(delete != 0)
	{
		int res = DeleteTop(fileName, topName);	
		HTTP_ASSERT(res != 0, config_fail, CGI_FAIL_RESP);

		strcpy(request, CGI_OK_RESP);
		return 1;
	}

	// delete ����, ���� ��������� ������� ��������		
	HTTP_ASSERT(elemName == 0, config_fail, CGI_FAIL_RESP);

	// ���������� ������ ���������
	elemName += sizeof(CONFIG_ELEMS_PARAMS) - 1;

    temp = elemName;
	while(!lastParam)
	{
		// ���� �������� ��������, ������� ������ ���� �����������
		// ���� ����� ������ (�� ������ ����� ���� ��� ��) ��� ����������� - ':'
		while(*temp != 0 && *temp != ':')	
			temp++;	

		// ������������ ��� ��������
		*temp = '\0';
		valName = ++temp;

		// ���� ����� �������� ��������
		// ���� ����� ������ (���� ����� - ��� ��������� �������� � ����� ��������� �����) ��� ����������� ������ - '@'
		// ������ ������������ ";", �� ���� ������ ����� ����������� � ���������� � ������
		while(*temp != 0 && *temp != '@')	
			temp++;

		// ����� ����� ���� ����������
		if(*temp == 0)
			lastParam = 1;

		// ������������ ��� ���������
		*temp = '\0';

		// javascript �� ����� ��������� �������� �������
		// ������ ' �������� %27, ������ " - %22
		// ������� ������ ������� �������� ^
	/*	if(!strcmp(elemName, "InitString_1") || !strcmp(elemName, "InitString_2"))
		{
			char *tmp = valName;
			escape(tmp);
			while(*tmp)	
			{
				if(*tmp == '^')
					*tmp = '\"';
				tmp++;
			}		
		}

		// # �� ���������� ����
		if(!strcmp(elemName, "DialNum_1") || !strcmp(elemName, "DialNum_2"))
		{
			char *tmp = valName;
			while(*tmp)	
			{
				if(*tmp == '!')
					*tmp = '#';
				tmp++;
			}		
		}	   */

		// ����� �������� ��������
		escape(valName);
		int res = SetParameter(fileName, topName, elemName, valName);
		HTTP_ASSERT(res != 0, config_fail, CGI_FAIL_RESP);

		// ������ ��������� ����
		elemName = ++temp;		
	}

	strcpy(request, CGI_OK_RESP);

	// text type
	return 1;

config_fail:

	strcpy(request, errMes);

	// text type
	return 1;
}

/**************************************************************************************/
/*************************************** INFO *****************************************/
/**************************************************************************************/

void vGetFreeMem(uint16_t * xMaxBlock, uint16_t * xTotalMem);
static void getInfoParam(char *paramName, char *buf)
{
	if (!strcmp(paramName, "SWVers"))
#if defined (DEBUG)
		strcpy(buf, (char *)"999");
#else
		strcpy(buf, (char *)VERS_ADDR);
#endif

	else if (!strcmp(paramName, "HWVers"))
		strcpy(buf, (char *)"1.21");
	else if (!strcmp(paramName, "BLVers"))
#if defined (DEBUG)
	strcpy(buf, (char *)"999");	
#else
	strcpy(buf, (char *)BOOT_VERS_ADDR);
#endif

#if(OPERATOR_NAME == OPTION_ON)
	else if (!strcmp(paramName, "OpName"))
	{
		char *opName = (char *)GetOpName();
		if(opName)
			strcpy(buf, opName);
		else
			strcpy(buf, "---");	
	}
#endif
#if(OWN_DIAL_NUMBER == OPTION_ON)
	else if(!strcmp(paramName, "SIMNum"))
	{
		char *num = (char *)GetSimNum();
		if(num)
			strcpy(buf, num);
		else
			strcpy(buf, "---");
	}
#endif
#if(SIGNAL_LEVEL == OPTION_ON)
	else if(!strcmp(paramName, "SigLevel"))
	{
		uint8_t num = GetSigLevel();
		sprintf(buf, "%02d", num);
	}
#endif
	else if(!strcmp(paramName, "SerNum"))
	{
		ReadSettingsFile(PARAMETRS_FILE_NAME);	
		if(GetSettingsFileStatus())
		{
			// ��� �� - ������ �������� ���������
			char *inf = (char *)GetPointToElement("INFO", "Serial");
			if(inf != 0)
				strcpy(buf, inf);
			else
				strcpy(buf, "---");
		}
		else
			// �� ��������
			strcpy(buf, "---");
		ParametrsMemFree();	
	}
	else if (!strcmp(paramName, "DevID"))
	{
		GetProcessorID(buf);
	}
	else if(!strcmp(paramName, "SimIP"))
	{
		char *ip = GetOurIPStr();

		if(ip)
			strcpy(buf, ip);
		else
			strcpy(buf, "00.00.00.00");
	}
#if(IMEI == OPTION_ON)
	else if(!strcmp(paramName, "IMEI"))
	{
		buf[0] = '?';
		buf[1] = 0x00;
		GetIMEI(buf);
	}
#endif  //IMEI == OPTION_ON
	else if(!strcmp(paramName, "Date"))
	{
		CLOCKData dateTime;
		DEVICE_HANDLE hClock = OpenDevice("Clock");
		if(!hClock)
		{
			strcpy(buf, "---");
			return;
		}
		SeekDevice(hClock, DATE_D, 0);
		ReadDevice(hClock, &dateTime.Date, 4);
		CloseDevice(hClock);

		sprintf(buf, "%02d.%02d.%04d", dateTime.Date, dateTime.Month, dateTime.Year + 2000);
		return;
	}
	else if(!strcmp(paramName, "Time"))
	{
		CLOCKData dateTime;
		DEVICE_HANDLE hClock = OpenDevice("Clock");
		if(!hClock)
		{
			strcpy(buf, "---");
			return;
		}
		SeekDevice(hClock, SEC_D, 0);
		ReadDevice(hClock, &dateTime.Sec, 3);
		CloseDevice(hClock);

		sprintf(buf, "%02d:%02d:%02d", dateTime.Hour, dateTime.Min, dateTime.Sec);
		return;
	}
//#if _AUTOGRAPH_ == OPTION_ON

	else if (!strcmp(paramName, "Disp"))
	{
		ReadSettingsFile(PARAMETRS_FILE_NAME);	
		if(GetSettingsFileStatus())
		{
			// ��� �� - ������ �������� ���������
			char *inf = (char *)GetPointToElement("INFO", "DISP");
			if(inf != 0)
				strcpy(buf, inf);
			else
				strcpy(buf, "0");
		}
		else
			// �� ��������
			strcpy(buf, "0");
		ParametrsMemFree();
	}
	else if (!strcmp(paramName, "AutIP"))
	{
		ReadSettingsFile(AUTOGRAPH_FILE_NAME);	
		if(GetSettingsFileStatus())
		{
			// ��� �� - ������ �������� ���������
			char *inf = (char *)GetPointToElement("INFO", "IP");
			if(inf != 0)
				strcpy(buf, inf);
			else
				strcpy(buf, "---");
		}
		else
			// �� ��������
			strcpy(buf, "---");
		ParametrsMemFree();
	}
	else if (!strcmp(paramName, "AutPort"))
	{
		ReadSettingsFile(AUTOGRAPH_FILE_NAME);	
		if(GetSettingsFileStatus())
		{
			// ��� �� - ������ �������� ���������
			char *inf = (char *)GetPointToElement("INFO", "Port");
			if(inf != 0)
				strcpy(buf, inf);
			else
				strcpy(buf, "---");
		}
		else
			// �� ��������
			strcpy(buf, "---");
		ParametrsMemFree();
	}
	else if (!strcmp(paramName, "AutID"))
	{
		ReadSettingsFile(AUTOGRAPH_FILE_NAME);	
		if(GetSettingsFileStatus())
		{
			// ��� �� - ������ �������� ���������
			char *inf = (char *)GetPointToElement("INFO", "Serial");
			if(inf != 0)
				strcpy(buf, inf);
			else
				strcpy(buf, "---");
		}
		else
			// �� ��������
			strcpy(buf, "---");
		ParametrsMemFree();
	}

//#endif

	else if(!strcmp(paramName, "MemTotal"))
	{
		sprintf(buf, "%d", configTOTAL_HEAP_SIZE);
	}
	else if(!strcmp(paramName, "MemInUse"))
	{
		uint16_t free, inUse;

		vGetFreeMem(&inUse, &free);
		inUse = configTOTAL_HEAP_SIZE - free;

		sprintf(buf, "%d", inUse);
	}
	else if(!strcmp(paramName, "MemFree"))
	{
		uint16_t free, inUse;

		vGetFreeMem(&inUse, &free);
		inUse = configTOTAL_HEAP_SIZE - free;

		sprintf(buf, "%d", free);
	}

#if (_GPS_ == DEVICE_ON)
	else if(!strcmp(paramName, "GPSLat"))
	{
		DEVICE_HANDLE hGPS = OpenDevice("GPS");
		if(!hGPS)
		{
			strcpy(buf, "---");
			return;
		}
		
		SeekDevice(hGPS, LATITUDE, 0);
		float lat;
		ReadDevice(hGPS, (uint8_t *)&lat, LATITUDE_SIZE);
		CloseDevice(hGPS);
		
		sprintf(buf, "%0.2f", lat);
		return;
	}
	else if(!strcmp(paramName, "GPSLon"))
	{
		DEVICE_HANDLE hGPS = OpenDevice("GPS");
		if(!hGPS)
		{
			strcpy(buf, "---");
			return;
		}
		
		SeekDevice(hGPS, LONGITUDE, 0);
		float lon;
		ReadDevice(hGPS, (uint8_t *)&lon, LONGITUDE_SIZE);	
		CloseDevice(hGPS);

		sprintf(buf, "%0.2f", lon);
		return;
	}
	else if(!strcmp(paramName, "GPSAlt"))
	{
		DEVICE_HANDLE hGPS = OpenDevice("GPS");
		if(!hGPS)
		{
			strcpy(buf, "---");
			return;
		}
		
		SeekDevice(hGPS, ALTITUDE, 0);
		float alt;
		ReadDevice(hGPS, (uint8_t *)&alt, ALTITUDE_SIZE);	
		CloseDevice(hGPS);

		sprintf(buf, "%0.2f", alt);
		return;	
	}
	else if(!strcmp(paramName, "GPSSat"))
	{
		DEVICE_HANDLE hGPS = OpenDevice("GPS");
		if(!hGPS)
		{
			strcpy(buf, "---");
			return;
		}	

		SeekDevice(hGPS, SATELLITES, 0);
		uint8_t satNum;
		ReadDevice(hGPS, (uint8_t *)&satNum, SATELLITES_SIZE);	
		CloseDevice(hGPS);

		sprintf(buf, "%d", satNum);
		return;	
	}
#endif

#if (_OWI_ == DEVICE_ON)
	else if(!strcmp(paramName, "OWI"))
	{
		DEVICE_HANDLE hOWI = OpenDevice("OWI");
		if(!hOWI)
		{
			strcpy(buf, "---");
			return;
		}	

		SeekDeviceText(hOWI, 0, 0);
		int res = ReadDeviceText(hOWI, buf, MAX_OWI_DATA_OFFSET);	
		CloseDevice(hOWI);
		buf[res] = 0;

		return;		
	}
#endif

	else
		strcpy(buf, "---");
}

static int
INFO_process(char *request)
{
	char *siteElem, *infoParam, *resp;
	uint8_t lastParam = 0;	

	// ������, ������ ����� ���� ����������
	char *temp = strchr((char *)request, ' ');
	HTTP_ASSERT(temp == 0, service_fail, CGI_FAIL_RESP);

	// ������������ ������ � �����������
	*temp = '\0';
	// ������ ������ � �������
	resp = temp + 1;
	temp = request;

	// ������ ����������
	siteElem = strstr(request, INFO_PARAM); 
	HTTP_ASSERT(temp == 0, service_fail, CGI_FAIL_RESP);
	siteElem += sizeof(INFO_PARAM) - 1;

	// ��������� XML
	strcpy(resp, INFO_XML_RESP_HEAD);
	temp = siteElem;

	while(!lastParam)
	{
		// ���� �������� �������� �����, � ������� ������ ���� ������� ��������
		// ���� ����� ������ (�� ������ ����� ���� ��� ��) ��� ����������� - ':'
		while(*temp != 0 && *temp != ':')	
			temp++;	

		// ������������ ��� �������� �����
		*temp = '\0';
		infoParam = ++temp;

		// ���� �������� ���������, ������� ��� ��������
		// ���� ����� ������ (���� ����� - ��� ��������� �������� � ����� ��������� �����) ��� ����������� ������ - ';'
		while(*temp != 0 && *temp != ';')	
			temp++;

		// ����� ����� ���� ����������
		if(*temp == 0)
			lastParam = 1;

		// ������������ ��� ���������
		*temp = '\0';

		// ��������� �������
		sprintf(&resp[strlen(resp)], XML_ELEM_BEGIN, siteElem);
		getInfoParam(infoParam, &resp[strlen(resp)]);
		sprintf(&resp[strlen(resp)], XML_ELEM_END, siteElem);

		// ������ ��������� ����
		siteElem = ++temp;
	}

	strcat(resp, INFO_XML_RESP_TAIL);
	strcpy(request, resp);
	
	// xml type
	return 2;  

service_fail:

	strcpy(request, errMes);

	// text type
	return 1;
}



// ��������������� �������������� �������
void escape(char *text)
{
	int len = strlen(text), newLen, i;
	newLen = len;

	for(i = 0; i < len; i++)
	{
		// ������ ������������������
		if(text[i] == '%' && (i + 2 <= len - 1))
		{
			switch(text[i + 1])
			{
				case '0': 
						switch(text[i + 2])
						{
							case 'D':
							case 'd':
									text[i] = '\r';
									break;			
							case 'A':
							case 'a':
									text[i] = '\n';
									break;
							default: continue;
						}
						break;
								
				case '2':
						switch(text[i + 2])
						{
							case '0':
									text[i] = ' ';
									break;		
							case '2':
									text[i] = '\"';
									break;
							case '3':
									text[i] = '#';
									break;
							case '4':
									text[i] = '$';
									break;
							case '5':
									text[i] = '%';
									break;
							case '6':
									text[i] = '&';
									break;
							case 'B':
							case 'b':
									text[i] = '+';
									break;
							case 'C':
							case 'c':
									text[i] = ',';
									break;
							case 'F':
							case 'f':
									text[i] = '/';
									break;
							default: continue;
						}
						break;  
						
				case '3':
						switch(text[i + 2])
						{
							case 'A':
							case 'a':
									text[i] = ':';
									break;		
							case 'B':
							case 'b':
									text[i] = ';';
									break;
							case 'C':
							case 'c':
									text[i] = '<';
									break;
							case 'D':
							case 'd':
									text[i] = '=';
									break;
							case 'E':
							case 'e':
									text[i] = '>';
									break;
							case 'F':
							case 'f':
									text[i] = '?';
									break;
							default: continue;
						}
						break; 
				case '4':
						switch(text[i + 2])
						{
							case '0':
									text[i] = '@';
									break;		
							default: continue;
						}
						break;
				case '5':
						switch(text[i + 2])
						{
							case 'B':
							case 'b':
									text[i] = '[';
									break;
							case 'C':
							case 'c':
									text[i] = '\\';
									break;		
							case 'D':
							case 'd':
									text[i] = ']';
									break;
							case 'E':
							case 'e':
									text[i] = '^';
									break;
							default: continue;
						}
						break;
				case '6':
						switch(text[i + 2])
						{
							case '0':
									text[i] = '\'';
									break;
							default: continue;
						}
						break;
				case '7':
						switch(text[i + 2])
						{
							case 'B':
							case 'b':
									text[i] = '{';
									break;
							case 'C':
							case 'c':
									text[i] = '|';
									break;		
							case 'D':
							case 'd':
									text[i] = '}';
									break;
							default: continue;
						}
						break;
				default: continue;
			}

			/*if(text[i + 1] == '2' && text[i + 2] == '0')
				text[i] = ' ';	
			else if(text[i + 1] == '2' && text[i + 2] == '2')
				text[i] = '\"';
			else if(text[i + 1] == '5' && text[i + 2] == 'C')
				text[i] = '\\';
			if(text[i + 1] == '3' && text[i + 2] == 'E')
				text[i] = '>';
			if(text[i + 1] == '3' && text[i + 2] == 'C')
				text[i] = '<';*/

			// �������� ������� ������
			//if(text[i] != '%')
			{
				memcpy(&text[i + 1], &text[i + 3], len - (i + 3));
				len -= 2;
			}
		}
	}

	// ������������ ����� ������
	if(newLen != len)
		text[len] = 0;
}

static int
FILE_process(char *request)
{
	char *fileName, *seek, *truncate, *text;
	int res, seekLen;

	// ������, ������ ����� ���� ����������
	char *temp = strchr((char *)request, ' ');
	HTTP_ASSERT(temp == 0, file_fail, CGI_FAIL_RESP);

	// ������������ ������ � �����������
	*temp = '\0';
	temp = request;

	// ��� �����
	fileName = strstr(temp, FILE_NAME_PARAM);
	HTTP_ASSERT(fileName == 0, file_fail, CGI_FAIL_RESP);
	fileName += sizeof(FILE_NAME_PARAM) - 1;
	
	// �������� � �����										 	
	seek = strstr(temp, FILE_SEEK_PARAM);
	HTTP_ASSERT(seek == 0, file_fail, CGI_FAIL_RESP);
	seek += sizeof(FILE_SEEK_PARAM) - 1;

	// ������� ����� �� �������� �������										 	
	truncate = strstr(temp, FILE_TRUNC_PARAM);
	HTTP_ASSERT(truncate == 0, file_fail, CGI_FAIL_RESP);
	truncate += sizeof(FILE_TRUNC_PARAM) - 1;

	// ����� ��� ������ � ����
	text = strstr(temp, FILE_TEXT_PARAM);
	HTTP_ASSERT(text == 0, file_fail, CGI_FAIL_RESP);
	text += sizeof(FILE_TEXT_PARAM) - 1;

	// ���� ����� ���������	<��� �����>
	temp = fileName;
	while(*temp != 0 && *temp != '&')
		temp++;	
	*temp = 0;

	// ���� ����� ���������	<�������� � �����>
	temp = seek;
	while(*temp != 0 && *temp != '&')
		temp++;	
	*temp = 0;
	seekLen = atoi(seek);

	// ���� ����� ���������	<�������� ����� �� �������� �������>
	temp = truncate;
	while(*temp != 0 && *temp != '&')
		temp++;	
	*temp = 0;

	// ���� ����� ���������	<�����>
	temp = text;
	while(*temp != 0)
		temp++;	
	*temp = 0;

	escape(text);

	// � url ������ �������� \r\n, ������� ������ ��� ���������� ������� \ � r
	// �� ���� ��������
	while(1)
	{
		char *sep = strstr(text, "\\r");
		if(!sep)
			break;
		// ������ �� ������� ����������
		memcpy(sep, "\r\n", 2);
	}

	FILE_OBJ File;
	// ��������� ���� ��� ������
	res = VFS_open(&File, fileName, FA_OPEN_EXISTING | FA_READ | FA_WRITE);
	HTTP_ASSERT(res != 0, file_fail, CGI_FAIL_RESP);	

	// �������� � �����
	res = VFS_fseek(&File, seekLen);
//	HTTP_ASSERT(res != 0, file_open_fail, CGI_FAIL_RESP);
	
	// ������ ������
	res = VFS_write(&File, (uint8_t *)text, strlen(text));	
	HTTP_ASSERT(res != 0, file_open_fail, CGI_FAIL_RESP);

	// ���������, ����� �� �������� ���� �� �������� �������
	if(!strcmp(truncate, "true"))
		// �����
		VFS_truncate(&File);

	VFS_close(&File);

	strcpy(request, CGI_OK_RESP);

	// text type
	return 1;  

file_open_fail:
	//VFS_close(&File);
file_fail:
	strcpy(request, errMes);

	//text type
	return 1;
}

static int
DATETIME_process(char *request)
{
	char *date, *time;
	CLOCKData dateTime;
	DEVICE_HANDLE hClock;

	// ������, ������ ����� ���� ����������
	char *temp = strchr((char *)request, ' ');
	HTTP_ASSERT(temp == 0, datetime_fail, CGI_FAIL_RESP);

	// ������������ ������ � �����������
	*temp = '\0';
	temp = request;

	// ����
	date = strstr(temp, DATETIME_DATE_PARAM);
	HTTP_ASSERT(date == 0, datetime_fail, CGI_FAIL_RESP);
	date += sizeof(DATETIME_DATE_PARAM) - 1;
	
	// �����									 	
	time = strstr(temp, DATETIME_TIME_PARAM);
	HTTP_ASSERT(time == 0, datetime_fail, CGI_FAIL_RESP);
	time += sizeof(DATETIME_TIME_PARAM) - 1;

	// ���� ����� ���������	<����>
	temp = date;
	while(*temp != 0 && *temp != '&')
		temp++;	
	*temp = 0;

	// ���� ����� ���������	<�����>
	temp = time;
	while(*temp != 0)
		temp++;	
	*temp = 0;

	int ss, mm, hh, DD, MM, YYYY;

	sscanf(date, "%02d.%02d.%04d", &DD, &MM, &YYYY);
	sscanf(time, "%02d:%02d:%02d", &hh, &mm, &ss);

	dateTime.Sec = (uint8_t)ss;
	dateTime.Min = (uint8_t)mm;
	dateTime.Hour = (uint8_t)hh;
	dateTime.Date = (uint8_t)DD;
	dateTime.Month = (uint8_t)MM;
	dateTime.Year = (uint8_t)(YYYY - 2000);

	hClock = OpenDevice("Clock");
	HTTP_ASSERT(hClock == 0, datetime_fail, CGI_FAIL_RESP);

	SeekDevice(hClock, SEC_D, 0);
	WriteDevice(hClock, &dateTime.Sec, 1);

	SeekDevice(hClock, MIN_D, 0);
	WriteDevice(hClock, &dateTime.Min, 1);

	SeekDevice(hClock, HOUR_D, 0);
	WriteDevice(hClock, &dateTime.Hour, 1);

	SeekDevice(hClock, DATE_D, 0);
	WriteDevice(hClock, &dateTime.Date, 1);

	SeekDevice(hClock, MONTH_D, 0);
	WriteDevice(hClock, &dateTime.Month, 1);

	SeekDevice(hClock, YEAR_D, 0);
	WriteDevice(hClock, &dateTime.Year, 1);

	CloseDevice(hClock);

	strcpy(request, CGI_OK_RESP);

	// text type
	return 1;  

datetime_fail:

	strcpy(request, errMes);

	// text type
	return 1;	
}


void program_reset(void);
static int
DISP_process(char *request)
{
		char data[2] = {'0', 0};
		if(strcmp(request, "state=0") == 0){
			data[0] = '0';
		}
		else if(strcmp(request, "state=1") == 0){
			data[0] = '1';
		}
		SetParameter(PARAMETRS_FILE_NAME, "INFO", "DISP", (char *)data);
		program_reset();
		return 0;
}


static int
REINIT_process(char *request)
{
	PutMesToComQueue(DEV_REINIT, NULL, NULL, 1000);
	strcpy(request, CGI_OK_RESP);

	// text type
	return 1;
}


static int
CAMERA_process(char *request)
{
#if(_ZM_CAMERA_)
	char *reqType;

	// ������, ������ ����� ���� ����������
	char *temp = strchr((char *)request, ' ');
	HTTP_ASSERT(temp == 0, camera_fail, CGI_FAIL_RESP);

	// ������������ ������ � �����������
	*temp = '\0';
	temp = request;

	// ������� ���� ������ �� ������ ������ ����������
	reqType = strstr(temp, CAMERA_LIST_PARAM);
	if(reqType)
	{
		// ���� ������
		FILE_OBJ fil;
    	DIR_OBJ dir;

		HTTP_ASSERT(VFS_opendir(&dir, "/at45db/PH"), camera_fail, CGI_FAIL_RESP);

		strcpy(request, INFO_XML_RESP_HEAD);
	    while ((VFS_readdir(&fil, &dir) == FR_OK) && fil.name[0] != NULL)
	    {
	    	if(!(fil.attrib & AM_DIR)) 
	        {
            	strcat(request, "<Snapshot>");
				strcat(request, fil.name);
				strcat(request, "</Snapshot>");		    
	        }
	    } 
	    VFS_closedir(&dir);

		strcat(request, INFO_XML_RESP_TAIL);

		// xml type
		return 2;
	}

#if defined(SMS_SNAP)
	// ���� ������ �� ����������������
	reqType = strstr(temp, CAMERA_SNAP_PARAM);
	if(reqType)
	{
		// ���� ������ �� �����

		COMMON_MESSAGE comMess;
		MODEM_MESSAGE modMess;
		// ���������� ��� ��������� �� �������
		while(xQueueReceive(HTTPQueue, &comMess, 0));

		// ��������� ��� SMS
		modMess.ID = SMS_PERFORM;
		modMess.par_1 = SMS_SNAP;
		modMess.par_2 = sizeof(ERROR_SMS) + 1;
		modMess.privat_data = (uint8_t *)request;
		modMess.queue =  HTTPQueue;

		// ���� ���������
		xQueueSendToFront(modem_queue, &modMess, 0);

		// ���� ������
		HTTP_ASSERT(!xQueueReceive(HTTPQueue, &comMess, 35000), camera_fail, CGI_FAIL_RESP);
		HTTP_ASSERT(strcmp(request, "OK"), camera_fail, CGI_FAIL_RESP);
		
		strcpy(request, CGI_OK_RESP);

		// text type
		return 1;	
	}
#endif

	reqType = strstr(temp, CAMERA_DELETE_PARAM);
	if(reqType)
	{
		// ���� ������ �� ��������
		char *name, *temp;
		char path[sizeof("/AT45DB/PH/12345678.jpg") + 1];

		uint8_t lastFile = 0;
		name = temp = reqType + sizeof(CAMERA_DELETE_PARAM) - 1;
		do
		{
			// ���� ����� ������, ���������� ';' ���� ������ '\0'
			while(*temp != 0 && *temp != ';')	
				temp++;
			// ���� 0 - ����� ��� ���������� �����
			if(!(*temp))
				lastFile = 1;
			*temp = 0;

			if((strlen(name) <= sizeof("12345678.jpg")))	
			{
				strcpy(path, "/at45db/PH/");
				strcat(path, name);	
				
				VFS_rmdir(path);		
			}
			name = ++temp;

		}while(!lastFile);

		strcpy(request, CGI_OK_RESP);

		// text type
		return 1;
	}

	strcpy(request, CGI_FAIL_RESP);

	// text type
	return 1;  

camera_fail:

#endif  //_ZM_CAMERA_
	
	strcpy(request, errMes);

	// text type
	return 1;
}

static int
TERMINAL_process(char *request)
{
	char *type, *text;
	int maxAnsLen = strlen(request);
	COMMON_MESSAGE comMess;
	MODEM_MESSAGE modMess;

	// ������, ������ ����� ���� ����������
	char *temp = strchr((char *)request, ' ');
	HTTP_ASSERT(temp == 0, terminal_fail, CGI_FAIL_RESP);

	// ������������ ������ � �����������
	*temp = '\0';
	temp = request;

	// ������� ���� ��� �������
	type = strstr(temp, TERMINAL_TYPE_PARAM);
	// ����� ����� �������
	text = strstr(temp, TERMINAL_TEXT_PARAM);

	HTTP_ASSERT(type == 0 || text == 0, terminal_fail, CGI_FAIL_RESP);

	type += sizeof(TERMINAL_TYPE_PARAM) - 1;
	text += sizeof(TERMINAL_TEXT_PARAM) - 1;

	// ���� ����� ���������	<��� �������>
	temp = type;
	while(*temp != 0 && *temp != '&')
		temp++;
	
	HTTP_ASSERT(temp >= (request + maxAnsLen), terminal_fail, CGI_FAIL_RESP);
	
	*temp = 0;

	HTTP_ASSERT(strcmp(type, TERMINAL_TYPE_TERM) != 0, terminal_fail, CGI_FAIL_RESP);

	// ���� ������-����������� ������� � ����������
	escape(text);
	temp = text;
	while(*temp != ' ')
		temp++;
	
	HTTP_ASSERT(temp >= (request + maxAnsLen), terminal_fail, CGI_FAIL_RESP);
	
	*temp = 0;
	
	// �������� ID �������
	modMess.par_1 = get_sms_ID(text);
		
	HTTP_ASSERT(modMess.par_1 == 0xFF, terminal_fail, "Unknow command");
	
		
	// ����� ����� ����� ��� ������������� � ���������� ���������, ������� ������� ��� SMS
	//strcpy(request, "= ");
	// �������� � ������ ������� ����� ��� TERMINAL, ��� ����� ������ ����� ��� ������
	//strcat(request, temp + 1);
	strcpy(request, temp + 1);

		// ���������� ��� ��������� �� �������
	while(xQueueReceive(HTTPQueue, &comMess, 0));

	// ��������� ��� SMS
	modMess.ID = SMS_PERFORM;

  if(maxAnsLen > 255)
		maxAnsLen = 255;			// � ��������� ��� ����� ������ - ����
	modMess.par_2 = maxAnsLen;
	modMess.privat_data = (uint8_t *)request;
	modMess.queue = HTTPQueue;

	// ���� ���������
	HTTP_ASSERT(!xQueueSendToFront(modem_queue, &modMess, 0), terminal_fail, "Error send message to modem queue");

	// ���� ������
	xQueueReceive(HTTPQueue, &comMess, portMAX_DELAY);
	//HTTP_ASSERT(!strcmp(request, "ERROR"), terminal_fail, CGI_FAIL_RESP);

	if(strlen(request) == 0)
		strcpy(request, TERMINAL_TOUT_ANS);

	// text type
	return 1;	

terminal_fail:

	strcpy(request, errMes);

	// text type
	return 1;
}

static int
CANSNIFFER_process(char *request)
{
	DEVICE_HANDLE hCan = NULL;
	
	if((strncmp(request, "active=0", 8) !=0) && (strncmp(request, "active=1", 8) !=0))
		goto fail_ex;

	hCan = OpenDevice("CAN");
	if(hCan == NULL)
		goto fail_ex;

	if(strncmp(request, "active=0", 8) ==0)
		DeviceIOCtl(hCan, SNIF_STOP, 0);
	else if(strncmp(request, "active=1", 8) ==0)
		DeviceIOCtl(hCan, SNIF_START, 0);
	else{
		CloseDevice(hCan);
		goto fail_ex;
	}			

	CloseDevice(hCan);
	strcpy(request, CGI_OK_RESP);
	return 1;
fail_ex:
	strcpy(request, CGI_FAIL_RESP);
	return 1;
}


#if defined (_PIPELINES_)


float GetUstVal(int ch, int is);

static int
PIPELINES_process(char *request)
{
	uint8_t mbIDsBuf[4]; // ����� ��� modbus id (� ������ ������ �� ����� 4 ���������)																				   
	uint8_t mbIDsCnt;    // ���������� id

	// ��������� XML-������
	strcpy(request, PIPELINES_XML_RESP_HEAD);

	// ���� ��������� ���������� id � modbus, ����� �����, ������� ����� � ������� ������������� � ����������
  	DEVICE_HANDLE hMBS = OpenDevice("MBS");	
	DeviceIOCtl(hMBS, GET_SUPPORTED_IDs_NUMBER, &mbIDsCnt);
  	DeviceIOCtl(hMBS, GET_SUPPORTED_IDs, mbIDsBuf);

	 // ������� ������� ���������� �� ������� Modbus
	 int rx_count, tx_count, cnt;
	 for(cnt = 0; cnt < mbIDsCnt; cnt++)
	 {
		// modbus id ���������� � �������� ���������
		// � ����-�� � ����������� �����
		rx_count = tx_count = (int)mbIDsBuf[cnt]; 
		
		DeviceIOCtl(hMBS, GET_TX_PACKETS_COUNT, (uint8_t *)&tx_count);	  
		DeviceIOCtl(hMBS, GET_RX_PACKETS_COUNT, (uint8_t *)&rx_count);	  

	    // ��������� ������� Modbus id
		sprintf(&request[strlen(request)], PIPELINES_XML_ELEM_BEGIN, "ID", cnt + 1);
		sprintf(&request[strlen(request)], "%d", mbIDsBuf[cnt]);
		sprintf(&request[strlen(request)], PIPELINES_XML_ELEM_END, "ID", cnt + 1);

		// ��������� ������� ��� ���������� tx
		sprintf(&request[strlen(request)], PIPELINES_XML_ELEM_BEGIN, "TX", cnt + 1);
		sprintf(&request[strlen(request)], "%d", tx_count);
		sprintf(&request[strlen(request)], PIPELINES_XML_ELEM_END, "TX", cnt + 1);
		
		// ��������� ������� ��� ���������� rx
		sprintf(&request[strlen(request)], PIPELINES_XML_ELEM_BEGIN, "RX", cnt + 1);
		sprintf(&request[strlen(request)], "%d", rx_count);
		sprintf(&request[strlen(request)], PIPELINES_XML_ELEM_END, "RX", cnt + 1);	
	 }

	 CloseDevice(hMBS);

	 // ������ ������� �������� �������������
	 // �� ������ ����� �� ��� �������
	 // ������ ���� - S0, S1
	 // ������ ���� - S2, S3...
	 SENSOR_HANDLE sensHandle1, sensHandle2;
	 for(cnt = 0; cnt < mbIDsCnt; cnt++)
	 {
	 	float R1 = -1.0f, R2 = -1.0f;
	 	char sensName[3];
		sensName[0] = 'S';
		sensName[1] = '0' + cnt * 2;
		sensName[2] = 0;
	  	
		// ������ ������ - ��������
		sensHandle1 = OpenSensorProc(sensName);
		if(sensHandle1)
			ReadSensor(sensHandle1, &R1, 1);

		sensName[1]++;
		// ������ ������ - ������
		sensHandle2 = OpenSensorProc(sensName);
		if(sensHandle2)
			ReadSensor(sensHandle2, &R2, 1);

		// ��������� ������� ��� ������������� ��������
		sprintf(&request[strlen(request)], PIPELINES_XML_ELEM_BEGIN, "RI", cnt + 1);
		sprintf(&request[strlen(request)], "%0.2f", R1);
		sprintf(&request[strlen(request)], PIPELINES_XML_ELEM_END, "RI", cnt + 1);

		// ��������� ������� ��� ������������� �������
		sprintf(&request[strlen(request)], PIPELINES_XML_ELEM_BEGIN, "RS", cnt + 1);
		sprintf(&request[strlen(request)], "%0.2f", R2);
		sprintf(&request[strlen(request)], PIPELINES_XML_ELEM_END, "RS", cnt + 1);
	 }

	 // ������ ������� �������
	 for(cnt = 0; cnt < mbIDsCnt; cnt++)
	 {
		// ��������� ������� ��� ������� �� ��������
		sprintf(&request[strlen(request)], PIPELINES_XML_ELEM_BEGIN, "UI", cnt + 1);
		sprintf(&request[strlen(request)], "%0.2f", GetUstVal(cnt, 0));
		sprintf(&request[strlen(request)], PIPELINES_XML_ELEM_END, "UI", cnt + 1);

		// ��������� ������� ��� ������� �� �������
		sprintf(&request[strlen(request)], PIPELINES_XML_ELEM_BEGIN, "US", cnt + 1);
		sprintf(&request[strlen(request)], "%0.2f", GetUstVal(cnt, 1));
		sprintf(&request[strlen(request)], PIPELINES_XML_ELEM_END, "US", cnt + 1);

	 }

	// ����� XML-������
	strcat(request, PIPELINES_XML_RESP_TAIL);
	
	// xml type
	return 2;	
}

#endif // _PIPELINES_

#endif // _HTTP_
