#pragma once


// ����� ������� �� ����
#define TABLE_START_ADDRESS		((uint8_t *)0x00007000)
#define		FL_ADDR_START			(0x3B00UL)

#define MAX_DOT_COUNT	4
#define LOG_PAR_COUNT	9
#define DOT_PAR_COUNT	30

#ifndef true
#define true 1
#endif 

#ifndef false
#define false 0
#endif

typedef enum {
	DevFreq,
	DevAnalog
}DEVICE_TYPE;

#define		FERQ_DEVICE_PINS		((1 << 4) | (1 << 5) | (1 << 6))
#define		ANALOG_DEVICE_PINS		((1 << 5) | (1 << 6))


// 0x2100 - ��������� ��������� 30 �����
// 0x2200 - ���� ���������
// 0x2212 - ��������������� �������� ��������� �������� ������ ������� ������� �� ������� ������
// 0x22X3 - ������ ������ �� NXP
// C 22 ������� 2010 ������ ������ ���������
// 0xHHTL
// HH - ������� ����� ������
// T - ��� ���������� 0 - ���������� ���������� �������; 1 - ��������; 2 - ���������� ���������� �����
// L - ������� ����� ������
#define VERSION_PO		0x2216



#define		LED_INIT		GPIOSetDir(0, 3, 1);
#define     SET_LED_STATE(s)			(GPIOSetValue(0, 3, s > 0 ? 0 : 1))
#define     LED_ON              		GPIOSetValue(0, 3, 0);
#define     LED_OFF              		GPIOSetValue(0, 3, 1);



typedef struct __attribute__ (( __packed__ )) tagLOG_param
{
	float Vtable[ LOG_PAR_COUNT ];
	float Rtable[ LOG_PAR_COUNT ];
}LOG_PARAM;	//4*8*2=64

typedef struct __attribute__ (( __packed__ )) tagDOT_table
{
	float Ftable[ DOT_PAR_COUNT ];
	float Vtable[ DOT_PAR_COUNT ];
}DOT_TABLE;

typedef struct __attribute__ (( __packed__ )) tagDOT_param
{
	float frequency;		// ������� ���������� ���� �� ������
						// !!!!!!!!! ��� ������� ��� �� ������������
	float Ftable[10];	// ������� ������������� �� ���� ��������
	float Vtable[10];
	unsigned short N; 	// ����� ����� ������������� ������
}DOT_PARAM;	//86 bytes

typedef struct __attribute__ (( __packed__ )) tagDOT_info
{
	unsigned short N; // ����� ��� �������������� �����������. �� 2��.
	unsigned short fl_freq; //���� ������� ���������� ���
	DOT_PARAM DOT[ MAX_DOT_COUNT ];
}DOT_INFO;	//86*2 + 4 =	176

typedef struct __attribute__ (( __packed__ )) tagRegisterStruct
{
	unsigned short U_type;		// 0 - ����� ���������� �� 4,5 �, 1 - ����� ���������� �� 12�, 2 - ����� ���������� �� 20�.
	unsigned short R_type;		// 0 - ����� 0-90 ��, 1 - ����� 0-350 ��, 2 - ����� 0 - 900 ��
	unsigned short Fl_logometr;	// ���������� ����� ��������� �������� ��� ���.
	unsigned short Configured;	// 0 - �� �������� 1 - ��������
	float VolumePercent;
	// 6 & 7 register
	float Reserv;			// ����� ������������ ������ "������" � % �� ������ ������. �������� �� 0 �� 100.
	// 8 & 9 register
	float U_max;	// ����� ����� � %
	DOT_INFO dot_info;
	LOG_PARAM log_param;

}REGISTER_STRUCT;	//176 + 20 + 64 = 260

typedef struct __attribute__ (( __packed__ )) tagOmniParam
{
	uint32_t 		e2_id;
	uint16_t		e2_n;
	uint16_t		e2_f_curr;
	uint8_t			e2_net_mode;
	uint8_t			e2_t;
	uint8_t			e2_period_auto;
	uint8_t			e2_report_on;
	uint8_t			e2_old_contin;
}OMNI_PARAM;


typedef struct __attribute__ (( __packed__ )) tagRegisterStructFlash
{
	REGISTER_STRUCT mb_struct;
	DOT_TABLE 		ProgramDotTable[ MAX_DOT_COUNT ];
	OMNI_PARAM		omn_param;
	uint32_t 		boudrate;
	uint16_t 		ID;
	uint16_t   		crc;
}FL_REGISTER_STRUCT;

typedef union tagRegisterFile
{
	REGISTER_STRUCT mb_struct;
	unsigned short mb_array[sizeof(REGISTER_STRUCT)/2];
}REGISTER_FILE;


float get_double_P(void *p);

extern FL_REGISTER_STRUCT *pFlashRegFile;
extern REGISTER_FILE RegFile;
extern OMNI_PARAM  omn_param;
extern DEVICE_TYPE DevType;
extern DOT_TABLE *ProgramDotTable;

inline float Reverse4(float in);
void mem_reverce(void *data, unsigned char count);
inline unsigned short Reverse2(unsigned short in);
FL_REGISTER_STRUCT * GetFlashActualData(void);
void WriteFlash(FL_REGISTER_STRUCT *flash);
uint16_t CRC16 (uint8_t * puchMsg, uint16_t usDataLen, uint16_t prevCRC);
