#include "config.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "main.h"
#include "display.h"
#include "LCD 133X65_UC1601S.h"
#include "clock_digits.h"
#include "icons.h"

#include "SensorCore.h"
#include "DriversCore.h"
#include "clock_drv.h"
#include "MB_server_device.h"
#include "User_Events.h"
#include "lwip_port.h"
#include "log.h"
#include "Modem_DRV.h"

#include "lpc17xx_gpio.h"
#include "Security.h"

uint8_t * GetOpName(void);  //
uint8_t GetSigLevel(void); 
uint8_t Sim_state_get(void);
char * GetOurIPStr(void);

typedef struct
{
	uint8_t mbIDsBuf[4]; // ����� ��� modbus id (� ������ ������ �� ����� 4 ���������)
	uint8_t mbIDsCnt;    // ���������� id
} __attribute__ ((__packed__)) MB_IDs_STRUCT;

static MB_IDs_STRUCT mbIDs;

typedef struct 
{
	// �������� ����
	uint8_t page;
	// �������� ������� ������
	uint8_t subService;
	// �������� ������� �����
	uint8_t subArchive;
	// ����������, ��� ������ ������ �� ��������� ������
	uint8_t blinkFlags;
	// ����������, �� ����� ��������� ���� ������ �����
	uint8_t failFlags;
	// ����� �������
	uint8_t obrivFlags;
	// ����������, �� ����� �������� �� ����� � ���� ������������
	uint8_t pageLock;
	// ���� ��������� �����
	uint8_t timeSetupFlag;
	// ��� ����������� � �����
	uint8_t timeSetupTarget;
	//������������ ��������� ����� ������ ���� ��� ����������
	//��� ��������� ����� ���������� ����������
	char timePrev[6];
} __attribute__ ((__packed__)) DISPLAY_CONTR_REG;

static DISPLAY_CONTR_REG dispContrReg = {.pageLock = 0xff};
									 
//���������� ��������� ���� �����
/*const struct tagDigitPlace clock_place[4] = {
                                            {.X = 11,
                                             .Y = 12},
                                            {.X = 36,
                                             .Y = 12},
                                            {.X = 76,
                                             .Y = 12},
                                            {.X = 102,
                                             .Y = 12}}; */
const struct tagDigitPlace clock_place[4] = {
                                            {.X = 2,
                                             .Y = 12},
                                            {.X = 27,
                                             .Y = 12},
                                            {.X = 67,
                                             .Y = 12},
                                            {.X = 93,
                                             .Y = 12}};   

//���������� ��������� ������ ����
const struct tagDigitPlace menu_icon_place[7] = {
                                                {.X = 1,
                                                 .Y = 45},
                                                {.X = 20,
                                                 .Y = 45},
                                                {.X = 39,
                                                 .Y = 45},
                                                {.X = 58,
                                                 .Y = 45},
                                                {.X = 77,
                                                 .Y = 45},
                                                {.X = 96,
                                                 .Y = 45},
                                                {.X = 115,
                                                 .Y = 45}}; 

extern uint8_t DisplayBlinkFlag; // ���� ������� ������� �������� �� ������� ����� �� ����� � ������� ������� ��� ������
extern ePPPThreadControl ePPPThrCtl;    // ������ ppp

#define CLR_MENU() 	LCD_erase_area(0,0,132,42) // ������� ������� ����
												 
void update_menu(uint8_t drawMenuFlag);												 

//������ ������ ������(��������)
void make_icon_dark(uint8_t icon_number)
{
	LCD_draw_image(menu_icon_place[icon_number].X,menu_icon_place[icon_number].Y,17,18,&icons[icon_number + 7][0],51);
}

//������ ������ �������(����������)
void make_icon_bright(uint8_t icon_number)
{
	LCD_draw_image(menu_icon_place[icon_number].X,menu_icon_place[icon_number].Y,17,18,&icons[icon_number][0],51);
}

// ����������� ������ ������/�������
void toggleIcons(uint8_t brightIcon, uint8_t darkIcon)
{
	make_icon_bright(brightIcon);
	make_icon_dark(darkIcon);
}

// ���������� x ��� ����������� ������ �� ������ ������
int setCentred(char *text)
{
	return (int)(133/2 - strlen(text)*6/2);
}

void LCD_reset(void)
{
	GPIO_SetValue(0, 1 << 17);
	GPIO_SetDir(0, 1 << 17, 1);		
}

//�������������
void init_lcd(void)
{
	LCD_reset();
    //�����������
    LCD_init();
    //�������
    LCD_clear();

	// ���� ��������� ���������� id � modbus, ����� �����, ������� ���� � ��������������� ��������
	DEVICE_HANDLE hMBS = OpenDevice("MBS");
	DeviceIOCtl(hMBS, GET_SUPPORTED_IDs_NUMBER, &mbIDs.mbIDsCnt);
	DeviceIOCtl(hMBS, GET_SUPPORTED_IDs, mbIDs.mbIDsBuf);
	CloseDevice(hMBS);

	if(mbIDs.mbIDsCnt > 4)
		mbIDs.mbIDsCnt = 4;
    
    make_icon_dark(0);
	
	// ������� ������� �������, ������� ���� id
	uint8_t i = 1;
	for(; i <= mbIDs.mbIDsCnt; i++)
		make_icon_bright(i);

	make_icon_bright(5);
	make_icon_bright(6);

	LCD_setXY(setCentred("�������������"), 28);
	printf("�������������");
}

//������� ����������� �����
void clear_time(void)
{
	memset(dispContrReg.timePrev,0,sizeof(dispContrReg.timePrev));
}

//���� �������� 60*5 � �� ������ ������ - ��������� �� ����
void reset_to_clock(void)
{
    if(dispContrReg.page == 0) return; 			// ��� �� �����
	if(dispContrReg.timeSetupFlag == 1) return;	// ����������� ����
	if(dispContrReg.pageLock != 0xff) return;   // ������� ������ �� �������� 
	if(dispContrReg.page == 8) return;			// ���������� ��������� �� ������

	dispContrReg.subService = 0;				// ����� ������� �� ������ ���� �� � ������ ��� �������
	dispContrReg.subArchive = 0;

	toggleIcons(dispContrReg.page, 0);
	dispContrReg.page = 0;
	update_menu(1);
}

void go_to_error_page(void)
{
	dispContrReg.page = 8; // �������� � �������
	CLR_MENU();
	
	LCD_setXY(2, 18);
	printf("��� ��������������");
	LCD_setXY(2, 27);
	printf("�������� ������.");	
	LCD_setXY(2, 36);
	printf("�������� �����.");
}

/************************************************/
/************************************************/
/**************������������ ������***************/
/************************************************/
/************************************************/

void switch_clock_menu(uint8_t press);
void switch_service_menu(uint8_t press);
void switch_archive_menu(uint8_t press);
void switch_error_menu(uint8_t press);

// ������������ �������� �����
void switch_menu(uint8_t press)
{
    if(press == KEY_OFF)
    {
        update_menu(0);
        return;
    }

	// ���� �� �� �������� ������
	if(dispContrReg.pageLock != 0xff)
	{
		// ��������� �� �������� �������
		if(press == KEY_ON_2000ms)
		{			
			// ������� ���� 
			dispContrReg.pageLock = 0xff;
			kvit_done(dispContrReg.page);
			update_menu(1);			
		}

		return;
	}

	if(dispContrReg.page == 0)
		switch_clock_menu(press);
	else if(dispContrReg.page >= 1 && dispContrReg.page <= 4)
	{
		if(dispContrReg.page == mbIDs.mbIDsCnt)	
		{
			toggleIcons(dispContrReg.page, 5);
			dispContrReg.page = 5;	
		}
		else
		{
			toggleIcons(dispContrReg.page, dispContrReg.page+1);
			dispContrReg.page++;
		}
		update_menu(1);	
	}
	else if(dispContrReg.page == 5)
		switch_service_menu(press);
	else if(dispContrReg.page == 6)
		switch_archive_menu(press);	
	else if(dispContrReg.page == 8)	
		switch_error_menu(press);
}

//��������� ������� ������ � ������ �����
void switch_clock_menu(uint8_t press)
{
    //����������� ������������� �������� �� �������� �������
    if(press == KEY_ON_2000ms && dispContrReg.timeSetupFlag == 1) 
    {
        switch(dispContrReg.timeSetupTarget)
        {
            //����������� ��������� ��������
            case SETUP_MIN   : 
            case SETUP_HOUR  : 
            case SETUP_DATE  : 
            case SETUP_MONTH : dispContrReg.timeSetupTarget++; break;
            case SETUP_YEAR  : dispContrReg.timeSetupTarget = SETUP_NOTH; break;

            case SETUP_NOTH : dispContrReg.timeSetupTarget = SETUP_MIN; break;
        }

        //������� ����������� �����, ����� ���������� ��������� ����������
        clear_time();
        return;
    }

    //�� ��������� ������� ������ ����������� ��������
    if(press == KEY_ON_100ms && dispContrReg.timeSetupFlag == 1)
    {
		uint8_t time;
		DEVICE_HANDLE hClock = OpenDevice("Clock");

        switch(dispContrReg.timeSetupTarget)
        {
		    //������++
            case SETUP_MIN  : SeekDevice(hClock, MIN_D, 0);
							  ReadDevice(hClock, &time, 1);
							  if(++time > 59) 
                                  time = 0;
                              SeekDevice(hClock, MIN_D, 0);
                              WriteDevice(hClock, &time, 1);
                              break;

            //����++
            case SETUP_HOUR : SeekDevice(hClock, HOUR_D, 0);
							  ReadDevice(hClock, &time, 1);
							  if(++time > 23) 
                                  time = 0;
                              SeekDevice(hClock, HOUR_D, 0);
                              WriteDevice(hClock, &time, 1);
                              break;
            
            //����� ������++
            case SETUP_DATE  :SeekDevice(hClock, DATE_D, 0);
							  ReadDevice(hClock, &time, 1);
							  if(++time > 31) 
                                  time= 1;
                              SeekDevice(hClock, DATE_D, 0);
                              WriteDevice(hClock, &time, 1);
                              break;

            //�����++
            case SETUP_MONTH  : SeekDevice(hClock, MONTH_D, 0);
							    ReadDevice(hClock, &time, 1);
							    if(++time > 12) 
                                    time = 1;
                                SeekDevice(hClock, MONTH_D, 0);
                                WriteDevice(hClock, &time, 1);
                                break;

            //���++
            case SETUP_YEAR  : SeekDevice(hClock, YEAR_D, 0);
							   ReadDevice(hClock, &time, 1);
							   if(++time > 19) 
                                   time = 0;
                               SeekDevice(hClock, YEAR_D, 0);
                               WriteDevice(hClock, &time, 1);
                               break;

            //��������� � ��������� ��������
            case SETUP_NOTH : dispContrReg.timeSetupFlag = 0;
							  // ���� ���� ���� � ��������������� - ��������� �� ���
							  if(mbIDs.mbIDsCnt)
							  {
							  	 toggleIcons(dispContrReg.page, dispContrReg.page+1); 
                              	 dispContrReg.page = 1;
                              	 
							  }
							  // ��� ����� �� ������
							  else
							  {
							  	 toggleIcons(dispContrReg.page, 5); 
                              	 dispContrReg.page = 5;	 
							  }
							  update_menu(1);
                              break;

        }

		CloseDevice(hClock);

        //������� ����������� �����, ����� ���������� ��������� ����������
        clear_time();
		update_menu(0);
        return;
    } 

	// ���� ���� ���� � ��������������� - ��������� �� ���
	if(mbIDs.mbIDsCnt)
	{
		toggleIcons(dispContrReg.page, dispContrReg.page+1); 
        dispContrReg.page = 1;
                         	 
	}
	// ��� ����� �� ������
	else
	{
		toggleIcons(dispContrReg.page, 5); 
        dispContrReg.page = 5;	 
	}
    update_menu(1);
}

uint8_t getDoorStatus(void);

void switch_service_menu(uint8_t press)
{
    switch(press)
    {
        //�������� �������
                           //���� �� �� ��������� � �������
        case KEY_ON_100ms: if(dispContrReg.subService == 0)
						   {	
                              //��������� � ���������� �����
							  toggleIcons(dispContrReg.page, dispContrReg.page+1);
							  dispContrReg.page++;
							  update_menu(1);
						   }
						   else 
						   {
						   	   // ������� ����������
						   	   if(dispContrReg.subService >= 1 && dispContrReg.subService <= 4)
							   {
						   	      // ��������� ����� � 5 �������, ���� ��������� modbus id
							      if(dispContrReg.subService == mbIDs.mbIDsCnt)	
			                         dispContrReg.subService = 5;	
								  // ��������� � ���������� ������� 
		                          else
			                         dispContrReg.subService++;
							   }
							   // ������� ������� ModBus
							   else if(dispContrReg.subService >= 5 && dispContrReg.subService <= 8)
							   {
						   	      // ��������� ����� � ������������ �������, ���� ��������� modbus id
							      if(dispContrReg.subService == (mbIDs.mbIDsCnt + 4))	
			                         dispContrReg.subService = SERVICE_INDEP_SUBMENU;	
								  // ��������� � ���������� ������� 
		                          else
			                         dispContrReg.subService++;
							   }
							   else
						   	   {
						   	      dispContrReg.subService++;
							      //���� �������� ����� �������
							      if(dispContrReg.subService > SERVICE_MENU_DEPTH)
									 // ���� ���� modbus id
								     if(mbIDs.mbIDsCnt)
							            //�������� � ������� ������ �������
							   	        dispContrReg.subService = 1;
									 else
									    //�������� � ������������ ������ �������
							   	        dispContrReg.subService = SERVICE_INDEP_SUBMENU;
							   } 	   
							   update_menu(1);	   
						   }
						   break;

							// ������� � ������� �� �������� �������
        case KEY_ON_2000ms: if(dispContrReg.subService == 0)
							{
                                // ���� ���� modbus id
								if(mbIDs.mbIDsCnt)
							    	//�������� � ������� ������ �������
							   	    dispContrReg.subService = 1;
								else
									//�������� � ������������ ������ �������
							   	    dispContrReg.subService = SERVICE_INDEP_SUBMENU;
								update_menu(1);
							}
							// ��������� ����� - ����� ��������� �������
                            else if(dispContrReg.subService == SERVICE_MENU_DEPTH)
                            {
								// ������ ����������� ���� ��� �������� �����
								if(getDoorStatus() == 0)
									return;								

								dispContrReg.subService = 0;

								toggleIcons(dispContrReg.page, 0);
								dispContrReg.page = 0;

								dispContrReg.timeSetupFlag = 1;
								dispContrReg.timeSetupTarget = SETUP_MIN;

							  	clear_time();
								
								update_menu(1);	
							}
							// ������� �� �������
							else
							{
								dispContrReg.subService = 0;
								
								toggleIcons(dispContrReg.page, dispContrReg.page+1);
								dispContrReg.page++;
							  	update_menu(1);
                            }
                            break;

        default : break;
    }
}

void switch_archive_menu(uint8_t press)
{ 
    switch(press)
    {
                           //���� �� �� ��������� � �������
        case KEY_ON_100ms: if(dispContrReg.subArchive == 0)
						   {
						   	  //��������� � ���������� �����
							  toggleIcons(dispContrReg.page, 0);
                              dispContrReg.page = 0;
							  clear_time();
							  update_menu(1);	
                           }
                           else
                           {
                              update_menu(1);
                           }
                           break;

        case KEY_ON_2000ms: if(dispContrReg.subArchive == 0)
                            {
                                dispContrReg.subArchive = 1;
                                update_menu(1);
                            }
                            else
                            {
								dispContrReg.subArchive = 0;
                                //��������� � ���������� �����
								toggleIcons(dispContrReg.page, 0);
                              	dispContrReg.page = 0;
							  	clear_time();
							  	update_menu(1);
                            }
                            break;

        default :           break;
    }
}

void switch_error_menu(uint8_t press)
{
	if(press == KEY_ON_2000ms)
	{
		dispContrReg.page = 0;
		clear_time();
		update_menu(1);	
	}
}

/************************************************/
/************************************************/
/***************���������� ������****************/
/************************************************/
/************************************************/

void update_resistors_menu(uint8_t drawMenuFlag);
void update_service_menu(uint8_t drawMenuFlag);
void update_archive_menu(uint8_t drawMenuFlag);
void update_clock_menu(uint8_t drawMenuFlag);

// drawMenuFlag ����� ��� �����������, ����� �������������� ���� ���� (�� ��������� ��������) ��� ���������� ������ 
void update_menu(uint8_t drawMenuFlag)
{
	if(dispContrReg.page == 0)
		update_clock_menu(drawMenuFlag);
	else if(dispContrReg.page >= 1 && dispContrReg.page <= 4)
		update_resistors_menu(drawMenuFlag);	
	else if(dispContrReg.page == 5)
		update_service_menu(drawMenuFlag);
	else if(dispContrReg.page == 6)
		update_archive_menu(drawMenuFlag);
}

//������ ���� ��� ������� � ������� �����
void draw_fail_menu(void)
{
	CLR_MENU();
    LCD_draw_rectangle(66, 22, 67, 31);

    LCD_setXY(50,9);
    printf("����� ");
	printf("%d", dispContrReg.page);
    
    LCD_setXY(30,18);
    printf("�������������");
    
    LCD_setXY(2,28);
    printf("��������:");

    LCD_setXY(70,28);
    printf("������:");
}

//������ ���� ��� ������� �������������/�������� ������
void draw_resistors_menu(void)
{
	CLR_MENU();
    LCD_draw_rectangle(66, 22, 67, 40);

    LCD_setXY(50,9);
    printf("����� ");
	printf("%d", dispContrReg.page);
    
    LCD_setXY(30,18);
    printf("�������������");
    
    LCD_setXY(2,28);
    printf("��������:");

    LCD_setXY(70,28);
    printf("������:");
}

//����������� ������������� �������� ��� ������� �������������
void get_res_format(float val, char *dst)
{
	if(val > 1000.0f) sprintf(dst, "> 1 ���");

    //���� >100 ��� - ������� ������ ����� �����
    else if( val >= 100.0 ) sprintf(dst, "%3lu ���", (uint32_t)val);

    //���� > 10 ��� � < 100 ��� ������� � ��������� �� �������
    else if ( val >= 10.0 ) sprintf(dst, "%3.1f ���", val);

    //���� < 10 ��� ������� � ��������� �� �����
    else sprintf(dst, "%3.2f ���", val);      
}

#define PLACE_VAL_FOR_PIPE() {LCD_erase_area(2,30,65,40);LCD_setXY(2,40);}		// ��������� ������ �������������/���������� ��� ��������	
#define PLACE_VAL_FOR_SIGNAL()	{LCD_erase_area(68,30,132,40);LCD_setXY(70,40);}	// ��������� ������ �������������/���������� ��� ���������� ��������

void update_resistors_menu(uint8_t drawMenuFlag)
{
	if((dispContrReg.failFlags & (1 << (dispContrReg.page - 1))) > 0)
	{
		if(dispContrReg.pageLock == dispContrReg.page)
		{
			if(drawMenuFlag)
				draw_fail_menu();
	
			LCD_setXY(2,40);
			printf("                     ");
	
			LCD_setXY(25,40);
			if(DisplayBlinkFlag)
				// ����������� �����
				invert_text();
			printf("����� ��������");
			direct_text();
	
			return;
		}
	}

	if(drawMenuFlag)
		draw_resistors_menu();

	// ���� ���� ����� �����
	if(dispContrReg.failFlags & (1 << (dispContrReg.page - 1)))
	{
		// ������� ��������
		PLACE_VAL_FOR_PIPE();
		printf("===");

		PLACE_VAL_FOR_SIGNAL();
		printf("===");

		return;
	}
	
	SENSOR_HANDLE sensHandle1, sensHandle2;
	float val;
	uint8_t bFl1, bFl2; // �����, ����� ��������� ����� �������

	switch(dispContrReg.page)
	{
		case 1: sensHandle1 = OpenSensorProc("S0");
				sensHandle2 = OpenSensorProc("S1");
				bFl1 = 0; bFl2 = 1;
				break;
		case 2:	sensHandle1 = OpenSensorProc("S2");
				sensHandle2 = OpenSensorProc("S3");
				bFl1 = 2; bFl2 = 3;
				break;
		case 3:	sensHandle1 = OpenSensorProc("S4");
				sensHandle2 = OpenSensorProc("S5");
				bFl1 = 4; bFl2 = 5;
				break;
		case 4:	sensHandle1 = OpenSensorProc("S6");
				sensHandle2 = OpenSensorProc("S7");
				bFl1 = 6; bFl2 = 7;
				break;
		default: return;
	}
	
	char text[20 + 1];	

	if(sensHandle1 == 0)
		sprintf(text, "������");
	else
	{	
		ReadSensor(sensHandle1, &val, 1); 
		get_res_format(val, text);

		// ���� ���������� ���� �������	������
		if((dispContrReg.blinkFlags & (1 << bFl1)))
			// � ���� ������� �������
			if(DisplayBlinkFlag)
				// ����������� �����
				invert_text();
	}

	PLACE_VAL_FOR_PIPE();
	printf(text);

	direct_text();
	
	memset(text,0,sizeof(text));
	if(sensHandle2 == 0)
		sprintf(text, "������");
	else
	{
		// ���� ���������� ���� �������	������
		if((dispContrReg.blinkFlags & (1 << bFl2)))
		{
			// � ���� ������� �������
			if(DisplayBlinkFlag)
				// ����������� �����
				invert_text();
		}

		if((dispContrReg.obrivFlags & (1 << bFl2)))
		{
			// ������ ����� ������� �����			
			sprintf(text, "%s", "�����");	
		}
		else
		{
			ReadSensor(sensHandle2, &val, 1); 
			sprintf(text, "%3lu ��", (uint32_t)val);
		}
	}

	PLACE_VAL_FOR_SIGNAL();
	printf(text);
	
	direct_text(); 	
}

//������ ���� ��� ������� ����������
void draw_voltage_menu(void)
{
    CLR_MENU();
    LCD_draw_rectangle(66, 22, 67, 40);

    LCD_setXY(50,9);
    printf("����� ");
	printf("%d", dispContrReg.subService);
    
    LCD_setXY(30,18);
    printf(" ����������  ");
    
    LCD_setXY(2,28);
    printf("��������:");

    LCD_setXY(70,28);
    printf("������:"); 
}

//������ ���� ��� ������� ������� ModBus
void draw_packets_menu(void)
{
    CLR_MENU();

    LCD_setXY(13,9);
    printf("����������: ID ");
    
    LCD_setXY(46,18);
	switch(dispContrReg.subService)
	{
		case 5: printf("����� 1"); break;
		case 6:	printf("����� 2"); break;
		case 7:	printf("����� 3"); break;
		case 8:	printf("����� 4"); break;
	}

	LCD_setXY(2,26);
	printf("����������: ");

	LCD_setXY(2,34);
	printf("�������: ");

	LCD_setXY(16,42);
	printf("�����������: ");
}

void draw_ustavki_menu(void)
{
	CLR_MENU();
    LCD_draw_rectangle(66, 22, 67, 40);

	LCD_setXY(setCentred("�������"), 9);
    printf("�������");

    LCD_setXY(46,18);
	switch(dispContrReg.subService)
	{
		case 9: printf("����� 1"); break;
		case 10:printf("����� 2"); break;
		case 11:printf("����� 3"); break;
		case 12:printf("����� 4"); break;
	}
    
    LCD_setXY(2,28);
    printf("��������:");

    LCD_setXY(70,28);
    printf("������:");	
}

void vGetFreeMem(uint16_t *xMaxBlock, uint16_t *xTotalMem);
float GetUstVal(int ch, int is);
#define PLACE_VAL_FOR_TX_PACKETS() {LCD_erase_area(74,18,132,26);LCD_setXY(74,26);}		// ��������� ������ �������� �������	
#define PLACE_VAL_FOR_RX_PACKETS()	{LCD_erase_area(56,26,132,34);LCD_setXY(56,34);}	// ��������� ������ ������������ �������

void update_service_menu(uint8_t drawMenuFlag)
{
	if(dispContrReg.subService == 0)
	{
		if(drawMenuFlag)
		{
			CLR_MENU();
			LCD_setXY(50,9); 
            printf("������");	
		}	
	}
	else if(dispContrReg.subService >= 1 && dispContrReg.subService <= 4)
	{
		if(drawMenuFlag)
			draw_voltage_menu();
				
		// ���� ���� ����� �����
		if(dispContrReg.failFlags & ( 1 << (dispContrReg.subService - 1)))
		{
			// ������� ��������
			PLACE_VAL_FOR_PIPE();
			printf("===");

			PLACE_VAL_FOR_SIGNAL();
			printf("===");

			return;
		}

		DEVICE_HANDLE hMBS = OpenDevice("MBS");

		switch(dispContrReg.subService)
		{
			case 1: SeekDevice(hMBS, 6, mbIDs.mbIDsBuf[0]); break;
			case 2:	SeekDevice(hMBS, 6, mbIDs.mbIDsBuf[1]); break;
			case 3:	SeekDevice(hMBS, 6, mbIDs.mbIDsBuf[2]); break;
			case 4:	SeekDevice(hMBS, 6, mbIDs.mbIDsBuf[3]); break;
		}
		float val;
		ReadDevice(hMBS, &val, 1);

		PLACE_VAL_FOR_PIPE();
		printf("%5.4f �", val);

		switch(dispContrReg.subService)
		{
			case 1: SeekDevice(hMBS, 4, mbIDs.mbIDsBuf[0]); break;
			case 2:	SeekDevice(hMBS, 4, mbIDs.mbIDsBuf[1]); break;
			case 3:	SeekDevice(hMBS, 4, mbIDs.mbIDsBuf[2]); break;
			case 4:	SeekDevice(hMBS, 4, mbIDs.mbIDsBuf[3]); break;
		}
		ReadDevice(hMBS, &val, 1);

		PLACE_VAL_FOR_SIGNAL();
		printf("%5.4f �", val);

		CloseDevice(hMBS);
	}
	else if(dispContrReg.subService >= 5 && dispContrReg.subService <= 8)
	{
		if(drawMenuFlag)
		{

			draw_packets_menu();

			LCD_setXY(103,9);
			printf("   ");
			LCD_setXY(103,9);

			uint8_t id;
			switch(dispContrReg.subService)
			{
				case 5: id = mbIDs.mbIDsBuf[0]; break;
				case 6:	id = mbIDs.mbIDsBuf[1]; break;
				case 7:	id = mbIDs.mbIDsBuf[2]; break;
				case 8:	id = mbIDs.mbIDsBuf[3]; break;
			}

			printf("%d", id);
		}
		
		int rx_count, tx_count;
	  	DEVICE_HANDLE hMBS = OpenDevice("MBS");

		switch(dispContrReg.subService)
		{
			case 5: rx_count = tx_count = (int)mbIDs.mbIDsBuf[0]; break;
			case 6:	rx_count = tx_count = (int)mbIDs.mbIDsBuf[1]; break;
			case 7:	rx_count = tx_count = (int)mbIDs.mbIDsBuf[2]; break;
			case 8:	rx_count = tx_count = (int)mbIDs.mbIDsBuf[3]; break;
		}

		DeviceIOCtl(hMBS, GET_TX_PACKETS_COUNT, (uint8_t *)&tx_count);	  
		DeviceIOCtl(hMBS, GET_RX_PACKETS_COUNT, (uint8_t *)&rx_count);
		
		CloseDevice(hMBS);
		
		PLACE_VAL_FOR_TX_PACKETS();
		printf("%d", tx_count);
		PLACE_VAL_FOR_RX_PACKETS();
		printf("%d", rx_count);	
		
		LCD_setXY(94,42);
		SENSOR_HANDLE sens; 
		switch(dispContrReg.subService)
		{
			case 5: sens = OpenSensorProc("S8");
					break;
			case 6:	sens = OpenSensorProc("S9");
					break;
			case 7:	sens = OpenSensorProc("S10");
					break;
			case 8:	sens = OpenSensorProc("S11");
					break;
			default: return;
		}
		
		if(sens == 0)	
		{
			printf("������");	
		}
		else
		{
			float val;
			ReadSensor(sens, &val, 1);
			if(val == 200.0f)
				printf("�����");	
			else
				printf("%2.1f", val / 100.0f);
		}
	}
	else if(dispContrReg.subService >= 9 && dispContrReg.subService <= 12)
	{
		if(drawMenuFlag)
			draw_ustavki_menu();

		float ustI, ustS;
		switch(dispContrReg.subService)
		{
			case 9: ustI = GetUstVal(0,0); ustS = GetUstVal(0,1); break;
			case 10:ustI = GetUstVal(1,0); ustS = GetUstVal(1,1); break;
			case 11:ustI = GetUstVal(2,0); ustS = GetUstVal(2,1); break;
			case 12:ustI = GetUstVal(3,0); ustS = GetUstVal(3,1); break;
		}

		PLACE_VAL_FOR_PIPE();
		printf("%0.1f ���", ustI);

		PLACE_VAL_FOR_SIGNAL();
		printf("%0.1f ��", ustS);
	}
	else if(dispContrReg.subService == 13)
	{
		if(drawMenuFlag)
		{
			CLR_MENU();
			LCD_setXY(2,10);
			printf("������ ��: %s", (char *)VERS_ADDR);

			LCD_setXY(2,20);
			printf("���������: %s", (char *)BOOT_VERS_ADDR);

			uint8_t id_boof[40];
			LCD_setXY(2,30);
			printf("ID:");

			GetProcessorID((char *)id_boof);
			int i = 0;
			for(i = 32; i > 15; i--)
				id_boof[i+1] = id_boof[i];
			id_boof[16] = '\n';


			LCD_setXY(20,30);
			printf("%s", (char *)id_boof);
								
		}
	}
	else if(dispContrReg.subService == 14)
	{
		if(drawMenuFlag)
		{
			uint16_t totalMem, maxBlock;
			vGetFreeMem(&maxBlock, &totalMem);
			
			CLR_MENU();

			LCD_setXY(2,21);
			printf("����� - %d ����", totalMem);//printf("����. ���� - %d ����", maxBlock);
			//LCD_setXY(2,30);
			//printf("����� - %d ����", totalMem);		
		}		
	}
	else if(dispContrReg.subService == 15)
	{
		char *pIP;
		if(drawMenuFlag)
		{
			CLR_MENU();
			LCD_setXY(1,10);
			printf("���:");

			LCD_setXY(1,25);
			printf("IP:");

			LCD_setXY(1,40);
			printf("������:");
		}

		LCD_erase_area(45,32,100,40);
		LCD_setXY(45,40);
		printf("%d", GetSigLevel());

		LCD_erase_area(30,17,132,25);
		LCD_setXY(30,25);
		pIP = GetOurIPStr();
		if(pIP != 0)
			printf(pIP);

		LCD_erase_area(30,2,132,10);
		LCD_setXY(30  ,10);
		printf((char *)GetSimNum());



		/*LCD_setXY(33,30);
		switch(ePPPThrCtl)
		{
			case CONNECTING: printf("�����������"); break; 
			case CONNECTED:	 printf("���������� "); break;
			case DISCONNECT: printf(" ��������� "); break;
		}*/		
	}
	else if(dispContrReg.subService == SERVICE_MENU_DEPTH)
	{
		if(drawMenuFlag)
		{
			CLR_MENU();

			LCD_setXY(40,9);
    		printf("���������");
    		LCD_setXY(55,18);
    		printf("����");
/*			  LCD_setXY(14,18);
			  printf("����������� ������");
			  LCD_setXY(55,27);
			  printf("���");
			  LCD_setXY(22,36);
			  printf("��������� �����");*/		
		}	
	}
}

static char logRecordBuf[LOG_RECORD_LEN + 1];

void update_archive_menu(uint8_t drawMenuFlag)
{
	if(dispContrReg.subArchive == 0)
	{
		if(drawMenuFlag)
		{
			CLR_MENU();
			LCD_setXY(50,9); 
            printf("�����");
			GoToTheNewestRecord();	
		}	
	}
	else
	{
		if(drawMenuFlag)
		{
		    CLR_MENU();
			memset(logRecordBuf, 0, sizeof(logRecordBuf));

			uint16_t numberOfRecord, currPos;
			GetArchInfo(&numberOfRecord, &currPos);
			
			if(numberOfRecord == 0)
			{
				LCD_setXY(2,9);
				printf("������ %d/%d",0,0);
				LCD_setXY(2,19);
				printf("����� ����");
				return;
					
			}

			GetLogRecord(logRecordBuf);

			LCD_setXY(2,9);
			printf("������ %d/%d",currPos+1, numberOfRecord);
			printf(logRecordBuf);	
		}	
	}
}


/********************************��������� ������ �������***********************************************/

void draw_pow_indy(uint8_t sig)
{
	int i = 0;
	int count = sig/4;

	LCD_erase_area(111,1,133,8);

	 LCD_draw_line(111,1,117,1);
	 LCD_draw_line(114,1,114,8);
	 LCD_draw_line(111,1,114,4);
	 LCD_draw_line(114,4,117,1);

	 for(i = 0; i < count; i++)
	 {
	 	 LCD_draw_line(117 + i*2, 8 - i, 117 + i*2, 8);
	 }
}

/*******************************************************************************************************/



void update_clock_menu(uint8_t drawMenuFlag)
{
	if(drawMenuFlag)
		CLR_MENU();

	//--------------------------------------------------//
	//-----------������ ���������-�����������-----------//
	//--------------------------------------------------//

	//���� ���� ������� = 1 ������ ��������� �� �����
    if(DisplayBlinkFlag == 0xff)
    {
        LCD_draw_image(56,16,4,4,bar,sizeof(bar));
        LCD_draw_image(56,30,4,4,bar,sizeof(bar));
    }
    else 
    {
        //����� ������� ���
        LCD_erase_area(56,16,59,34);
    }

	//--------------------------------------------------------------//
	//-----------������ ������������� ��� ��������� �����-----------//
	//--------------------------------------------------------------//

    //���� � ������ ��������� ������� � ���������� ���� - ������ ��� ������ �����
    if(dispContrReg.timeSetupTarget == SETUP_HOUR) 
		//LCD_draw_line(8,40,62,40);
		LCD_draw_line(8,40,62,40);
    //����� ������� �����
    else 
		LCD_erase_area(8,39,62,41);

    //���� � ������ ��������� ������� � ���������� ������ - ������ ��� �������� �����
    if(dispContrReg.timeSetupTarget == SETUP_MIN) 
		LCD_draw_line(71,40,125,40);
    //����� ������� �����
    else 
		LCD_erase_area(71,39,125,41);

	//---------------------------------//
	//-----------������ ����-----------//
	//---------------------------------//

	CLOCKData time;
	DEVICE_HANDLE hClock;
	hClock = OpenDevice("Clock");
    SeekDevice(hClock, 0, 0);
    ReadDevice(hClock, (void *)&time, sizeof(CLOCKData));
	CloseDevice(hClock);

	//----------------------------------//
    //-----------������� ����-----------//
	//----------------------------------//

	char date_now[13];

    sprintf(date_now, "%02d", time.Date);
    sprintf(&date_now[4], "%02d", time.Month);
    sprintf(&date_now[8], "%4d", time.Year + 2000);

    LCD_setXY(2,9); 
    //���� � ������ ��������� ������� � ���������� ����� - ����������� �����
    if( dispContrReg.timeSetupTarget == SETUP_DATE ) 
		invert_text();
    printf(date_now);
    direct_text();
    printf(".");

    //���� � ������ ��������� ������� � ���������� ����� - ����������� �����
    if( dispContrReg.timeSetupTarget == SETUP_MONTH ) 
		invert_text();
    printf(&date_now[4]);
    direct_text();
    printf(".");

    //���� � ������ ��������� ������� � ���������� ��� - ����������� ���
    if( dispContrReg.timeSetupTarget == SETUP_YEAR ) 
		invert_text();
    printf(&date_now[8]);
    direct_text();

	//-----------------------------------//
	//-----------������� �����-----------//
	//-----------------------------------//

	char time_now[6];
    sprintf(time_now, "%2d", time.Hour);
    sprintf(&time_now[2], "%02d", time.Min);
	                
    //���������� ����������� ����� � �������
    //������� �����, ���� ��� ����������
    if(strcmp(dispContrReg.timePrev, time_now) != 0)// return;
	{
		int i = 0;
	    for(; i < 4; i++)
	    {
			if(time_now[i] == 0 || time_now[i] == ' ')
			{		
				LCD_erase_area(clock_place[i].X,clock_place[i].Y,clock_place[i].X+CLOCK_SIZE_X,clock_place[i].Y+CLOCK_SIZE_Y);	
				continue;
			}
	
			char digBuf[2] = {time_now[i], 0};
			int dig = atoi(digBuf);
	
			LCD_draw_image(clock_place[i].X,clock_place[i].Y,CLOCK_SIZE_X,CLOCK_SIZE_Y,&clock_big_digits[dig][0],sizeof(clock_big_digits)/10);	 
	    }
	}

	// ������� ���������� � ��������� ����
	uint8_t modem_st = Modem_state_get();
	if(modem_st == GPRS_ON || modem_st == ON_LINE || modem_st == OFF_LINE || modem_st == PPP_ON)
	{
		LCD_setXY(125, 20);
		printf("G");

		LCD_setXY(125, 30);
		if(modem_st == ON_LINE ||  modem_st == PPP_ON)
			printf("I");
		else
			printf(" ");
	}
	else
	{
		LCD_setXY(125, 20);
		printf(" ");
		LCD_setXY(125, 30);
		printf(" ");
	}
	
	LCD_erase_area(65,1,111,9);
	LCD_setXY(65, 9);
	printf((char *)GetOpName());  

	draw_pow_indy(GetSigLevel());

	LCD_setXY(125, 40);
	if(Sim_state_get() == 0)
		printf("S");
	else  printf(" ");
		


	/*	LCD_setXY(125, 20);
		printf("G");

		LCD_setXY(125, 30);
		printf("I");  */


	//���������  ������� �����
    strncpy(dispContrReg.timePrev, time_now, 6);
}

/************************************************/
/************************************************/
/***************��������� �������****************/
/************************************************/
/************************************************/

void lock_page(uint8_t page)
{
	// �� ����� ������� �� �������� ������� ���
	if(page > mbIDs.mbIDsCnt)		
		return;

	// ���� ��� ���� ������������ - �����
	if(dispContrReg.pageLock != 0xff)
		return;

	// ���� ���������� ������ ������������� - �����			   
	if(dispContrReg.page == 8)
		return;

	toggleIcons(dispContrReg.page, page);

	dispContrReg.page = dispContrReg.pageLock = page;

	dispContrReg.subService = 0;
	dispContrReg.subArchive = 0;
	dispContrReg.timeSetupFlag = 0;
	dispContrReg.timeSetupTarget = SETUP_NOTH;

	update_menu(1);
}

// ����� �������
void set_blink_flag(uint8_t blinkFlag)
{
	dispContrReg.blinkFlags |= blinkFlag;
}

void clr_blink_flag(uint8_t blinkFlag)
{
	dispContrReg.blinkFlags &= ~blinkFlag;
}

// ����� ������� ���������� ��������
void set_obriv_flag(uint8_t obrivFlag)
{
	dispContrReg.obrivFlags |= obrivFlag;
}

void clr_obriv_flag(uint8_t obrivFlag)
{
	dispContrReg.obrivFlags &= ~obrivFlag;
}

// ����� ������� �����
void set_fail_page(uint8_t failFlag)
{
	dispContrReg.failFlags |= failFlag;	
}

void clr_fail_page(uint8_t failFlag)
{
	dispContrReg.failFlags &= ~failFlag;	
}

