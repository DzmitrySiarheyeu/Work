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

static uint8_t mbIDsBuf[4]; // буфер для modbus id (в данной версии не более 4 устройств)																				   
static uint8_t mbIDsCnt;    // количество id

static const char httpFailResp[] = "HTTP/1.1 200 OK\r\nCache-Control: no-cache\r\nContent-type: text/html\r\nContent-length: 513\r\n\r\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\"><html><head><title>Ошибка</title><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\"></head><body><center><h1>Ошибка сервера.Перезагрузите страницу.</h1></center></body></html>";

static const char httpHeader[] = "HTTP/1.1 200 OK\r\nCache-Control: no-cache\r\nContent-type: text/html\r\nContent-length: 2056\r\n\r\n";

static const char httpServHead[] = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\"><html><head><title>Кот v1.90 web-интерфейс</title><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\"></head><body>";
static const char httpServTail[] = "</body></html>";

static const char httpServVersion[] = "<pre>Номер прошивки: %s</pre>";

static const char httpServCurrTime[] = "<pre>Текущее время: %02d.%02d.%4d  %2d.%02d</pre>";

static const char httpServResTableHead[] = "<b style=\"font-size: x-large\">Таблица измеренных сопротивлений</b><table border=\"1\" cellpadding=\"0\" bgcolor=\"#FFFF99\"><tr bgcolor=\"#FF3399\"> <th width=\"200\">Номер трубы</th><th width=\"200\">Сопротивление изоляции, кОм</th><th width=\"200\">Сопротивление сигнальных проводов, Ом</th></tr>";
static const char httpServResTableLine[] = "<tr><td>%d</td><td  bgcolor=\"%s\">%0.2f</td><td  bgcolor=\"%s\">%0.2f</td></tr>";

#define BG_COLOR_NORMAL "#FFFF99"
#define BG_COLOR_ERROR  "#FF3300"

static const char httpServUstTableHead[] = "<b style=\"font-size: x-large\">Уставки</b><table border=\"1\" cellpadding=\"0\" bgcolor=\"#33CCFF\"><tr bgcolor=\"#FFFF66\"> <th width=\"200\">Номер трубы</th><th width=\"200\">Уставка по сопротивлению изоляции, кОм</th><th width=\"200\">Уставка по сопротивлению сигнальных проводов, Ом</th></tr>";
static const char httpServUstTableLine[] = "<tr><td>%d</td><td>%0.1f</td><td>%0.1f</td></tr>";

static const char httpServTableSeparator[] = "<pre></pre>"; 

static const char httpServStatTableHead[] = "<b style=\"font-size: x-large\">Статистика</b><table border=\"1\" cellpadding=\"0\" bgcolor=\"#99FF33\"><tr bgcolor=\"#FFCC00\"> <th width=\"200\">ID устройства</th><th width=\"200\">Пакетов передано</th><th width=\"200\">Пакетов принято</th></tr>";
static const char httpServStatTableLine[] = "<tr><td>%d</td><td>%d</td><td>%d</td></tr>";

static const char httpServTableTail[] = "</table>";

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// если меняешь HTTP_BUF_SIZE,
// поменяй число Content-length в httpHeader
// на сколько изменяешь HTTP_BUF_SIZE, на столько меняй и там
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
		// нет памяти
		// высылаем ошибку
		send(soc, httpFailResp, sizeof(httpFailResp), NULL);
		return;
	}
	memset(snd_buf, 0, HTTP_BUF_SIZE);
	
  

  // читаем количество труб и их Modbus id
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

  // Шлем серийный номер
  sprintf(respBuf, httpServVersion, VERS_ADDR);
  HTTP_ASSERT(strlen(snd_buf) + strlen(respBuf) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, respBuf);

  hHandle = OpenDevice("Clock");
  CLOCKData dateTime;

  SeekDevice(hHandle, 0, 0);
  ReadDevice(hHandle, &dateTime, sizeof(dateTime));

  // Шлем текущее время
  sprintf(respBuf, httpServCurrTime, dateTime.Date, dateTime.Month, dateTime.Year+2000, dateTime.Hour, dateTime.Min);
  CloseDevice(hHandle);
  hHandle = 0;
  
  HTTP_ASSERT(strlen(snd_buf) + strlen(respBuf) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, respBuf);

  /****************************************************************************************/
  /****************************************************************************************/

  // Шлем заголовок таблицы	сопротивлений
  HTTP_ASSERT(strlen(snd_buf) + strlen(httpServResTableHead) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, httpServResTableHead);

  cnt = 0;
  SENSOR_HANDLE sensHandle1, sensHandle2;

  // шлем строки таблицы для котороых есть ModBus ID
  for(; cnt < mbIDsCnt; cnt++)
  {
  	// для каждого устройства есть 2 датчика
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

  	// Шлем табличные данные
  	sprintf(respBuf, httpServResTableLine, 
		cnt + 1,
		(res1 < GetUstVal(cnt, 0)) || (res1 == -1.0f) ? BG_COLOR_ERROR : BG_COLOR_NORMAL,
		res1, 
		(res2 > GetUstVal(cnt, 1)) || (res2 == -1.0f) ? BG_COLOR_ERROR : BG_COLOR_NORMAL, 
		res2);
	HTTP_ASSERT(strlen(snd_buf) + strlen(respBuf) >= HTTP_BUF_SIZE, http_send);
	strcat(snd_buf, respBuf);
  }

  // Шлем конец таблицы
  HTTP_ASSERT(strlen(snd_buf) + strlen(httpServTableTail) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, httpServTableTail);

  /***************************************************************************************/
  /***************************************************************************************/

  HTTP_ASSERT(strlen(snd_buf) + strlen(httpServTableSeparator) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, httpServTableSeparator);

  /***************************************************************************************/
  /***************************************************************************************/

  // Шлем заголовок таблицы	уставок
  HTTP_ASSERT(strlen(snd_buf) + strlen(httpServUstTableHead) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, httpServUstTableHead);

  for(cnt = 0; cnt < 4; cnt++)
  {
  	 // шлем строку таблицы
     sprintf(respBuf, httpServUstTableLine, cnt + 1, GetUstVal(cnt, 0), GetUstVal(cnt, 1));   
	 HTTP_ASSERT(strlen(snd_buf) + strlen(respBuf) >= HTTP_BUF_SIZE, http_send);
	 strcat(snd_buf, respBuf);
  }

  // Шлем конец таблицы
  HTTP_ASSERT(strlen(snd_buf) + strlen(httpServTableTail) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, httpServTableTail);

  /***************************************************************************************/
  /***************************************************************************************/

  HTTP_ASSERT(strlen(snd_buf) + strlen(httpServTableSeparator) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, httpServTableSeparator);

  /***************************************************************************************/
  /***************************************************************************************/

  // Шлем заголовок таблицы	статистики
  HTTP_ASSERT(strlen(snd_buf) + strlen(httpServStatTableHead) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, httpServStatTableHead);

  int rx_count, tx_count;
  hHandle = OpenDevice("MBS");

  // шлем строки таблицы для котороых есть ModBus ID
  for(cnt = 0; cnt < mbIDsCnt; cnt++)
  {
	// modbus id высылается в качестве параметра
	// в него-же и принимается ответ
	rx_count = tx_count = (int)mbIDsBuf[cnt]; 
	
	DeviceIOCtl(hHandle, GET_TX_PACKETS_COUNT, (uint8_t *)&tx_count);	  
	DeviceIOCtl(hHandle, GET_RX_PACKETS_COUNT, (uint8_t *)&rx_count);	  

  	// Шлем табличные данные
  	sprintf(respBuf, httpServStatTableLine, mbIDsBuf[cnt], tx_count, rx_count);
	HTTP_ASSERT(strlen(snd_buf) + strlen(respBuf) >= HTTP_BUF_SIZE, http_send);
	strcat(snd_buf, respBuf);
  }

  CloseDevice(hHandle); // !!! Обязательно закрыть, при открытии захватывается семафор
  hHandle = 0;

  // Шлем конец таблицы
  HTTP_ASSERT(strlen(snd_buf) + strlen(httpServTableTail) >= HTTP_BUF_SIZE, http_send);
  strcat(snd_buf, httpServTableTail);

  /***************************************************************************************/
  /***************************************************************************************/

  http_send:
  
  if(hHandle) // Если ассрет произошел в цикле определения устройств модбасс, то мы могли не закрыть хендл модбасса
	  CloseDevice(hHandle);

  // Шлем конец страницы
  if(strlen(snd_buf) + sizeof(httpServTail) < HTTP_BUF_SIZE)
  	strcat(snd_buf, httpServTail);

  snd_len = HTTP_BUF_SIZE - strlen(snd_buf);

  // наш сайт имеет фиксированный размер, который прописан в Content-length в httpHeader, который заведомо больше, чем реальный размер сайта
  // мы высылаем буфер, который включает заголовок + сам сайт + что-то останется пустым
  // мы должны забить это пустое место мусором
  if(snd_len > 0)
  	memset(&snd_buf[strlen(snd_buf)], '\n', snd_len);
  // отсылаем
  send(soc, snd_buf, HTTP_BUF_SIZE, NULL);

  vPortFree(snd_buf);
  return;
}

#endif
