#ifndef WEB_CGI_H
#define WEB_CGI_H

#include "config.h"
#if(_HTTP_ == DEVICE_ON)

#if(_SECURITY_ == OPTION_ON)
	#define MAX_PASSW_LEN	MAX_PASS_LENGTH
	#define MAX_LOGIN_LEN	MAX_LOGIN_LENGTH
#else
	#define MAX_PASSW_LEN	0
	#define MAX_LOGIN_LEN	0
#endif 	

/*
 * The command table entry data structure
 */
typedef const struct
{
    char *command;                /* CGI string id  */
    int  (*func)(char *); /* actual function to call */
} CGI_CMD;

/*Функции обработки запросов*/

extern int 
LOGIN_process(char *request);
extern int 
SENSOR_process(char *request);
extern int 
CONFIG_process(char *request);
extern int 
INFO_process(char *request);
extern int 
FILE_process(char *request);
static int
DATETIME_process(char *request);
static int
REINIT_process(char *request);
static int
CAMERA_process(char *request);
static int
TERMINAL_process(char *request);
static int
CANSNIFFER_process(char *request);
static int
DISP_process(char *request);

#define CGI_LOGIN_PROCESS 		{"LOGIN", LOGIN_process}
#define CGI_SENSOR_PROCESS 		{"SENSOR", SENSOR_process}
#define CGI_CONFIG_PROCESS 		{"CONFIG", CONFIG_process}
#define CGI_INFO_PROCESS 		{"INFO", INFO_process}
#define CGI_FILE_PROCESS 		{"FILE", FILE_process}
#define CGI_DATETIME_PROCESS 	{"DATETIME", DATETIME_process}
#define CGI_REINIT_PROCESS 		{"REINIT", REINIT_process}
#define CGI_CAMERA_PROCESS 		{"CAMERA", CAMERA_process}
#define CGI_TERMINAL_PROCESS 	{"TERMINAL", TERMINAL_process}
#define CGI_CANSNIFFER_PROCESS 	{"CANSNIFFER", CANSNIFFER_process}
#define CGI_DISP_PROCESS 	{"DISP", DISP_process}

#define CGI_MAX_COMMANDS       	sizeof(CGI_CMD_ARRAY)/sizeof(CGI_CMD )

#define DOUBLE_END 				"\r\n\r\n"

#define CGI_FAIL_RESP		"error"			
#define CGI_OK_RESP			"correct"

#define XML_ELEM_BEGIN		"<%s>"
#define XML_ELEM_END		"</%s>"

/*Запрос <логин и пароль>*/

#define LOGIN_PARAM			"user="
#define PASSW_PARAM			"&pass="

#define LOGIN_ERROR_STRING	"alert(\"Неверный логин или пароль\")"

#if defined (_PIPELINES_)
	#define LOGIN_REDIR_STRING	"document.location.href = \"config.htm\""
#else
	#define LOGIN_REDIR_STRING	"document.location.href = \"info.htm\""
#endif

/*Запрос <датчик>*/

#define SENSORS_PARAM			"sensors="
#define SENSORS_CL_PARAM 		"clear="

#define SENSOR_XML_RESP_HEAD  	"<?xml version=\"1.0\" encoding=\"WINDOWS-1251\"?><body><sensors>"
#define SENSOR_XML_RESP_TAIL  	"</sensors></body>"

/*запрос <sms option>*/

//#define OPTION_NAME_PARAM		"option="
//#define OPTION_VAL_PARAM		"&value="

//#define OPTION_RESP_TEMPLATE	"document.OptForm.option_val.value=\"%s\""
//#define OPTION_ERR_RESP		"document.getElementById(\"OptSpan\").innerHTML = \"Error!\""
//#define OPTION_OK_RESP		"document.getElementById(\"OptSpan\").innerHTML = \"Ok!\""

/*запрос CONFIG <запись любого параметра на флешке>*/

#define CONFIG_FILE_PARAM		"file="
#define CONFIG_TOP_PARAM		"&top="
#define CONFIG_ELEMS_PARAMS		"&elems="
#define CINFIG_DEL_PARAM		"&delete"
			
/*запрос INFO <сервисные данные>*/

#define INFO_PARAM				"info="

#define INFO_XML_RESP_HEAD  	"<?xml version=\"1.0\" encoding=\"WINDOWS-1251\"?><body><info>"
#define INFO_XML_RESP_TAIL  	"</info></body>"

/*запрос FILE <запись части файла>*/

#define FILE_NAME_PARAM				"file="
#define FILE_SEEK_PARAM				"&seek="
#define FILE_TRUNC_PARAM		    "&truncate="
#define FILE_TEXT_PARAM		    	"&text="

/*запрос DATETIME <установка даты/времени>*/

#define DATETIME_DATE_PARAM				"date="
#define DATETIME_TIME_PARAM				"&time="

/*запрос CAMERA для записи, удаления фото и получения списка фотографий*/

#define CAMERA_LIST_PARAM				"photos_list="
#define CAMERA_SNAP_PARAM				"make_snap="
#define CAMERA_DELETE_PARAM				"delete_photos="

/*запрос TERMINAL для доступа к S485 и MBS*/

#define TERMINAL_TYPE_PARAM				"term_type="
#define TERMINAL_TEXT_PARAM				"&term_text="

#define TERMINAL_TYPE_BOOT				"boot"
#define TERMINAL_TYPE_RESET				"reset"
#define TERMINAL_TYPE_TERM				"term"
#define TERMINAL_TYPE_MBS				"mbs"

#define TERMINAL_TOUT_ANS				"--timeout--"



/**
 * Implements a CGI call
 *
 * @param CGI name
 * @param argument to be used for CGI called function 
 * @return returned value by function linked to execute by POST request
 */
int 
CGI_parser(char *name, char *request);

#endif //_HTTP_

#endif
