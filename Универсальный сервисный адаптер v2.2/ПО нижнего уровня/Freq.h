#ifndef __DOT_H__
#define __DOT_H__


//********************************
// ��������� ������� ������ ���������� �������
#define IDLE_FREQ_MEASURE		0
#define START_FREQ_MEASURE		1    	// ������ ��������� �������
#define FREQ_MEASURE			2		// �������������� ��������� �������
#define FREQ_REDY_MEASURE		3		// ��������� ��������� �����
#define N_Period_Measure		20		// ����������� ������������ ������� � �� ����� ������� ������������ ��������� �������
#define N2_Period_Measure       (2*N_Period_Measure)
#define Div_T1 					1
#define Fclktimer (F_CPU/Div_T1)
//********************************


typedef struct tagFREQ_Registr	
{
	volatile unsigned int 	Count_Timer;
	volatile unsigned int 	Count_Capture;
	volatile unsigned char 	State;
	volatile unsigned char 	PreState;
	volatile unsigned char 	CapFlag;
	volatile unsigned short	First_Capture;
	volatile unsigned short	Last_Capture;
	volatile long	Tick_Counter;
	volatile unsigned char in;
	volatile float buf[10];
	volatile float Alpha;
}FREQ_REG;


extern void FREQProcess(void);
#endif
