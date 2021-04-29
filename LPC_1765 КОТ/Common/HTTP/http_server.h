#ifndef _HTTP_WEB_H_
#define _HTTP_WEB_H_

#include "config.h"
#if(_HTTP_ == DEVICE_ON)


/* ------------------------ Defines ---------------------------------------- */
/********************* The port on which we listen. **************************/
#define HTTP_PORT                   80
#define HTTP_REQ_PROC_SPACE		    (1024+256)
#define GET_FLAG                    0
#define POST_FLAG                   1

/****************** Priorities for the application tasks. ********************/
#define HTTP_TASK_PRIORITY          ( tskIDLE_PRIORITY + 1 )

/********************************* MIME. *************************************/
typedef enum {mime_html = 0, mime_text, mime_javascript, mime_css, mime_gif, mime_jpeg, mime_png} MIME_TYPES;

#define	MIME_HTML "text/html"
#define	MIME_TEXT "text/plain"
#define MIME_JS   "text/javascript"
#define	MIME_CSS  "text/css"
#define MIME_XML  "text/xml"
#define MIME_GIF  "image/gif"
#define MIME_JPEG "image/jpeg"	 
#define MIME_PNG  "image/png"


/*****************************HTTP Requests***********************************/
#define HTTP_GET_REQUEST            "GET /"
#define HTTP_POST_REQUEST           "POST /"

/****************************HTTP OK Responses********************************/

#define HTTP_HEADER_OK_PART1		"HTTP/1.1 200 OK\r\nCache-Control: no-cache\r\nContent-type: %s\r\n"
#define HTTP_HEADER_OK_PART2		"Content-length: %d\r\nConnection: close\r\n\r\n"

#define HTTP_HEADER_GZIP_PART		"Content-Encoding: gzip\r\n"

#define HTTP_CGI_HEADER_MIME_PLAIN	"HTTP/1.1 200 OK\r\nCache-Control: no-cache\r\nConnection: close\r\nContent-type: text/plain\r\nContent-length: "
#define HTTP_CGI_HEADER_MIME_XML	"HTTP/1.1 200 OK\r\nCache-Control: no-cache\r\nConnection: close\r\nContent-type: text/xml\r\nContent-length: "

/**************************HTTP ERROR Responses*******************************/
// в качестве общей ошибки используем код 500 - внутренняя ошибка сервера
#define	HTTP_ERROR_HEADER			"HTTP/1.1 500 Internal Server Error\r\nCache-Control: no-cache\r\nContent-type: text/html\r\nContent-length: %d\r\n\r\n"

// тело HTTP-ответа об ошибке
#define HTTP_ERROR_BODY_PART1		"<html><head><title>500</title></head><body bgcolor=\"white\"><center><h1>500 Ошибка сервера</h1></center><hr><center>"
#define HTTP_ERROR_BODY_PART2		"</center></body></html>"

// файл не найден
#define HTTP_NOT_FOUND_BODY			"Страница не найдена."
// неизвестный метод (не GET и не POST)
#define HTTP_METHOD_ERROR_BODY 		"Метод не поддерживается."
// ошибка при обработке запроса CGI (нет обработчика)
#define HTTP_CGI_HANDLER_ERROR		"CGI-скрипт не найден."
// не найдены все параметры при обработке CGI
/*#define HTTP_CGI_PARAMS_ERROR		"Не хватает параметров для обработки CGI."*/	

// если не смогли выделить память, то не сможет отправить ошибку, поэтому в виде статических буферов
#define	HTTP_MEM_ERROR_HEADER		"HTTP/1.1 500 Internal Server Error\r\nCache-Control: no-cache\r\nContent-type: text/html\r\nContent-length: 158\r\n\r\n" 
#define HTTP_MEM_ERROR_BODY			"<html><head><title>500</title></head><body bgcolor=\"white\"><center><h1>500 Ошибка сервера</h1></center><hr><center>Не хватает памяти.</center></body></html>"

// для ошибки доступа без пароля и логина другая ошибка
#define HTTP_FORBIDDEN_HEADER		"HTTP/1.1 403 Forbidden\r\nCache-Control: no-cache\r\nContent-type: text/html\r\nContent-length: 183\r\n\r\n"
#define HTTP_FORBIDDEN_DATA			"<html><head><title>403</title></head><body bgcolor=\"white\"><center><h1>403 Нет доступа</h1></center><hr><center>Для просмотра страницы необходима авторизация.</center></body></html>"


/**************************Имена по умолчанию*********************************/
#define HTTP_LOGIN_PAGE_NAME		"AVT.HTM"

/*Web Connection Options*/
typedef enum
{
  WEB_CLOSED,
  WEB_OPEN
} web_state;

/********************Public Functions ***************************************/

extern char *errMes;
// выражение, метка, сообщение
#define HTTP_ASSERT(b,l,m)			\
	if(b)							\
	{								\
		errMes = m;					\
		goto l;						\
	}								

/**
 * Start an embedded HTTP server Task: 1 client and multiple files per transfer
 *
 * @param none
 * @return none
 */
xTaskHandle CreateMyHTTP( char *name, unsigned portSHORT usStackDepth, void *pvParameters, unsigned portBASE_TYPE uxPriority );
void httpTask( void *pvParameters );
#endif //_HTTP_

#endif
