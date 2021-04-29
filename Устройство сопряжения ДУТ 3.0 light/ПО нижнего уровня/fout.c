#include "fout.h"
#include <math.h>
#include "potentiometer.h"
//----------------- R OUT TIMER0-----------


// �������� ������� �� 2 ������ �������-������� ��������� �  ����� ��������� 
// (������� -  �������, ��� ����� ����� � �������)
	
volatile	unsigned char big=0;				// ������� ��������
volatile	unsigned char big_tmp=0;			// ������� �������� 
volatile	unsigned char count_big=0;			// ���������� ������� ��������
volatile	unsigned char count_big_tmp=0;		// ���� �����, ���������������� � ����������
	
volatile	unsigned char hvost;				// ��������� ��������
volatile	unsigned char hvost_tmp;			// ��������� ��������

void FOutInit(void)
{
	cbi(PRR,  PRTIM2);      //On TIMER0
	sbi(TCCR2A, WGM21);
	OCR2A=00;
	TCCR2B = FOUT_TIMER_CLOCK_DIVIRDER;
	TIMSK2|=(1<<OCIE2A); // ��� ���������� ������
}

// ������ ��������
void set_Freq(double freq){
	
	unsigned short tmp_hvost;
	unsigned short counts;
	unsigned char tmp_count_big;
	unsigned char tmp_big;

	counts = ((OUT_FRTBL_TO_FR/freq) - 1.0);

	tmp_count_big = (counts/255)+1;	
	
	tmp_big=counts/tmp_count_big;

	tmp_count_big--;

	tmp_hvost=counts-(unsigned short)(tmp_big*(unsigned short)tmp_count_big);

	while (tmp_hvost > 0xFF){
		tmp_hvost-=tmp_count_big;
		tmp_big++;
	}

	tmp_big--;

	cli();
	big = tmp_big;
	count_big = tmp_count_big;	
	hvost=tmp_hvost;
	sei();
}


ISR(TIMER2_COMPA_vect){

	if 	(count_big_tmp){
		OCR2A=big_tmp;
		count_big_tmp--;
	}
	else 
	{
		FREQ_OUT_PORT^=(1<<FREQ_OUT_PIN);	// �������� ����
		OCR2A=hvost_tmp;

		count_big_tmp=count_big;
		big_tmp=big;
		hvost_tmp=hvost;
	}
}
