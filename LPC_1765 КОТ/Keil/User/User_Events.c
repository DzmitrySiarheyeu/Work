#include "Config.h"
#if(_EVENTS_)

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "User_Events.h"
#include "Events.h"
#include "DriversCore.h"
#include "SensorCore.h"
#include "Timers.h"
#include "display.h"
#include "ping.h"
#include "stats.h"

EVENT_HANDLE evPipe1_I_ON = {.ID = evKvit_1_i_on, .name = "kvit_1_p_on", .pNext = NULL};
EVENT_HANDLE evPipe1_S_ON = {.ID = evKvit_1_s_on, .name = "kvit_1_s_on", .pNext = NULL};
EVENT_HANDLE evPipe2_I_ON = {.ID = evKvit_2_i_on, .name = "kvit_2_p_on", .pNext = NULL};
EVENT_HANDLE evPipe2_S_ON = {.ID = evKvit_2_s_on, .name = "kvit_2_s_on", .pNext = NULL};
EVENT_HANDLE evPipe3_I_ON = {.ID = evKvit_3_i_on, .name = "kvit_3_p_on", .pNext = NULL};
EVENT_HANDLE evPipe3_S_ON = {.ID = evKvit_3_s_on, .name = "kvit_3_s_on", .pNext = NULL};
EVENT_HANDLE evPipe4_I_ON = {.ID = evKvit_4_i_on, .name = "kvit_4_p_on", .pNext = NULL};
EVENT_HANDLE evPipe4_S_ON = {.ID = evKvit_4_s_on, .name = "kvit_4_s_on", .pNext = NULL};
EVENT_HANDLE evPipe1_I_OFF = {.ID = evKvit_1_i_off, .name = "kvit_1_p_off", .pNext = NULL};
EVENT_HANDLE evPipe1_S_OFF = {.ID = evKvit_1_s_off, .name = "kvit_1_s_off", .pNext = NULL};
EVENT_HANDLE evPipe2_I_OFF = {.ID = evKvit_2_i_off, .name = "kvit_2_p_off", .pNext = NULL};
EVENT_HANDLE evPipe2_S_OFF = {.ID = evKvit_2_s_off, .name = "kvit_2_s_off", .pNext = NULL};
EVENT_HANDLE evPipe3_I_OFF = {.ID = evKvit_3_i_off, .name = "kvit_3_p_off", .pNext = NULL};
EVENT_HANDLE evPipe3_S_OFF = {.ID = evKvit_3_s_off, .name = "kvit_3_s_off", .pNext = NULL};
EVENT_HANDLE evPipe4_I_OFF = {.ID = evKvit_4_i_off, .name = "kvit_4_p_off", .pNext = NULL};
EVENT_HANDLE evPipe4_S_OFF = {.ID = evKvit_4_s_off, .name = "kvit_4_s_off", .pNext = NULL};

EVENT_HANDLE devFail1_ON = {.ID = devFail_1_on, .name = "qCheck_1_on", .pNext = NULL};
EVENT_HANDLE devFail2_ON = {.ID = devFail_2_on, .name = "qCheck_2_on", .pNext = NULL};
EVENT_HANDLE devFail3_ON = {.ID = devFail_3_on, .name = "qCheck_3_on", .pNext = NULL};
EVENT_HANDLE devFail4_ON = {.ID = devFail_4_on, .name = "qCheck_4_on", .pNext = NULL};
EVENT_HANDLE devFail1_OFF = {.ID = devFail_1_off, .name = "qCheck_1_off", .pNext = NULL};
EVENT_HANDLE devFail2_OFF = {.ID = devFail_2_off, .name = "qCheck_2_off", .pNext = NULL};
EVENT_HANDLE devFail3_OFF = {.ID = devFail_3_off, .name = "qCheck_3_off", .pNext = NULL};
EVENT_HANDLE devFail4_OFF = {.ID = devFail_4_off, .name = "qCheck_4_off", .pNext = NULL};

EVENT_HANDLE doorEvent_ON = {.ID = doorEv_on, .name = "doorEv_on", .pNext = NULL};
EVENT_HANDLE doorEvent_OFF = {.ID = doorEv_off, .name = "doorEv_off", .pNext = NULL};

extern xQueueHandle events_queue;

void kvit_event(uint8_t ev);
void fail_event(uint8_t ev);
void ResetClockTimer(void)	;

static char mesBuf[40]; 	// ����� ��� ������ � �����
extern uint8_t initErrorFlag;      // ���� ������ �������������
extern uint8_t startInitFlag;		// �������� ������ ��������, ������� � ������ ������
uint8_t DisplayBlinkFlag = 0;
extern int flag_pos;

// ��������� ������� �� �������� �������� ����� 
// �� ����������� � saver, ������ ��� �� ����� ����� ���������� �������
static uint8_t failStatusFlags = 0, failDisplayFlags = 0;
// ���� ��������� ������� �� �������� �����
static uint8_t doorEvFlag = 0;	
uint8_t getDoorStatus(void){return doorEvFlag;}

void QueueProcEvent(void);

uint32_t savedFlags[2];
uint32_t tempFlags[2];

#define savedEventsStatus 	 savedFlags[0]	 // ������� ������� �� �����
#define savedKvitStatus 	 savedFlags[1]	 // ������� ������������ �� �����

#define tempEventsStatus 	 tempFlags[0]	 // ������� ������� �� �����
#define tempKvitStatus 	     tempFlags[1]	 // ������� ������������ �� �����

void events_task(void *pvParameters)
{
	init_lcd();
    for(;;)
    {
		// ���� ����� ������ � ����� ��������, �� ��� �� ����� �������� � �� �� ������ �� ���������� �������������
		// �.�. ����� �������������� � �������
		// ������� ����������� ������������� �� ����� ������ �������������
		if(initErrorFlag == 1)
		{
			go_to_error_page();
			startInitFlag = 1;
			initErrorFlag = 2;	
		}

		if(startInitFlag == 1)
		{
			startInitFlag = 2;

			// ���� ���� ������, ������� ���� ����������
			// ���� ���� ������, �� ������ �� ����������
			if(!initErrorFlag)
				kvit_done(0);

			update_menu(1);	
		}	
        QueueProcEvent();
		delay(50);
    }
}

void QueueProcEvent(void)
{
    EVENTS_MESSAGE message;
	static uint32_t currEventStatus = 0; // ��������� ������� �� ��������
	static uint8_t  failStatus = 0;      // ��������� ������� �� �������� �������� �����
	static uint8_t  currDoorStatus = 0;  // ������� �����

    //���� �� ��������� � �������?
    while(xQueueReceive(events_queue, (void *)&message, 0))
	{   
		// ��������� ����� ������� 
		switch(message.ID)
		{
			case evKvit_1_i_on: currEventStatus |= (1 << 0); break;	 	
			case evKvit_1_s_on: currEventStatus |= (1 << 1); break;
			case evKvit_2_i_on: currEventStatus |= (1 << 2); break;
			case evKvit_2_s_on: currEventStatus |= (1 << 3); break;
			case evKvit_3_i_on: currEventStatus |= (1 << 4); break;
			case evKvit_3_s_on: currEventStatus |= (1 << 5); break;
			case evKvit_4_i_on: currEventStatus |= (1 << 6); break;
			case evKvit_4_s_on: currEventStatus |= (1 << 7); break;

			case evKvit_1_i_off: currEventStatus &= ~(1 << 0); break;	 	
			case evKvit_1_s_off: currEventStatus &= ~(1 << 1); break;
			case evKvit_2_i_off: currEventStatus &= ~(1 << 2); break;
			case evKvit_2_s_off: currEventStatus &= ~(1 << 3); break;
			case evKvit_3_i_off: currEventStatus &= ~(1 << 4); break;
			case evKvit_3_s_off: currEventStatus &= ~(1 << 5); break;
			case evKvit_4_i_off: currEventStatus &= ~(1 << 6); break;
			case evKvit_4_s_off: currEventStatus &= ~(1 << 7); break;

			case devFail_1_on : failStatus |= (1 << 0); break;
			case devFail_2_on : failStatus |= (1 << 1); break;
			case devFail_3_on : failStatus |= (1 << 2); break;
			case devFail_4_on : failStatus |= (1 << 3); break;

			case devFail_1_off : failStatus &= ~(1 << 0); break;
			case devFail_2_off : failStatus &= ~(1 << 1); break;
			case devFail_3_off : failStatus &= ~(1 << 2); break;
			case devFail_4_off : failStatus &= ~(1 << 3); break;

			case doorEv_on: 	 currDoorStatus = 1; break;
			case doorEv_off: 	 currDoorStatus = 0; break;
#if(_PING_)
			case PING_TIMER:
				PingPool();
			//	stats_display();
				break;
#endif

		case RESETE_TO_CLOCK_TIMER:
			reset_to_clock();
			break;		

		case BUTTON:
			ResetClockTimer();

			if(startInitFlag == 2)
				switch_menu((uint8_t)message.data);
		  
			break;

		case DISPLAY_UPDTAE_TIMER:
			DisplayBlinkFlag ^= 0xff;

			if(startInitFlag == 2)
				update_menu(0);
					
			break;
		}
	}

	if(!(startInitFlag && !initErrorFlag)) return;

	DEVICE_HANDLE hSaver;
//	// ����� ������ ����������� ����� ������� � ������������
//	DEVICE_HANDLE hSaver = OpenDevice("SAVER");
//	SeekDevice(hSaver, 0, 0);
//	int res = ReadDevice(hSaver, &savedFlags, 1);
//	CloseDevice(hSaver);
//
//	// �� ���������, ������, ��������, ������ ������ ��� � ��� �����
//	if(res == 0)
//	{
//		savedEventsStatus = 0;
//		savedKvitStatus = 0;	
//	}

	// ���������� ������� ��������� ����������� ������
	tempEventsStatus = savedEventsStatus;
	tempKvitStatus = savedKvitStatus;

	// ���� ��������� ������� �� �������� �����
	// ���� �������� � �����
	if(currDoorStatus == 1 && doorEvFlag == 0)
	{
		memset(mesBuf, 0, sizeof(mesBuf));
		sprintf(mesBuf, "����� �������");
		PUT_MES_TO_LOG(mesBuf, 0, 0);	
	}

	doorEvFlag = currDoorStatus;

	int i;
	// �� ���� ��������� �����������
	for(i = 0; i < 4; i++)
	{
		// ���� ���� ������ �����
		if((failStatus & (1 << i)) > 0)
		{
			// ���� � ������� ��� �� �� ����
			if((failStatusFlags & (1 << i)) == 0)
			{
				set_fail_page(1 << i);
				// �������� ������� ������� �� �������
				fail_event(i);
				// ���������� �� ������, ���� �� ������ ���������� ������
				failDisplayFlags |= (1 << i);
				
				memset(mesBuf, 0, sizeof(mesBuf));
				sprintf(mesBuf, "����� %d: ��� �����", i + 1);
				PUT_MES_TO_LOG(mesBuf, 0, 0);	
			}
		}
		// ���� ������ ����� ���, � � ������� ��� ����
		else if((failStatusFlags & (1 << i)) > 0)
			// ������� ����� ������ �����
			clr_fail_page(1 << i);
	}
	
	failStatusFlags = failStatus;

	// ��������� �� ����������� ������
	for(i = 0; i < 4; i++)
	{
		if(failStatusFlags & (1 << i))
		{
			// ����� �������� ������� �� �������� ���� ��� �����
			switch(i)
			{
				case 0:	currEventStatus &= ~((1 << 0) | (1 << 1));
						savedEventsStatus &= ~((1 << 0) | (1 << 1));
						savedKvitStatus |=  ((1 << 0) | (1 << 1)); break; 
	
				case 1:	currEventStatus &= ~((1 << 2) | (1 << 3));
						savedEventsStatus &= ~((1 << 2) | (1 << 3));
						savedKvitStatus |=  ((1 << 2) | (1 << 3)); break;
	
				case 2:	currEventStatus &= ~((1 << 4) | (1 << 5));
						savedEventsStatus &= ~((1 << 4) | (1 << 5));
						savedKvitStatus |=  ((1 << 4) | (1 << 5)); break;
	
				case 3:	currEventStatus &= ~((1 << 6) | (1 << 7));
						savedEventsStatus &= ~((1 << 6) | (1 << 7));
						savedKvitStatus |=  ((1 << 6) | (1 << 7)); break;
			}
		}
	}

	// ��� ������ ��������� ����� ������� ���� �����
	if((currEventStatus & (1 << 1)))
		set_obriv_flag(1 << 1);
	else
		clr_obriv_flag(1 << 1);

	if((currEventStatus & (1 << 3)))
		set_obriv_flag(1 << 3);
	else
		clr_obriv_flag(1 << 3);

	if((currEventStatus & (1 << 5)))
		set_obriv_flag(1 << 5);
	else
		clr_obriv_flag(1 << 5);

	if((currEventStatus & (1 << 7)))
		set_obriv_flag(1 << 7);
	else
		clr_obriv_flag(1 << 7);

	// �� ���� ��������� ��������
	for(i = 0; i < 8; i++)
	{
		// ���� ���� �������
		if((currEventStatus & (1 << i)))
		{
			// ������� �� �������������?
			if(!(savedKvitStatus & (1 << i)))
			{
				// ������������� ���� ������� �������������
				set_blink_flag(1 << i);

				// �������� ������� ������� �� �������
				kvit_event(i);
				
				// ����� �������� ������� �� ������, ���� ������ �� ���������� ��� �������
				// �� ����� ����� �������� ������� � �����
				// ��� ������� ������� ����� � ��� ������ ���� ��� �� �������
				if(!(savedEventsStatus & (1 << i)))
				{	
					memset(mesBuf, 0, sizeof(mesBuf));
								
					SENSOR_HANDLE hSens;
					float val;
					switch(i)
					{
						case 0: hSens = OpenSensorProc("S0"); break;
						case 1: hSens = OpenSensorProc("S1"); break;
						case 2: hSens = OpenSensorProc("S2"); break;
						case 3: hSens = OpenSensorProc("S3"); break;
						case 4: hSens = OpenSensorProc("S4"); break;
						case 5: hSens = OpenSensorProc("S5"); break;
						case 6: hSens = OpenSensorProc("S6"); break;
						case 7: hSens = OpenSensorProc("S7"); break;
					}
					
					if(hSens == 0)
						sprintf(mesBuf, "�������: ��� �������");
					else
					{
						switch(i)
						{
							case 0: sprintf(mesBuf, "����� 1, ��������\rR="); break;
							case 1: sprintf(mesBuf, "����� 1, ������\rR="); break;
							case 2: sprintf(mesBuf, "����� 2, ��������\rR="); break;
							case 3: sprintf(mesBuf, "����� 2, ������\rR="); break;
							case 4: sprintf(mesBuf, "����� 3, ��������\rR="); break;
							case 5: sprintf(mesBuf, "����� 3, ������\rR="); break;
							case 6: sprintf(mesBuf, "����� 4, ��������\rR="); break;
							case 7: sprintf(mesBuf, "����� 4, ������\rR="); break;
						}

						ReadSensor(hSens, &val, 1);

						switch(i)
						{
							case 0: 
							case 2: 
							case 4: 
							case 6: sprintf(&mesBuf[strlen(mesBuf)], "%0.1f ���", val); break;

							case 1: 
							case 3: 
							case 5: 
							case 7: sprintf(&mesBuf[strlen(mesBuf)], "%0.1f ��", val); break;
						}
							
					}

					PUT_MES_TO_LOG(mesBuf, 0, 0);		
						
				}
				// �� ������ ���� �� ������ ���������� ������ ������, ���������� ��
				// ���� ���� ���������� ����� ������ ����� �������������
				savedEventsStatus |= (1 << i); 	
			}
		}
		// ������� ���, �.�. �������� � ���������� ��������� 
		else 
		{
			// ����� �������� ���� ������������
			savedKvitStatus &= ~(1 << i);				
		}
	}

	// ���� ����� ����������, �� ���� ���������
	if(tempEventsStatus != savedEventsStatus || tempKvitStatus != savedKvitStatus)
	{
		hSaver = OpenDevice("SAVER");
		SeekDevice(hSaver, 0, 0);
		WriteDevice(hSaver, &savedFlags, 1);
		CloseDevice(hSaver);	
	}
}

void kvit_done(uint8_t page)
{
//	uint32_t savedFlags[2];
//
//#define savedEventsStatus 	 savedFlags[0]	 // ������� ������� �� �����
//#define savedKvitStatus 	 savedFlags[1]	 // ������� ������������ �� �����
//
	DEVICE_HANDLE hSaver = OpenDevice("SAVER");
//	SeekDevice(hSaver, 0, 0);
//	int res = ReadDevice(hSaver, &savedFlags, 1);
//	if(res == 0)
//	{
//		savedFlags[0] = 0;
//		savedFlags[1] = 0;
//	}
	
	// ���������� ����� ������ � ������������� ����� ������������
	switch(page)		
	{
		case 1: savedEventsStatus &= ~(1 << 0);
				savedEventsStatus &= ~(1 << 1); 
				savedKvitStatus |= (1 << 0);
				savedKvitStatus |= (1 << 1);
				clr_blink_flag(1 << 0);
				clr_blink_flag(1 << 1);
				GPIO_SetValue(3, (1 << 26));

				failDisplayFlags &= ~(1 << 0);
				break;
		case 2:	savedEventsStatus &= ~(1 << 2);
				savedEventsStatus &= ~(1 << 3); 
				savedKvitStatus |= (1 << 2);
				savedKvitStatus |= (1 << 3);
				clr_blink_flag(1 << 2);
				clr_blink_flag(1 << 3);
				GPIO_SetValue(3, (1 << 25));

				failDisplayFlags &= ~(1 << 1);
				break;
		case 3:	savedEventsStatus &= ~(1 << 4);
				savedEventsStatus &= ~(1 << 5); 
				savedKvitStatus |= (1 << 4);
				savedKvitStatus |= (1 << 5);
				clr_blink_flag(1 << 4);
				clr_blink_flag(1 << 5);
				GPIO_SetValue(1, (1 << 28));

				failDisplayFlags &= ~(1 << 2);
				break;
		case 4:	savedEventsStatus &= ~(1 << 6);
				savedEventsStatus &= ~(1 << 7); 
				savedKvitStatus |= (1 << 6);
				savedKvitStatus |= (1 << 7);
				clr_blink_flag(1 << 6);
				clr_blink_flag(1 << 7);
				GPIO_SetValue(1, (1 << 29));

				failDisplayFlags &= ~(1 << 3);
				break;
	}

	SeekDevice(hSaver, 0, 0);
	WriteDevice(hSaver, &savedFlags, 1);
	CloseDevice(hSaver);

	// ����������� ������ ����� ����� �����������
	int i;
	// �� ���� ��������� �����������
	for(i = 0; i < 4; i++)
	{
		// ���� ����� ���������� ������ �����
		if((failDisplayFlags & (1 << i)) > 0)
		{
			set_fail_page(1 << i);
			fail_event(i);
			break;
		}
	}

	// �� ���� ��������� ��������
	for(i = 0; i < 8; i++)
	{
		// ���� ���� ����������� ������� ������ ��� �� �����
		if((savedEventsStatus & (1 << i)))
		{ 
			// ��� ������ ������������ �������������� ��� ���������� ������ �����
			kvit_event(i);

			// ��� ��������� ���������� ��� ������� ���������� ����� ����� ������������� ������� 
			// ��� ������������� � �������� ������ �������� ���������� �� �������� ��������
			set_blink_flag(1 << i);
		}	
	}
}

void kvit_event(uint8_t ev)
{
	switch(ev)
	{
		case 0:
		case 1: lock_page(1);
				GPIO_ClearValue(3, (1 << 26));
				break;
	    case 2:
		case 3:	lock_page(2);
				GPIO_ClearValue(3, (1 << 25));
				break;
		case 4:
		case 5:	lock_page(3);
				GPIO_ClearValue(1, (1 << 28));
				break;
		case 6:
		case 7:	lock_page(4);
				GPIO_ClearValue(1, (1 << 29));
				break;
	}
}

void fail_event(uint8_t ev)
{
	switch(ev)
	{
		case 0:	lock_page(1);break;
		case 1: lock_page(2);break;
	    case 2:	lock_page(3);break;
		case 3:	lock_page(4);break;
	}
}

void InitEvents(void)
{
    events_queue = xQueueCreate(SIZE_EVENTS_QUEUE, sizeof(EVENTS_MESSAGE));

	add_event(&evPipe1_I_ON);
	add_event(&evPipe1_S_ON);
	add_event(&evPipe2_I_ON);
	add_event(&evPipe2_S_ON);
	add_event(&evPipe3_I_ON);
	add_event(&evPipe3_S_ON);
	add_event(&evPipe4_I_ON);
	add_event(&evPipe4_S_ON);
	add_event(&evPipe1_I_OFF);
	add_event(&evPipe1_S_OFF);
	add_event(&evPipe2_I_OFF);
	add_event(&evPipe2_S_OFF);
	add_event(&evPipe3_I_OFF);
	add_event(&evPipe3_S_OFF);
	add_event(&evPipe4_I_OFF);
	add_event(&evPipe4_S_OFF);
	add_event(&devFail1_ON);
	add_event(&devFail2_ON);
	add_event(&devFail3_ON);
	add_event(&devFail4_ON);
	add_event(&devFail1_OFF);
	add_event(&devFail2_OFF);
	add_event(&devFail3_OFF);
	add_event(&devFail4_OFF);
	add_event(&doorEvent_ON);
	add_event(&doorEvent_OFF);

	// ���������� ������
	GPIO_SetValue(3, (1 << 26));
	GPIO_SetValue(3, (1 << 25));

	GPIO_SetValue(1, (1 << 28));
	GPIO_SetValue(1, (1 << 29));

	GPIO_SetDir(3, 1 << 26, 1);	// 1
	GPIO_SetDir(3, 1 << 25, 1);	// 2

	GPIO_SetDir(1, 1 << 28, 1);	// 3
	GPIO_SetDir(1, 1 << 29, 1);	// 4

	DEVICE_HANDLE hSaver = OpenDevice("SAVER");
	SeekDevice(hSaver, 0, 0);
	int res = ReadDevice(hSaver, &savedFlags, 1);
	CloseDevice(hSaver);

	// �� ���������, ������, ��������, ������ ������ ��� � ��� �����
	if(res == 0)
	{
		savedEventsStatus = 0;
		savedKvitStatus = 0;	
	}
}

#endif
