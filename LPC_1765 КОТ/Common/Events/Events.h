#include "Config.h"
#if(_EVENTS_)

#ifndef EVENTS_H
#define EVENTS_H

#include <stdint.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "SensorCore.h"


#define     MAX_NAME_LENGTH     13
#define     SIZE_EVENTS_QUEUE   30


typedef struct tagMessageEvent 
{
    uint8_t   ID;
    uint32_t  data;
	char  *name;
}EVENTS_MESSAGE;

typedef struct tagEventhandle
{
    uint8_t ID;
    char name[MAX_NAME_LENGTH];
    uint8_t exist_flag;
    struct tagEventhandle *pNext;
}EVENT_HANDLE;


typedef uint8_t(*CHECK_FUNC)(float res_1, float res_2);
typedef uint8_t(*BOOL_CHECK)(uint8_t res_1, uint8_t res_2);

typedef struct __attribute__ ((__packed__)) tagEventInfo 
{
    uint16_t 	event_ID;		 	// ��������� ������� ������������, ���� ������� ������� �����������
	char		*name;				//  ��������� �� ������ ���������� ��� ��� ���������
	char		*pOPZ;				//  ��������� �� ������ ���������� ��������� �������� �������� ������
    uint8_t 	status;			 	// ���������� ����������� �� �������
	uint8_t 	multiplicity_flag;	// ��������������� ���� ������� ������ ����������� ��� ������ �������� �������
    uint8_t 	event_exist_flag;	// ��������������� ��� ������ ������������� �������� �������
	char		mes_num[4];			// ����� ������ ��� ���� ������� �������� ���
	struct 		tagEventInfo *pNext;
}EVENT_INFO; 
 

uint8_t EventID(char *name);
int PutEvenToQueue(uint8_t id, uint32_t data, char *name, int timeout);
void add_event(EVENT_HANDLE *new_event);

void ClrEventExistFlag(uint8_t id);
void SetEventExistFlag(uint8_t id);
uint8_t GetEventExistFlag(uint8_t id);
char * Get_event_name(uint8_t id);
float opz_calc(char *opz);
char * opz_form(char *buf, int len);

void add_event_to_list(EVENT_INFO *pCheck);
void InitEventCheck(void);
void CheckEvents(void);
void DelAllEvents(void);

extern xQueueHandle events_queue;

#endif
#endif


