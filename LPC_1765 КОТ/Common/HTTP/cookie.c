#include "config.h"
#if(_HTTP_ == DEVICE_ON)

#include <string.h>
#include <stdio.h>

#include "cookie.h"
#include "security.h"
#include "http_cgi.h"

#define COOKIES_TEMPLATE		"Set-Cookie: user=%s;\r\nSet-Cookie: pass=%s;\r\n"
#define COOKIE 					"Cookie: "
#define LOGIN_COOKIE			"user="
#define PASSW_COOKIE			"pass="

static char passwCookie[MAX_PASSW_LEN + 1], loginCookie[MAX_LOGIN_LEN + 1];

int CheckCookies(char *file, char *req)
{
#if(_SECURITY_ == OPTION_OFF)
	return 1;
#endif
	
	// ищем расширение файла
	char *ext;
	// не нашли - ошибка
	if( (ext = strchr(file, '.')) == 0 )
		return 0;
	ext++;

	// если запрос - html-страница, cookie будем проверять
	if(!strcmp(ext, "HTM") || !strcmp(ext, "HTML") || !strcmp(ext, "SHTM") || !strcmp(ext, "SHTML"))
	{				
		char *temp = strstr(req, COOKIE);
		if(temp == 0)
			return 0;
	
		temp += sizeof(COOKIE) - 1;
	
		char login[MAX_LOGIN_LEN + 1], passw[MAX_PASSW_LEN + 1];
		memset(login, 0, sizeof(login));
		memset(passw, 0, sizeof(passw));
	
		// ищем пароль и логин
		char *loginPos = strstr(temp, LOGIN_COOKIE), *passwPos = strstr(temp, PASSW_COOKIE);
		int loginLen = 0, passwLen = 0;
		
		if(loginPos == 0 || passwPos == 0)			
			return 0;	
	
		// логин
		loginPos += sizeof(LOGIN_COOKIE) - 1;
		while(*loginPos != 0 && *loginPos != ';' && *loginPos != '\r' && *loginPos != '\n' && loginLen < MAX_LOGIN_LEN)
			login[loginLen++] = *loginPos++;
	
		// пароль
		passwPos += sizeof(PASSW_COOKIE) - 1;
		while(*passwPos != 0 && *passwPos != ';' && *passwPos != '\r' && *passwPos != '\n' && passwLen < MAX_PASSW_LEN)
			passw[passwLen++] = *passwPos++;
	

		loginLen = strlen(login) > strlen(loginCookie) ? strlen(login) : strlen(loginCookie);
		passwLen = strlen(passw) > strlen(passwCookie) ? strlen(passw) : strlen(passwCookie);

		// сравниваем с сохраненными паролями
		int res = !(strncmp(login, loginCookie, loginLen) || strncmp(passw, passwCookie, passwLen));

#if(_SECURITY_ == OPTION_ON)
		// и SECURITY
		res = res || NetAccessRequest(login, passw);
#endif

		return res;
	}
		
	return 1;
}

void SetCookies(char *login, char *passw)
{
	uint8_t procID[33];
	uint8_t codeWord[8];

	GetProcessorID((char *)procID);

	uint8_t i;
	for(i = 0; i < 8; i++)
		codeWord[i] = procID[i] & procID[i + 8] ^ procID[i + 16] | procID[i + 24];	

	for(i = 0; i < 8; i++)
	{
		passwCookie[i] = passw[i] ^ codeWord[i];
		loginCookie[i] = login[i] ^ codeWord[i];	

		// для EI
		// он не видит непечатные символы в cookie,
		// поэтому все символы приводяться к диапазону печатных
		passwCookie[i] = passwCookie[i] % 0x5D + 0x21;
		loginCookie[i] = loginCookie[i] % 0x5D + 0x21;
			

		// нельзя использовать
		if(passwCookie[i] == ';')
			passwCookie[i]++;
		if(loginCookie[i] == ';')
			loginCookie[i]++;
		if(passwCookie[i] == 0x27 || passwCookie[i] == '\"')
			passwCookie[i]++;
		if(loginCookie[i] == 0x27 || loginCookie[i] == '\"')
			loginCookie[i]++;
	}
//	strcpy(passwCookie, passw);
//	strcpy(loginCookie, login);
}



void AddCookies(char *req)
{
	sprintf(req, COOKIES_TEMPLATE, loginCookie, passwCookie);
}

#endif // _HTTP_
