#pragma once

#include "util.h"

#define MAX_DOT_COUNT	4
#define LOG_PAR_COUNT	9
#define DOT_PAR_COUNT	30


// 0x2100 - ��������� ��������� 30 �����
// 0x2200 - ���� ���������
// 0x2212 - ��������������� �������� ��������� �������� ������ ������� ������� �� ������� ������
// C 22 ������� 2010 ������ ������ ���������
// 0xHHTL
// HH - ������� ����� ������
// T - ��� ���������� 0 - ���������� ���������� �������; 1 - ��������; 2 - ���������� ���������� �����
// L - ������� ����� ������
#define VERSION_PO		0x2212

#define FREQUENCY_INPUT	1
//#define R_OUT_ENABLE	1

#ifndef FREQUENCY_INPUT
#define ANALOG_INPUT 1
#endif

#define __ALSO_FUNC__		__attribute__ ((section (".also_var")))

typedef struct tagLOG_param
{
	double Vtable[ LOG_PAR_COUNT ];
	double Rtable[ LOG_PAR_COUNT ];
}LOG_PARAM;	//4*8*2=64

typedef struct tagDOT_table
{
	double Ftable[ DOT_PAR_COUNT ];
	double Vtable[ DOT_PAR_COUNT ];
}DOT_TABLE;

typedef struct tagDOT_param
{
	double frequency;		// ������� ���������� ���� �� ������
						// !!!!!!!!! ��� ������� ��� �� ������������
	double Ftable[10];	// ������� ������������� �� ���� ��������
	double Vtable[10];
	unsigned short N; 	// ����� ����� ������������� ������
}DOT_PARAM;	//86 bytes

typedef struct tagDOT_info
{
	unsigned short N; // ����� ��� �������������� �����������. �� 2��.
	unsigned short fl_freq; //���� ������� ���������� ���
	DOT_PARAM DOT[ MAX_DOT_COUNT ];
}DOT_INFO;	//86*2 + 4 =	176

typedef struct tagRegisterStruct
{
	unsigned short U_type;		// 0 - ����� ���������� �� 4,5 �, 1 - ����� ���������� �� 12�, 2 - ����� ���������� �� 20�.
	unsigned short R_type;		// 0 - ����� 0-90 ��, 1 - ����� 0-350 ��, 2 - ����� 0 - 900 ��
	unsigned short Fl_logometr;	// ���������� ����� ��������� �������� ��� ���.
	unsigned short Configured;	// 0 - �� �������� 1 - ��������
	double VolumePercent;
	// 6 & 7 register
	double Reserv;			// ����� ������������ ������ "������" � % �� ������ ������. �������� �� 0 �� 100.
	// 8 & 9 register
	double U_max;	// ����� ����� � %
	DOT_INFO dot_info;
	LOG_PARAM log_param;
}REGISTER_STRUCT;	//176 + 20 + 64 = 260

typedef union tagRegisterFile
{
	REGISTER_STRUCT mb_struct;
	unsigned short mb_array[sizeof(REGISTER_STRUCT)/2];
}REGISTER_FILE;

double get_double_P(PGM_VOID_P p);

extern __ATTR_EEPROM__ REGISTER_FILE EepromRegFile;
extern REGISTER_FILE RegFile;

extern __ATTR_EEPROM__ byte  EEP_ID0 ;	
extern __ATTR_EEPROM__ unsigned long   EEP_Boudrate0 ;
extern DOT_TABLE ProgramDotTable[MAX_DOT_COUNT];
