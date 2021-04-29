// ������: 		�������� light 2.0
// �����������:	��� ����� ����� 11.2 ����� ���������� �������� ������������
// ����������:	���� �.�.
// ����:		24.05.2011


#ifndef _SUMMATOR_H_
#define _SUMMATOR_H_


#define		STATE_NUM			3   //  ���������� ������� ���
#define		LED_PIN_STATE_NUM   4   //  ������������ ���������� ��������� ��������� ���� ��� ���������� � ��� 1�

/**************��������� ��� ������������� *********************/

#define		TIMER_TICK_H_20		114   //����� ����� ��� ��������� �������� ������ �� ���� (��� 20%)
#define		TIMER_TICK_L_20		28  //����� ����� ��� ��������� ������� ������ �� ���� (��� 20%)

#define		TIMER_TICK_H_30		92   //����� ����� ��� ��������� �������� ������ �� ���� (��� 30%)
#define		TIMER_TICK_L_30		50  //����� ����� ��� ��������� ������� ������ �� ���� (��� 30%)

#define		TIMER_TICK_H_0		255   //����� ����� ��� ��������� �������� ������ �� ���� (��� 0%)
#define		TIMER_TICK_L_0		1  //����� ����� ��� ��������� ������� ������ �� ���� (��� 0%)

#define		TIMER_TICK_20		(TIMER_TICK_H_20 + TIMER_TICK_L_20)  //����� ����� ������� (������ 10� ��� 20%)	
#define		TIMER_TICK_30		(TIMER_TICK_H_30 + TIMER_TICK_L_30)  //����� ����� ������� (������ 10� ��� 30%)	
#define		TIMER_TICK_0		(TIMER_TICK_H_0 + TIMER_TICK_L_0)  //����� ����� ������� (������ 10� ��� 0%)

               /*��� ������� 1 ��� � �������*/
#define		TIMER_TICK_1pS_ST1		2880 // ����� 200 ��
#define		TIMER_TICK_1pS_ST2		11520 // �� ����� 800 ��

               /*��� ������� 2 ���� � �������*/
#define		TIMER_TICK_2pS_ST1		2880 // ����� 200 ��
#define		TIMER_TICK_2pS_ST2		1440 // �� ����� 100 ��
#define		TIMER_TICK_2pS_ST3		2880 // ����� 200 ��
#define		TIMER_TICK_2pS_ST4		7200 // �� ����� 500 ��	

               /*��� ������� 3 ���� � �������*/
#define		TIMER_TICK_3pS_ST1		7200 // ����� 500 ��
#define		TIMER_TICK_3pS_ST2		7200 // �� ����� 500 ��

				/*��������� ������*/
#define		KEY_PRESSED				0   // ������ ������
#define		KEY_PRESSING			1   // ������ ���������� (���� ��������� ����� ��� ���������� ��������)
#define		KEY_FREE				2   // ������ ��������
#define		KEY_FREEING				3   // ������ ����������� (���� ��������� ����� ��� ���������� ��������)
#define		KEY_WAIT_FREE			4   // ������ ���� ����������


#define		TIME_BOUNCE_DOWN		10  //����� � �������� �� ��� ���������� ��������



typedef struct tagPWMInfo
{
	unsigned char	count[2];  //  ����� �� ������� ������ ������� ������ ��� ������������ �������
}PWM_INFO;


typedef struct tagLEDInfo
{
	unsigned short	count[LED_PIN_STATE_NUM];  //  ����� �� ������� ������ ������� ������ ��� ������� ������� ������
}LED_INFO;


typedef struct tagSumInfo
{
	unsigned char	state;    		//  ������� �����
	unsigned char	line_state;		//  ������� ��������� ��������� ����
	PWM_INFO		pwm[STATE_NUM]; //  ��� ������ ���
	LED_INFO		led[STATE_NUM]; //
	unsigned char	led_state_count;
}SUM_INFO;


typedef struct tagKeyInfo
{
	unsigned char	state;			//  ��������� ������
	unsigned short	count;			//  ������� ������� � ����������� ���������
}KEY_INFO;


#endif
