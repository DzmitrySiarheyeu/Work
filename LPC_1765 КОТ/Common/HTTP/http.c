#include "Config.h"
#if(_HTTP_)

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"

static const char httpServHead[] = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\r\n<html>\r\n<head>\r\n<title>Кот v1.0 web-интерфейс</title>\r\n</head>\r\n<body>\r\n";
static const char httpServTail[] = "</body>\r\n</html>";

static const char httpServSerial[] = "<pre>Серийный номер: %d</pre>\r\n";

static const char httpServCurrTime[] = "<pre>Текущее время: %2d.%2d.%4d  %2d.%2d</pre>\r\n";

static const char httServInOut[] = "<pre>Пакетов принято: %d\r\nПакетов передано: %d</pre>\r\n";

static const char httpServTableHead[] = "<b style=\"font-size: x-large\"Таблица значений датчиков</b>\r\n<table border=\"1\" cellpadding=\"0\" bgcolor=\"#FFFF99\">\r\n<tr bgcolor=\"Red\"> <th width=\"200\">Номер датчика</th><th width=\"200\">Значение</th></tr>\r\n";
static const char httpServTableTail[] = "</table>\r\n";
static const char httpServTableLine[] = "<tr><td>%d</td><td>%10.3f</td></tr>\r\n";

static char respBuf[128];

void http_server_serve(struct netconn *conn) {
  struct netbuf *inbuf;
  char *buf;
  uint16_t buflen;
  
  /* Read the data from the port, blocking if nothing yet there. 
   We assume the request (the part we care about) is in one netbuf */
  inbuf = netconn_recv(conn);
  
  if (netconn_err(conn) == ERR_OK) 
  {
    netbuf_data(inbuf, (void *)&buf, &buflen);
    
    /* Is this an HTTP GET command? (only check the first 5 chars, since
    there are other formats for GET, and we're keeping it very simple )*/
    if (buflen>=5 && !strncmp(buf, "GET /", 5)) 
	{
      /* Send the HTML header 
             * subtract 1 from the size, since we dont send the \0 in the string
             * NETCONN_NOCOPY: our data is const static, so no need to copy it
       */
      netconn_write(conn, httpServHead, sizeof(httpServHead)-1, NETCONN_NOCOPY);

	  // Шлем серийный номер
	  sprintf(respBuf, httpServSerial, 123456);
	  netconn_write(conn, respBuf, strlen(respBuf), NETCONN_NOCOPY);

	  // Шлем текущее время
	  sprintf(respBuf, httpServCurrTime, 16, 5, 2011, 12, 0);
	  netconn_write(conn, respBuf, strlen(respBuf), NETCONN_NOCOPY);

	  // Шлем количество переданных и принятых пакетов
	  sprintf(respBuf, httServInOut, 123, 321);
	  netconn_write(conn, respBuf, strlen(respBuf), NETCONN_NOCOPY);

	  // Шлем заголовок таблицы
	  netconn_write(conn, httpServTableHead, sizeof(httpServTableHead)-1, NETCONN_NOCOPY);

      // Шлем табличные данные
	  sprintf(respBuf, httpServTableLine, 1, 100000);
	  netconn_write(conn, respBuf, strlen(respBuf), NETCONN_NOCOPY);
	  sprintf(respBuf, httpServTableLine, 2, 200000);
	  netconn_write(conn, respBuf, strlen(respBuf), NETCONN_NOCOPY);
	  sprintf(respBuf, httpServTableLine, 3, 300000);
	  netconn_write(conn, respBuf, strlen(respBuf), NETCONN_NOCOPY);

	  // Шлем конец таблицы
	  netconn_write(conn, httpServTableTail, sizeof(httpServTableTail)-1, NETCONN_NOCOPY);

	  // Шлем конец страницы
	  netconn_write(conn, httpServTail, sizeof(httpServTail)-1, NETCONN_NOCOPY);
    }
  }
  /* Close the connection (server closes in HTTP) */
  netconn_close(conn);
  
  /* Delete the buffer (netconn_recv gives us ownership,
   so we have to make sure to deallocate the buffer) */
  netbuf_delete(inbuf);
}

void httpTask(void *pvParameters) {
  struct netconn *conn, *newconn;

  for(;;)
  {
      /* Create a new TCP connection handle */
      conn = netconn_new(NETCONN_TCP);
  
      /* Bind to port 80 (HTTP) with default IP address */
      netconn_bind(conn, IP_ADDR_ANY, 80);
  
      /* Put the connection into LISTEN state */
      netconn_listen(conn);
  
      while(1) 
      {
        newconn = netconn_accept(conn);
        http_server_serve(newconn);
        netconn_delete(newconn);
      }
  }
}

#endif
