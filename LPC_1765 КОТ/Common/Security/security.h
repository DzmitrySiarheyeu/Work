#ifndef _SECURITY_H_
#define _SECURITY_H_
#include "Config.h"
#if(_SECURITY_ == OPTION_ON)


#define		ADMIN_LOGIN_LENGTH()	(strlen(ADMIN_LOGIN))

#define		AUTH_PHONE_NUM_LENGTH		15



/******************************Для IAP комманды чтения ID процессора******************************/

#define CMD_SUCCESS 		0

/*************************************************************************************************/

typedef struct tagAuntificInfo
{
	char 		login[MAX_LOGIN_LENGTH + 1];					//   пароль пользователя ets
	char 		password[MAX_PASS_LENGTH + 1];				//   пароль пользователя ets
}AUTH_INFO;

typedef struct tagSecurityInfo
{
	AUTH_INFO  *auth_list;    		   //   указатель на список догинов и паролей в системе
	uint16_t	number_auth_records;   //   количество учетных записей
	char	    admin_pas[MAX_PASS_LENGTH];	       //   пароль высчитанный из ID процессора
	char	    sms_pas[MAX_PASS_LENGTH];	       //   пароль для смс собщений
	uint16_t	numder_sms_num;		   //   количсиво разрешенных номеров (если это параметр равняется нулю фильтрация по номерам отключена)
	char	   *phone_num_list;   	   //   Указатель на массив с номерами телефонов			 
}SECURITY_INFO;
	
#endif  //  _SECURITY_

//   Прататипы глобальных функций

int NetAccessRequest(char *login, char *password);
int SmsAccessRequest(char *num, char *password);
void GetPassword(char *password, uint16_t num);
void GetProcessorID(char *id);
void SecurityInit(void);
void DelAllSecurity(void);

#endif   //  _SECURITY_H_    

