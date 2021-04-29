#include "Config.h"
#if(_HTTP_)

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "sockets.h"

#include "SensorCore.h"
#include "DriversCore.h"
#include "MB_server_device.h"
#include "clock_drv.h"

float GetUstVal(int ch, int is);

static uint8_t mbIDsBuf[4]; // ����� ��� modbus id (� ������ ������ �� ����� 4 ���������)																				   
static uint8_t mbIDsCnt;    // ���������� id

static const char httpFailResp[] = "HTTP/1.1 200 OK\r\nCache-Control: no-cache\r\nContent-type: text/html\r\nContent-length: 513\r\n\r\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\"><html><head><title>������</title><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\"></head><body><center><h1>������ �������.������������� ��������.</h1></center></body></html>";

static const char httpHeader[] = "HTTP/1.1 200 OK\r\nCache-Control: no-cache\r\nContent-type: text/html\r\nContent-length: 2056\r\n\r\n";

static const char httpServHead[] = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\"><html><head><title>��� v1.90 web-���������</title><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\"></head><body>";
static const char httpServTail[] = "</body></html>";

static const char httpServVersion[] = "<pre>����� ��������: %s</pre>";

static const char httpServCurrTime[] = "<pre>������� �����: %02d.%02d.%4d  %2d.%02d</pre>";

static const char httpServResTableHead[] = "<b style=\"font-size: x-large\">������� ���������� �������������</b><table border=\"1\" cellpadding=\"0\" bgcolor=\"#FFFF99\"><tr bgcolor=\"#FF3399\"> <th width=\"200\">����� �����</th><th width=\"200\">������������� ��������, ���</th><th width=\"200\">������������� ���������� ��������, ��</th></tr>";
static const char httpServResTableLine[] = "<tr><td>%d</td><td  bgcolor=\"%s\">%0.2f</td><td  bgcolor=\"%s\">%0.2f</td></tr>";

#define BG_COLOR_NORMAL "#FFFF99"
#define BG_COLOR_ERROR  "#FF3300"

static const char httpServUstTableHead[] = "<b style=\"font-size: x-large\">�������</b><table border=\"1\" cellpadding=\"0\" bgcolor=\"#33CCFF\"><tr bgcolor=\"#FFFF66\"> <th width=\"200\">����� �����</th><th width=\"200\">������� �� ������������� ��������, ���</th><th width=\"200\">������� �� ������������� ���������� ��������, ��</th></tr>";
static const char httpServUstTableLine[] = "<tr><td>%d</td><td>%0.1f</td><td>%0.1f</td></tr>";

static const char httpServTableSeparator[] = "<pre></pre>"; 

static const char httpServStatTableHead[] = "<b style=\"font-size: x-large\">����������</b><table border=\"1\" cellpadding=\"0\" bgcolor=\"#99FF33\"><tr bgcolor=\"#FFCC00\"> <th width=\"200\">ID ����������</th><th width=\"200\">������� ��������</th><th width=\"200\">������� �������</th></tr>";
static const char httpServStatTableLine[] = "<tr><td>%d</td><td>%d</td><td>%d</td></tr>";

static const char httpServTableTail[] = "</table>";

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ���� ������� HTTP_BUF_SIZE,
// ������� ����� Content-length � httpHeader
// �� ������� ��������� HTTP_BUF_SIZE, �� ������� ����� � ���
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define HTTP_BUF_SIZE	(2048 + 100)

#include "FreeRTOS.h"
#include "task.h"


#define HTTP_ASSERT(b,l)		\
if(b)							\
	goto l;						\


static char respBuf[128];

void SendWap(int soc) {

	DEVICE_HANDLE hHandle;
	uint8_t cnt;
	float res1, res2;
  
	int snd_len = 0;
	char *snd_buf = pvPortMalloc(HTTP_BUF_SIZE);
	if(!snd_buf)
	{
		// ��� ������
		// �������� ������
		send(soc, httpFailResp, sizeof(httpFailResp), NULL);
		return;
	}
	memset(snd_buf, 0, HTTP_BUF_SIZE);
	
  

  // ������ ���������� ���� � �� Modbus id
  DEVICE_HANDLE hMBS = OpenDevice("MBS");	
  DeviceIOCtl(hMBS, GET_SUPPORTED_IDs_NUMBER, &mbIDsCnt);
  DeviceIOCtl(hMBS, GET_SUPPORTED_IDs, mbIDsBuf);
  CloseDevice(hMBS);

  /* Send the HTML header 
         * subtract 1 from the size, since we dont send the \0 in the string
         * NETCONN_NOCOPY: our data is const static, so no need to copy it
   */
  strcat(snd_buf, httpHeader);
  strcat(snd_buf, httpServHead);

  // ���� �������� �����
  sprintf(respBuf, httpServVersion, VERS_ADDR);
  HTTP_ASSERT(strlen(snd_buf) + strlen(respBuf) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, respBuf);

  hHandle = OpenDevice("Clock");
  CLOCKData dateTime;

  SeekDevice(hHandle, 0, 0);
  ReadDevice(hHandle, &dateTime, sizeof(dateTime));

  // ���� ������� �����
  sprintf(respBuf, httpServCurrTime, dateTime.Date, dateTime.Month, dateTime.Year+2000, dateTime.Hour, dateTime.Min);
  CloseDevice(hHandle);
  hHandle = 0;
  
  HTTP_ASSERT(strlen(snd_buf) + strlen(respBuf) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, respBuf);

  /****************************************************************************************/
  /****************************************************************************************/

  // ���� ��������� �������	�������������
  HTTP_ASSERT(strlen(snd_buf) + strlen(httpServResTableHead) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, httpServResTableHead);

  cnt = 0;
  SENSOR_HANDLE sensHandle1, sensHandle2;

  // ���� ������ ������� ��� �������� ���� ModBus ID
  for(; cnt < mbIDsCnt; cnt++)
  {
  	// ��� ������� ���������� ���� 2 �������
	switch(cnt)
	{
		case 0: sensHandle1 = OpenSensorProc("S0");
				sensHandle2 = OpenSensorProc("S1");
				break;
		case 1: sensHandle1 = OpenSensorProc("S2");
				sensHandle2 = OpenSensorProc("S3");
				break;
		case 2:	sensHandle1 = OpenSensorProc("S4");
				sensHandle2 = OpenSensorProc("S5");
				break;
		case 3:	sensHandle1 = OpenSensorProc("S6");
				sensHandle2 = OpenSensorProc("S7");
				break;
		default:sensHandle1 = 0;
				sensHandle2 = 0;
				break;
	}
  	
	if(sensHandle1 == 0)
		res1 = -1.0f;
	else
		ReadSensor(sensHandle1, &res1, 1);
		
	if(sensHandle2 == 0)
		res2 = -1.0f;
	else
		ReadSensor(sensHandle2, &res2, 1);	

  	// ���� ��������� ������
  	sprintf(respBuf, httpServResTableLine, 
		cnt + 1,
		(res1 < GetUstVal(cnt, 0)) || (res1 == -1.0f) ? BG_COLOR_ERROR : BG_COLOR_NORMAL,
		res1, 
		(res2 > GetUstVal(cnt, 1)) || (res2 == -1.0f) ? BG_COLOR_ERROR : BG_COLOR_NORMAL, 
		res2);
	HTTP_ASSERT(strlen(snd_buf) + strlen(respBuf) >= HTTP_BUF_SIZE, http_send);
	strcat(snd_buf, respBuf);
  }

  // ���� ����� �������
  HTTP_ASSERT(strlen(snd_buf) + strlen(httpServTableTail) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, httpServTableTail);

  /***************************************************************************************/
  /***************************************************************************************/

  HTTP_ASSERT(strlen(snd_buf) + strlen(httpServTableSeparator) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, httpServTableSeparator);

  /***************************************************************************************/
  /***************************************************************************************/

  // ���� ��������� �������	�������
  HTTP_ASSERT(strlen(snd_buf) + strlen(httpServUstTableHead) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, httpServUstTableHead);

  for(cnt = 0; cnt < 4; cnt++)
  {
  	 // ���� ������ �������
     sprintf(respBuf, httpServUstTableLine, cnt + 1, GetUstVal(cnt, 0), GetUstVal(cnt, 1));   
	 HTTP_ASSERT(strlen(snd_buf) + strlen(respBuf) >= HTTP_BUF_SIZE, http_send);
	 strcat(snd_buf, respBuf);
  }

  // ���� ����� �������
  HTTP_ASSERT(strlen(snd_buf) + strlen(httpServTableTail) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, httpServTableTail);

  /***************************************************************************************/
  /***************************************************************************************/

  HTTP_ASSERT(strlen(snd_buf) + strlen(httpServTableSeparator) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, httpServTableSeparator);

  /***************************************************************************************/
  /***************************************************************************************/

  // ���� ��������� �������	����������
  HTTP_ASSERT(strlen(snd_buf) + strlen(httpServStatTableHead) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, httpServStatTableHead);

  int rx_count, tx_count;
  hHandle = OpenDevice("MBS");

  // ���� ������ ������� ��� �������� ���� ModBus ID
  for(cnt = 0; cnt < mbIDsCnt; cnt++)
  {
	// modbus id ���������� � �������� ���������
	// � ����-�� � ����������� �����
	rx_count = tx_count = (int)mbIDsBuf[cnt]; 
	
	DeviceIOCtl(hHandle, GET_TX_PACKETS_COUNT, (uint8_t *)&tx_count);	  
	DeviceIOCtl(hHandle, GET_RX_PACKETS_COUNT, (uint8_t *)&rx_count);	  

  	// ���� ��������� ������
  	sprintf(respBuf, httpServStatTableLine, mbIDsBuf[cnt], tx_count, rx_count);
	HTTP_ASSERT(strlen(snd_buf) + strlen(respBuf) >= HTTP_BUF_SIZE, http_send);
	strcat(snd_buf, respBuf);
  }

  CloseDevice(hHandle); // !!! ����������� �������, ��� �������� ������������� �������
  hHandle = 0;

  // ���� ����� �������
  HTTP_ASSERT(strlen(snd_buf) + strlen(httpServTableTail) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, httpServTableTail);

  /***************************************************************************************/
  /***************************************************************************************/

  http_send:
  
  if(hHandle) // ���� ������ ��������� � ����� ����������� ��������� �������, �� �� ����� �� ������� ����� ��������
	  CloseDevice(hHandle);

  // ���� ����� ��������
  if(strlen(snd_buf) + sizeof(httpServTail) < HTTP_BUF_SIZE)
  	strcat(snd_buf, httpServTail);

  snd_len = HTTP_BUF_SIZE - strlen(snd_buf);

  // ��� ���� ����� ������������� ������, ������� �������� � Content-length � httpHeader, ������� �������� ������, ��� �������� ������ �����
  // �� �������� �����, ������� �������� ��������� + ��� ���� + ���-�� ��������� ������
  // �� ������ ������ ��� ������ ����� �������
  if(snd_len > 0)
  	memset(&snd_buf[strlen(snd_buf)], '\n', snd_len);
  // ��������
  send(soc, snd_buf, HTTP_BUF_SIZE, NULL);

  vPortFree(snd_buf);
  return;
}

#endif
