/*include*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <avr\wdt.h>
#include <avr/pgmspace.h>
#include "mbfanction0.h"
#include "mbclient.h"
#include "usb_adapt.h"


///////////////////////// ��� ������� ����� //////////////////
MODBUS_INT CPT0[9];

volatile uint8_t timer0;
volatile unsigned char  *pUDR0;
volatile uint8_t * volatile pBuf0;
uint8_t DataBuffer0[ MBCLIENT_MAX_LENGHT ];
volatile uint8_t BufferSize0 = 0;
uint8_t hostID0 = 99;
volatile uint8_t state0 = State_INITIAL;

static inline void SET_DE485_0(void){ cbi(UCSR0B, RXEN0); /*sbi(DE485_0_PORT, DE485_0_PIN);*/ }
static inline void CLR_DE485_0(void){ /*cbi(DE485_0_PORT, DE485_0_PIN);*/ sbi(UCSR0B, RXEN0); }


// ������� ��������� �� ������
void SendErrorResponse(uint8_t ExceptionCode)
{
	CPT0[SLAVE_EXSEPT_ERR_CNTR].i++;		//��������� �����. �������
	DataBuffer0[1] |= MOD_ERROR_CODE_FLAG;	//� ������ �-��� ��������� ��������� �������� ������ (0x80)
	DataBuffer0[2] = ExceptionCode;			//�������� ��� ����������
	MBClientSend(2);			//�������

}

// ��������� �-��� 0x08 - �����������
void DiagnosticModbus0(uint8_t *buffer,uint8_t Size)
{
	uint8_t i;
	//���� ��� ����-��� ����������� ������-�� ��������
	if ((buffer[2] >= BUS_MESSAGE_COUNT) && (buffer[2] <= BUS_CHARRACTER_OVERRUN_COUNT))
	{
		buffer[3] = CPT0[buffer[2] - 0x0A].b[1];	//��������� � ���������� ���� �������
		buffer[4] = CPT0[buffer[2] - 0x0A].b[0];
		Size = 5;								//��������� ������� ���������
		MBClientSend(Size);				//�������
		return;
	}
	switch(buffer[2])
	{
		case CLEAR_COUNTERS:					//����-��� ������� ���������
			for(i = 1;i < 9;i++)				//���������� �������
				CPT0[i].i = 0;
			break;
		case RETURN_QUERY_DATA:					//����-��� ���
			break;								//������ �� ������
		default:										//����-��� ���������� 
			SendErrorResponse(ILLEGAL_FUNCTION);	//������� ��������������� ����������
			return;
	}
	MBClientSend(Size);//�������
}

uint8_t CheckCRC0(void)
{
	//���������, ��������� �� ��������� CRC � �����������
	if (CRC16(DataBuffer0, BufferSize0 - 2, 0xFFFF) != *(unsigned int *)(DataBuffer0 + BufferSize0 - 2))
		return CRC_ERROR; //���� ��� - ������� ������
	return 0;
}

//int SIZE = 0;
void MBClientSend(unsigned char size)
{
	unsigned int crc;

	if(size > 252)
	{
		CPT0[SLAVE_EXSEPT_ERR_CNTR].i++;		//��������� �����. �������
		DataBuffer0[1] |= MOD_ERROR_CODE_FLAG;	//� ������ �-��� ��������� ��������� �������� ������ (0x80)
		DataBuffer0[2] = ILLEGAL_DATA_VALUE;			//�������� ��� ����������
		size = 2;
	}
	SET_DE485_0();
	DataBuffer0[0] = MBClientGetHostID0();	//������ ��-� ������ - ����� �����
	//���������� CRC
	crc=CRC16(DataBuffer0, size + 1, 0xFFFF);
	DataBuffer0[size + 1]=(unsigned char)(crc & 0xFF);
	DataBuffer0[size + 2]=(unsigned char)(crc >> 8);
	//����������� ���������(������������ � ����������� ���������� UART) �� ����� 
	pBuf0=DataBuffer0;
	BufferSize0=size + 3;		//������ ����������� ������ ������ ������+1(�����)+2(CRC)
	//SIZE += BufferSize0;
	UCR0 |= (1 << UDRIE0);		//���������� ����������
	while (!(UCSR0B & (1 << RXEN0)));			//���� �� ��������� �����.
	state0 = State_INITIAL; //���������� ��������� �������������
}

uint8_t MBClientReceive0(void)
{
	static uint8_t stage = 0;
		
	switch(stage)
	{
		case 0:
			BufferSize0 = 0;			//������� ������
			state0 = State_INITIAL;	//!!!!!!!!!!!!!!!!!!!!!
			StartModbusTimer0();
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
			if(timer0 > T35_Ticks)
			{
				stage++;
				state0 = State_IDLE; //���������� ��������� �������������
			}
			break;
		case 3:
			if(state0 == State_RECEIVING || state0 == State_WAITING)
			{
				stage++;
			}
			break;
		case 4:
			if(timer0 > T15_Ticks)
			{
				stage++;
				state0=State_WAITING;
			}
		case 5:
			if(timer0 > T35_Ticks)
			{
				stage = 0;
				//�������� ������� ���������� ������
				if (!((BufferSize0 < 4) || (BufferSize0 > MBCLIENT_MAX_LENGHT)))
				{
					//��������� CRC
					if(!CheckCRC0())
					{
						CPT0[BUS_MSG_CNTR].i++;
						//���������, ��� �� ������������ �����
						if ((DataBuffer0[0] == 0) || (DataBuffer0[0] == MBClientGetHostID0()))		
						{
							//���� �� ��������� ���������� �����	        
							CPT0[SLAVE_MSG_CNTR].i++;
							return (BufferSize0 - 3);
						}
					}
					else
						{CPT0[BUS_COM_ERR_CNTR].i++;}
				}
				else
					{CPT0[BUS_COM_ERR_CNTR].i++;}
				
					}
			break;

	}
	return 0;
}

//��������� ���������� ����� ������ � UART
SIGNAL(USART_RX_vect)
{
	uint8_t temp = *pUDR0;
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
		DataBuffer0[BufferSize0++] = temp; //������� ���������� ������ � �����
		if(BufferSize0 >= MBCLIENT_MAX_LENGHT)//���� ������ ������ �������� ������������ ��������
			state0 = State_WAITING;//������� � ��������� ��������
	}
	StartModbusTimer0();//������ �������
}

SIGNAL(USART_TX_vect)
{
	
	state0 = State_IDLE; //���������� ��������� �������������
	CLR_DE485_0();
}

SIGNAL(USART_UDRE_vect)
{
	//sbi(UCSR0B,TXC0);
	*pUDR0 = *pBuf0; /* start transmition */
	pBuf0++;
	BufferSize0--;
	if(!BufferSize0)
	{
		cbi(UCR0, UDRIE0); /* disable UDRE interrupt */
		sbi(UCSR0B, TXCIE0);
	}
}
