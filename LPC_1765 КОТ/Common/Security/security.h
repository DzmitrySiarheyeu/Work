#ifndef _SECURITY_H_
#define _SECURITY_H_
#include "Config.h"
#if(_SECURITY_ == OPTION_ON)


#define		ADMIN_LOGIN_LENGTH()	(strlen(ADMIN_LOGIN))

#define		AUTH_PHONE_NUM_LENGTH		15



/******************************��� IAP �������� ������ ID ����������******************************/

#define CMD_SUCCESS 		0

/*************************************************************************************************/

typedef struct tagAuntificInfo
{
	char 		login[MAX_LOGIN_LENGTH + 1];					//   ������ ������������ ets
	char 		password[MAX_PASS_LENGTH + 1];				//   ������ ������������ ets
}AUTH_INFO;

typedef struct tagSecurityInfo
{
	AUTH_INFO  *auth_list;    		   //   ��������� �� ������ ������� � ������� � �������
	uint16_t	number_auth_records;   //   ���������� ������� �������
	char	    admin_pas[MAX_PASS_LENGTH];	       //   ������ ����������� �� ID ����������
	char	    sms_pas[MAX_PASS_LENGTH];	       //   ������ ��� ��� ��������
	uint16_t	numder_sms_num;		   //   ��������� ����������� ������� (���� ��� �������� ��������� ���� ���������� �� ������� ���������)
	char	   *phone_num_list;   	   //   ��������� �� ������ � �������� ���������			 
}SECURITY_INFO;
	
#endif  //  _SECURITY_

//   ��������� ���������� �������

int NetAccessRequest(char *login, char *password);
int SmsAccessRequest(char *num, char *password);
void GetPassword(char *password, uint16_t num);
void GetProcessorID(char *id);
void SecurityInit(void);
void DelAllSecurity(void);

#endif   //  _SECURITY_H_    

