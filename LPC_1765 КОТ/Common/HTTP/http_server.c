/******************************************************************************
*                                                  
*  (c) copyright Freescale Semiconductor 2009
*  ALL RIGHTS RESERVED
*
*  File Name:   http_server.c
*                                                                          
*  Description: HTTP Server 
*                                                                              
*  Assembler:   Codewarrior for V1 V6.2.1
*                                            
*  Version:     1.1                                           
*                                                                              
*  Location:    Guadalajara,Mexico                                             
*                                                                              
*                                                  
* UPDATED HISTORY:
*
* REV   YYYY.MM.DD  AUTHOR        DESCRIPTION OF CHANGE
* ---   ----------  ------        --------------------- 
* 1.0   2009.05.19  Mr Alcantara  HTTP 2.0 server released
* 1.1   2009.06.10  Mr Alcantara  Changes to a lower RAM usage
* 
******************************************************************************/                  
/* Freescale  is  not  obligated  to  provide  any  support, upgrades or new */
/* releases  of  the Software. Freescale may make changes to the Software at */
/* any time, without any obligation to notify or provide updated versions of */
/* the  Software  to you. Freescale expressly disclaims any warranty for the */
/* Software.  The  Software is provided as is, without warranty of any kind, */
/* either  express  or  implied,  including, without limitation, the implied */
/* warranties  of  merchantability,  fitness  for  a  particular purpose, or */
/* non-infringement.  You  assume  the entire risk arising out of the use or */
/* performance of the Software, or any systems you design using the software */
/* (if  any).  Nothing  may  be construed as a warranty or representation by */
/* Freescale  that  the  Software  or  any derivative work developed with or */
/* incorporating  the  Software  will  be  free  from  infringement  of  the */
/* intellectual property rights of third parties. In no event will Freescale */
/* be  liable,  whether in contract, tort, or otherwise, for any incidental, */
/* special,  indirect, consequential or punitive damages, including, but not */
/* limited  to,  damages  for  any loss of use, loss of time, inconvenience, */
/* commercial loss, or lost profits, savings, or revenues to the full extent */
/* such  may be disclaimed by law. The Software is not fault tolerant and is */
/* not  designed,  manufactured  or  intended by Freescale for incorporation */
/* into  products intended for use or resale in on-line control equipment in */
/* hazardous, dangerous to life or potentially life-threatening environments */
/* requiring  fail-safe  performance,  such  as  in the operation of nuclear */
/* facilities,  aircraft  navigation  or  communication systems, air traffic */
/* control,  direct  life  support machines or weapons systems, in which the */
/* failure  of  products  could  lead  directly to death, personal injury or */
/* severe  physical  or  environmental  damage  (High  Risk Activities). You */
/* specifically  represent and warrant that you will not use the Software or */
/* any  derivative  work of the Software for High Risk Activities.           */
/* Freescale  and the Freescale logos are registered trademarks of Freescale */
/* Semiconductor Inc.                                                        */ 
/*****************************************************************************/

#include "config.h"
#if(_HTTP_ == DEVICE_ON)

#include <string.h>
#include <stdio.h>
#include "ModemTask.h"

/* ------------------------ FreeRTOS includes ----------------------------- */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* ------------------------ lwIP includes --------------------------------- */
#include "sys.h"
#include "sockets.h"

/* ------------------------ Project includes ------------------------------ */
#include "http_server.h"
#include "http_cgi.h"
#include "cookie.h"

/* ------------------------------ FAT ------------------------------------- */
#include "ff.h"    
#include "VFS.h"
#include "main.h"

/* ------------------------------ KOT ------------------------------------- */
#if defined(_PIPELINES_)
	#include "wap.h"
#endif

/********************Private Variables ***************************************/

char *errMes;

// ���� ��� ������� cookies
uint8_t Use_Cookies = 0;
// ������� ��� ����� � ��������� uart
xQueueHandle HTTPQueue;

const static char *httpErrors[] = {
	HTTP_NOT_FOUND_BODY,
	HTTP_METHOD_ERROR_BODY,
	HTTP_CGI_HANDLER_ERROR
};

const static char *MIMETypes[] = {
	MIME_HTML,
	MIME_TEXT,
	MIME_JS,
	MIME_CSS,
	MIME_GIF,
	MIME_JPEG,
	MIME_PNG
};

typedef enum {httpNotFound = 0, httpMethodError, httpCGIHandlError, /*httpCGIParamsError,*/ httpMemError, httpForbidden} HTTP_ERROR_CODES; 

/********************Private Functions ***************************************/

// ��������� ������� � ������� �������
static inline void toupper(char *str)
{
	int toUpperShift = 'a' - 'A';	

	while(*str)
	{
		if(*str >= 'a' && *str <= 'z')
			*str -= toUpperShift;

		str++;
	}
}

// �������� ������ ��� ��������� �������
static inline char*
HTTP_GetReqBuf(void)
{
	char *buf = pvPortMalloc(USER_HTTP_REQ_PROC_SPACE);
	if(!buf)
		return 0;

	memset(buf, 0, USER_HTTP_REQ_PROC_SPACE);

	return buf;
}

// ����������� ������ ��� ��������� �������
static inline void 
HTTP_FreeReqBuf(char *buf)
{
	vPortFree(buf);	
}

// ��������� ���������� ����� accept
static uint8_t HTTP_ProcessConnection(int soc);
// ��������� HTTP-�������
static void HTTP_ProcessRequest(int soc, char *request, uint8_t flag);
// �������� HTTP-�������� ��� ������ ���� � ������
static void HTTP_SendWebPage(int soc, char *file_name, char *temporal_buffer);
// �������� ��������� ����� ������� (�� ����) ����� ��������� �������
inline static void HTTP_SendData(int soc, char *array_to_send, uint16_t aray_length, uint8_t outTextType);
// �������� ������ 
static void HTTP_SendError(int soc, char *buf, HTTP_ERROR_CODES code);

static MIME_TYPES getMIMEType(char *file_name);

/**
 * Handle each request: timeout, close or request: HTTP 2.0 compliant
 *
 * @param �����
 * @param ��������� �����
 * @return 0 ���� ���������� �� �������
 */
static uint8_t
HTTP_ProcessConnection(int soc)
{
	char *buf = HTTP_GetReqBuf();
	if(!buf)
	{
		DEBUG_PUTS("HTTP MALLOC FAIL");

		HTTP_SendError(soc, 0, httpMemError);
		closesocket(soc);
		delay(20);
		return 0;
	}
	int len = recv(soc, (uint8_t *)buf, USER_HTTP_REQ_PROC_SPACE, 0);
	if(len > 0 && len != -1)
	{              
		//is this a GET method: assumig GET or POST id is at the first pbuf packet
		if( strstr(buf, HTTP_GET_REQUEST) != NULL )
		{
			HTTP_ProcessRequest(soc, buf, GET_FLAG);
		}
		else if( strstr(buf, HTTP_POST_REQUEST) != NULL )
		{
			HTTP_ProcessRequest(soc, buf, POST_FLAG);
		}
		else
		{
			HTTP_SendError(soc, buf, httpMethodError);
		}
		HTTP_FreeReqBuf(buf);
		return 0;
	}
	
	HTTP_FreeReqBuf(buf);
	//closesocket(soc);
	return -1;
}

/**
 * ������������ ������� GET � POST 
 *
 * @param �����
 * @param ����� � ��������
 * @param ��������� �����
 * @param ���� ���� ������� 
 * @return none
 */
static void 
HTTP_ProcessRequest(int soc, char *request, uint8_t flag)
{
	/*parsing variables*/
	char *end_temp;
	char *fileName, *buffer;
	uint16_t u16Length;
	
	/*pointing to a new buffer*/
	if( flag == GET_FLAG )
	{
		fileName = (char *)((uint32_t)request + (uint32_t)(strlen(HTTP_GET_REQUEST)));
		// ���� ������, �.�. ��� ������ url
		end_temp = (char *)strchr((const char *)fileName, ' ');
	}
	else if( flag == POST_FLAG )
	{
		fileName = (char *)((uint32_t)request + (uint32_t)(strlen(HTTP_POST_REQUEST)));
		// ���� ?-����������� url � ����������
		end_temp = (char *)strchr((const char *)fileName, '?');
	}

	u16Length = (uint16_t)(end_temp - fileName);
	
	//looking for file's name: assuming name's at first linked-list packet
	if(end_temp != NULL)
	{
		if(u16Length != 0 )
		{
			// ��������� ����� � �������� �� url 
			fileName[u16Length] = '\0';
			// � ��� ���������
			buffer = &fileName[u16Length + 1];

			toupper(fileName);
				
			// ���� ������ GET
			if(flag == GET_FLAG)
			{
#if defined(_PIPELINES_)
				// ���� ������ ���� xxx.xxx.xxx.xxx/wap
				// ���� ������� �������� ���� ��� �����
				if(strlen(fileName) == 3 && !strcmp(fileName, "WAP"))
				{
					SendWap(soc);
					return;
				}
#endif

				// ���� ���� �������� cookies, �� ��������� �� �� ����
				int res = Use_Cookies ? 1 : CheckCookies(fileName, buffer);

				if(res == 0)
				{
					// cookie �����������
					HTTP_SendError(soc, 0, httpForbidden);		
				}		
				else
				{
					// cookie ������
					// ������� ����� � �������� ����������� ��������
					HTTP_SendWebPage(soc, fileName, buffer);
				}
			}
			else if(flag == POST_FLAG)
			{
				uint8_t type = CGI_parser(fileName, buffer);
				if(type)
				{
					// ��������� ����������� �������
					if(buffer)
						// �������� �����
						HTTP_SendData(soc, buffer, (uint16_t)strlen(buffer), type);		                 
				}	
				else
				{
					//cgi not found
					HTTP_SendError(soc, buffer, httpCGIHandlError);          	        	
				}
			}
		}
		else//send DEFAULT WEB PAGE!!!!
		{
			HTTP_SendWebPage(soc, HTTP_LOGIN_PAGE_NAME, request);
		}
	}
	return;  
}

/** 
 * ���� mime-��� �� ���������� �����
 */
static MIME_TYPES getMIMEType(char *file_name)
{
	char *ext = strchr(file_name, '.');
	
	if(ext == 0)	
		return mime_html;

	ext++;
	if(!strcmp(ext, "HTM"))
		return mime_html;
	else if(!strcmp(ext, "JS"))
		return mime_javascript;
	else if(!strcmp(ext, "CSS"))
		return mime_css;
	else if(!strcmp(ext, "GIF"))
		return mime_gif;
	else if(!strcmp(ext, "JPG"))
		return mime_jpeg;
	else if(!strcmp(ext, "PNG"))
		return mime_png;
	else 
		return mime_text;
}

/**
 * �������� ����, ������� ������ ���������� �� ������
 *
 * @param �����
 * @��� �����
 * @��������� �����
 */
static void 
HTTP_SendWebPage(int soc, char *file_name, char *temporal_buffer)
{
	char *file_path = (char *)pvPortMalloc(sizeof("/at45db/WEB/") + strlen(file_name));
	if(file_path == 0)
	{
		HTTP_SendError(soc, 0, httpMemError);			
		return;
	}

	// �������, ��� file_name - ��� ������������� ���� ��� ������ �����
	strcpy(file_path, "/at45db/WEB/");
	strcat(file_path, file_name);	

	// ������� ������� ������������� ����
	FILE_OBJ fil;
	FRESULT Res = (FRESULT)VFS_open(&fil, file_path, FA_OPEN_EXISTING | FA_READ);
	DEBUG_PUTS(file_path);
	if(Res != FR_OK)
	{
		// ����� ������� ��� ������������ ������
		//VFS_close(&fil);

		// ��� ������ �����
		// ���� � ����� ������

		
		if(strcmp(file_name, "CAN_SNIF.BIN") == 0)
			strcpy(file_path, "/rom/");
		else
			strcpy(file_path, "/at45db/");
		strcat(file_path, file_name);
		
		// ������� ������� ������������� ���� 
		Res = (FRESULT)VFS_open(&fil, file_path, FA_OPEN_EXISTING | FA_READ);	
	}

	if(Res == FR_OK)
	{
		// ���� ����
		// ��������� � �������� �������� ��������� ������

		// ���������� ��� ���������
		MIME_TYPES type = getMIMEType(file_name);

		// ����� ������ ����� ���������
		sprintf(temporal_buffer, HTTP_HEADER_OK_PART1, MIMETypes[type]);

#if HTTP_GZIP_ON == DEVICE_ON
		// ��� ���� ����� ����� �������� ������ (�.�. ����� ���������� �� ������ ��������� ���� � ������, � ���������������� ���� �������)	
		// ��������� ���� Content-Encoding
		if(type != mime_text && type != mime_gif && type != mime_jpeg && type != mime_png)
			strcat(temporal_buffer, HTTP_HEADER_GZIP_PART);
#endif
		// ���� ���� �������� cookie
		if(Use_Cookies)
		{
			Use_Cookies = 0;
			// ��������� 
			AddCookies(&temporal_buffer[strlen(temporal_buffer)]);
		}
		// ����� ��������� ����� ���������
		sprintf(&temporal_buffer[strlen(temporal_buffer)], HTTP_HEADER_OK_PART2, fil.size);

		// ��������	
		send(soc, temporal_buffer, strlen(temporal_buffer), NULL);
		delay(50);
		// ������ ���� ��� ����
		uint32_t left_to_read = fil.size, len_to_read = (left_to_read > 512 ? 512 : left_to_read);
		// ���� ���� ��� ������
		VFS_fseek(&fil, 0);
		while(left_to_read)
		{
			// ��������� �����
			VFS_read(&fil, (uint8_t *)temporal_buffer, len_to_read);		
			// �������� �����	
			send(soc, temporal_buffer, len_to_read, NULL);
			delay(50);

			// ��������� ����������
			left_to_read -= len_to_read;
			// ����������, ������� ����� ������
			len_to_read = (left_to_read > 512 ? 512 : left_to_read);
		}

		VFS_close(&fil);
	} 
	else
	{
		//VFS_close(&fil);
		HTTP_SendError(soc, temporal_buffer, httpNotFound);	
		DEBUG_PUTS("HTTP FILE FAIL");
	}

	vPortFree(file_path);
}

/**
 * �������� ��������� �����, ���������� ����� ��������� CGI
 *
 * @param �����
 * @param �����
 * @param ����� ������
 * @return none
 */
char lenBuf[10];
inline static void
HTTP_SendData(int soc, char *array_to_send, uint16_t aray_length, uint8_t outTextType)
{
	DEBUG_PUTS("HTTP DEBUG: \r\n");
	DEBUG_PUTS(array_to_send);
	DEBUG_PUTS("\r\n");
	delay(500);

	uint32_t contentLen = strlen(array_to_send);
	if(contentLen > 9999)
		contentLen = 0;

	// ����� ������								 	
	sprintf(lenBuf, "%d\r\n\r\n", contentLen);

	// ���� ���������
	// ���� ��� ������
	if(outTextType == 1)
		// ������ �����
		send(soc, HTTP_CGI_HEADER_MIME_PLAIN, strlen(HTTP_CGI_HEADER_MIME_PLAIN), MSG_MORE);
	else if(outTextType == 2)
		// �������� XML
		send(soc, HTTP_CGI_HEADER_MIME_XML, strlen(HTTP_CGI_HEADER_MIME_XML), MSG_MORE);

	// ���� ����� ������
	send(soc, lenBuf, strlen(lenBuf), NULL);	
	delay(500);
	// ���� ��� ����� ������
	send(soc, array_to_send, aray_length, NULL);	 	
}

/**
 * �������� ����� � �������
 *
 * @param �����
 * @param �����
 * @param ��� ������
 * @return none
 */
inline static void 
HTTP_SendError(int soc, char *buf, HTTP_ERROR_CODES code)
{
	if(code == httpForbidden)
	{
		send(soc, HTTP_FORBIDDEN_HEADER, sizeof(HTTP_FORBIDDEN_HEADER), NULL);	
		send(soc, HTTP_FORBIDDEN_DATA, sizeof(HTTP_FORBIDDEN_DATA), NULL);
		return;
	}
	else if(code == httpMemError)
	{
		send(soc, HTTP_MEM_ERROR_HEADER, sizeof(HTTP_MEM_ERROR_HEADER), NULL);	
		send(soc, HTTP_MEM_ERROR_BODY, sizeof(HTTP_MEM_ERROR_BODY), NULL);
		return;	
	}

	// ����� ������ ��� �������
	int sendLen = sizeof(HTTP_ERROR_BODY_PART1) + strlen(httpErrors[code]) + sizeof(HTTP_ERROR_BODY_PART2);

	// ���������
	sprintf(buf, HTTP_ERROR_HEADER, sendLen);
	// ��������
	send(soc, buf, strlen(buf), NULL);

	// ���� ������
	strcpy(buf, HTTP_ERROR_BODY_PART1);
	strcat(buf, httpErrors[code]);
	strcat(buf, HTTP_ERROR_BODY_PART2);

	// ��������
	send(soc, buf, strlen(buf), NULL);
}

/********************Public Functions ***************************************/

void
httpTask( void *pvParameters )
{
	struct sockaddr_in addr;
	int timeout;
	unsigned long one = 1;
	int newSocket = -1;
	uint32_t alen = sizeof(addr);
	// ��� ��� ��������� Connection: Keep-Alive
	//web_state connection = WEB_CLOSED;
	//int count_keep_alive = 0;

	//http_queue = xQueueCreate(4, sizeof(MODEM_MESSAGE));

	/* Parameters are not used - suppress compiler error */
    ( void )pvParameters;

	int httpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(httpSocket < 0)
		goto http_server_exit;

  	timeout = 5000;

	memset(&addr, 0, sizeof(addr));
	addr.sin_len = sizeof(addr);
  	addr.sin_family      = AF_INET;
  	addr.sin_addr.s_addr = 0;
  	addr.sin_port        = htons(HTTP_PORT); // ��. bind, ��� �������� � netconn ���� ������������� � ���������� ����

	setsockopt(httpSocket, SOL_SOCKET, SO_REUSEADDR, (void*) &one, sizeof(one));
	setsockopt(httpSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	one = 1;
	ioctlsocket(httpSocket, FIONBIO, (void*) &one);    

  	if (bind(httpSocket, (struct sockaddr *)&addr, sizeof(struct sockaddr)) < 0)
  	{
     	closesocket(httpSocket);
      	return;
  	}

	listen(httpSocket, 5); // ������ �������� - ������� ������� ����� ������� �����������

    for(;;)
    { 
		// ��� ��� ��������� Connection: close
		if((newSocket = accept(httpSocket, (struct sockaddr *)&addr, (u32_t *)&alen)) >= 0)
		{
			setsockopt(newSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
			HTTP_ProcessConnection(newSocket);
			closesocket(newSocket);
			delay(1);
		}
		else
			delay(200);
		
		// ��� ��� ��������� Connection: Keep-Alive
		/*if((tempSocket = accept(httpSocket, (struct sockaddr *)&addr, (u32_t *)&alen)) >= 0)
		{
			setsockopt(tempSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
			if(newSocket >= 0)
				closesocket(newSocket);
			newSocket = tempSocket;
			count_keep_alive=0;
		}
		if(newSocket >= 0)
		{
			// Service connection: if closed or timeout app reached exit 
			if(HTTP_ProcessConnection(newSocket) != 0)
			{
				closesocket(newSocket);
				newSocket = -1;
				count_keep_alive=0;
			}
			else		
				count_keep_alive++;				
			delay(1);
		}
		else
			delay(200);
		*/
    }
/*not enough memory :S*/
http_server_exit:
    /*error*/
   // vPortFree(temporal_buffer);
    
    /*never get here!*/    
    return;
}

xTaskHandle CreateMyHTTP(char *name, unsigned portSHORT usStackDepth, void *pvParameters, unsigned portBASE_TYPE uxPriority )
{
	HTTPQueue = xQueueCreate(1, sizeof(COMMON_MESSAGE));
  return (xTaskHandle)sys_thread_new(name, httpTask, pvParameters, usStackDepth, uxPriority);
}

#endif // _HTTP_
