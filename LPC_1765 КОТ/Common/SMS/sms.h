#ifndef _SMS_H_
#define _SMS_H_


#define MAX_SMS_NAME_LENGTH       15
#define MAX_SMS_BUFF_LENGTH       128
#define PHONE_NUM_LENGTH          16



//-***********************************************************


//#define       ERROR_SMS      "ERROR"
//#define       OK_SMS         "OK"


typedef struct tagSMSdata
{
    char *name;
    char *arg;
		char *pass;
		uint8_t count; 
}SMS_DATA;

typedef struct tagSMSInfo
{
    char index[5];
    char phon_num[16];
    char name[MAX_SMS_NAME_LENGTH];
    char buf[MAX_SMS_BUFF_LENGTH];
}SMS_INFO;

typedef struct tagSMShandle
{
    uint8_t ID;
    const char *name; //[MAX_SMS_NAME_LENGTH];
    struct tagSMShandle *pNext;
}SMS_HANDLE;


void InitSMS(void);    //  Инициализация сообщений
int sms_proces(void);
int sms_perform(uint8_t ID, char *data, uint8_t count);
uint8_t get_sms_ID(char *name);

#endif // _SMS_H_
