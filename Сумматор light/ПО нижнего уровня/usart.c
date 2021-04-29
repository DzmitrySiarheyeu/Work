#include "usart.h"
#include "mbfanction0.h"
#include "omnicomm.h"

#define PORT_DE PORTD
#define DDR_DE DDRD
#define PIN_DE 2

__ALSO_VAR__ volatile byte timer0;
__ALSO_VAR__ volatile byte *pUDR0;
__ALSO_VAR__ volatile byte * volatile pBuf0;
byte DataBuffer0[ MBCLIENT_MAX_LENGHT ];
__ALSO_VAR__ volatile byte BufferSize0 = 0;

__ALSO_VAR__ volatile byte state0 = State_INITIAL;

void SET_DE485_0(void){ cbi(UCSR0B, RXEN0); sbi(PORT_DE, PIN_DE); }
void CLR_DE485_0(void){ cbi(PORT_DE, PIN_DE); sbi(UCSR0B, RXEN0); }


//������������� UART
void InitUART(void)
{
	//unsigned long BRATE;

	// ��������� UART
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
	pUDR0 = &UDR0;
	// ��������� ����� 
	cbi(PORT_DE, PIN_DE);
	DDR_DE |= (1 << PIN_DE);
	// ��������� �������� ��������
	//eeprom_read_block(&BRATE, (void*)&EEP_Boudrate0, sizeof(BRATE));
	MBClientSetBaudrate0((byte)(F_CPU/16/19200 - 1));
	//��� ���������� ��������
	MBClientSetHostID0(99);//eeprom_read_byte((void*)&EEP_ID0));

	//���� ����� ������ ������ ��� ������ ����
	PORTD=1<<0;

	sbi(TIMSK0, TOIE0);	
}

SIGNAL(SIG_OVERFLOW0)
{
	MBClientIncTimer0();
	omnicomm_timer++;
}

__MBCL_RECEIVE__ byte UartRecivePacket(void)
{
	static byte stage = 0;
		
	switch(stage)
	{
		case 0:
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//  ���� ��� ������������ ���������� ��� ������������� ������ Omnicomm � Modbus
// ��� ���� MODBUS ����� ������������� �� ��������� !!!! ��������� ��� ����� ������� - ����
// ���������� �� ����� ��� ���������. ��� ���������� ������ MODBUS ������ ����� ����� �� ����������

		/*	BufferSize0 = 0;			//������� ������
			state0 = State_INITIAL;	//!!!!!!!!!!!!!!!!!!!!!
			stage++;		
			break;
		case 1:
			if(timer0 > T15_Ticks)
			{
				stage++;
				CLR_DE485_0();
			}
			break;
		case 2:
			*/
			if(state0 == State_INITIAL)
			{
				if(timer0 > T35_Ticks)
				{
					stage++;
					BufferSize0 = 0;
					state0 = State_IDLE; //���������� ��������� �������������
				}
			}
			else
			{
				state0 = State_IDLE; //���������� ��������� �������������
				BufferSize0 = 0;
				stage++;
			}
			break;
		case 1:
			if(state0 == State_RECEIVING || state0 == State_WAITING)
			{
				stage++;
			}
			break;
		case 2:
			if(timer0 > T15_Ticks)
			{
				stage++;
				state0=State_WAITING;
			}
		case 3:
			if(timer0 > T35_Ticks)
			{
				stage = 0;
				return BufferSize0;
			}	
			break;

	}
	return 0;
}

//��������� ���������� ����� ������ � UART
SIGNAL(USART_RX_vect)
{

	byte temp = *pUDR0;
	//���� ��������� ����� 1.5 � 3.5 ���������� �����������
	if (state0 == State_WAITING)
	{
		//�������� ����� � �����
		BufferSize0 = 0;
		return;
	}
	//���� �� � ��������� �������������
	if (state0 != State_INITIAL)
	{
		if (BufferSize0 == 0) state0=State_RECEIVING;//������� � ��������� �����
		DataBuffer0[ BufferSize0++ ] = temp; //������� ���������� ������ � �����
		if(BufferSize0 >= USART_PACKET_MAX_LENGHT)//���� ������ ������ �������� ������������ ��������
			state0 = State_WAITING;//������� � ��������� ��������
	}
	StartModbusTimer0();//������ �������
}

SIGNAL(USART_TX_vect)
{
	cbi(UCSR0B, TXCIE0);
	CLR_DE485_0();
	state0 = State_IDLE; //���������� ��������� �������������	
	StartModbusTimer0();
}

SIGNAL(USART_UDRE_vect)
{

	*pUDR0 = *pBuf0; /* start transmition */
	pBuf0++;
	BufferSize0--;
	if(!BufferSize0)
	{
		cbi(UCR0, UDRIE0); /* disable UDRE interrupt */
		sbi(UCSR0B,TXCIE0);
	}
}
