
/*include*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include <string.h>
#include <avr\wdt.h>
#include <avr/pgmspace.h>
#include "iit_boot.h"
#include "mbfanction0.h"
#include "mbclient.h"
#include "rs_dot.h"


///////////////////////// ��� ������� ����� //////////////////
__ALSO_VAR__ MODBUS_INT CPT0[9];
__ID__ byte hostID0 = 99;

// ������� ��������� �� ������
__SEND_ERROR__ void SendErrorResponse(byte ExceptionCode)
{
	CPT0[SLAVE_EXSEPT_ERR_CNTR].i++;		//��������� �����. �������
	DataBuffer0[1] |= MOD_ERROR_CODE_FLAG;	//� ������ �-��� ��������� ��������� �������� ������ (0x80)
	DataBuffer0[2] = ExceptionCode;			//�������� ��� ����������
	MBClientSend(2);			//�������
}

// ��������� �-��� 0x08 - �����������
__DIAGNOSTIC__ void DiagnosticModbus0(byte *buffer,byte Size)
{
	byte i;
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

byte CheckCRC0(void)
{
	//���������, ��������� �� ��������� CRC � �����������
	if (CRC16(DataBuffer0, BufferSize0 - 2, 0xFFFF) != *(unsigned int *)(DataBuffer0 + BufferSize0 - 2))
		return CRC_ERROR; //���� ��� - ������� ������
	return 0;
}

__MBCL_SEND__ void MBSend(unsigned char clientID, unsigned char size)
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
	DataBuffer0[0] = clientID;	//������ ��-� ������ - ����� �����
	//���������� CRC
	crc=CRC16(DataBuffer0, size + 1, 0xFFFF);
	DataBuffer0[size + 1]=(unsigned char)(crc & 0xFF);
	DataBuffer0[size + 2]=(unsigned char)(crc >> 8);
	//����������� ���������(������������ � ����������� ���������� UART) �� ����� 
	pBuf0=DataBuffer0;
	BufferSize0=size + 3;		//������ ����������� ������ ������ ������+1(�����)+2(CRC)
	UCR0 |= (1 << UDRIE0);		//���������� ����������
	while (!(UCSR0B & (1 << RXEN0)));			//���� �� ��������� �����.
	state0 = State_INITIAL; //���������� ��������� �������������
}

inline void MBClientSend(unsigned char size)
{
    MBSend(MBClientGetHostID0(), size);
}


// �������� �� ��� �� �������� ����� - ����� (�� �����) ����� ������
byte check_modbus_packet(unsigned char clientID)
{
	//�������� ������� ���������� ������
	if (!((BufferSize0 < 4) || (BufferSize0 > MBCLIENT_MAX_LENGHT)))
	{
		//��������� CRC
		if(!CheckCRC0())
		{
			CPT0[BUS_MSG_CNTR].i++;
			//���������, ��� �� ������������ �����
			if ((DataBuffer0[0] == 0) || (DataBuffer0[0] == clientID))		
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
	
	return 0;
}


inline byte MBClientReceive0(void)
{
    return UartRecivePacket();
}


