#include <string.h>
#include <stdint.h>
#include "mbfanction0.h"
#include "mbclient.h"
#include "crc16.h"
#include "uart.h"
 													   
MODBUS_INT CPT0[9];
byte hostID0 = 99;

// ������� ��������� �� ������
void SendErrorResponse(byte ExceptionCode)
{
	CPT0[SLAVE_EXSEPT_ERR_CNTR].i++;		//��������� �����. �������
	UartBuff[1] |= MOD_ERROR_CODE_FLAG;	//� ������ �-��� ��������� ��������� �������� ������ (0x80)
	UartBuff[2] = ExceptionCode;			//�������� ��� ����������
	MBClientSend(2);			//�������
}

// ��������� �-��� 0x08 - �����������
void DiagnosticModbus0(byte *buffer,byte Size)
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

byte CheckCRC0(int length)
{
	//���������, ��������� �� ��������� CRC � �����������
	uint16_t crc = 0;
	memcpy((char *)&crc, (UartBuff + length - 2), 2);
	if (CRC16(UartBuff, length - 2, 0xFFFF) != crc)
		return CRC_ERROR; //���� ��� - ������� ������
	return 0;
}

void MBSend(unsigned char clientID, unsigned char size)
{
	unsigned int crc;

	if(size > 252)
	{
		CPT0[SLAVE_EXSEPT_ERR_CNTR].i++;		//��������� �����. �������
		UartBuff[1] |= MOD_ERROR_CODE_FLAG;	//� ������ �-��� ��������� ��������� �������� ������ (0x80)
		UartBuff[2] = ILLEGAL_DATA_VALUE;			//�������� ��� ����������
		size = 2;
	}

	UartBuff[0] = clientID;	//������ ��-� ������ - ����� �����
	//���������� CRC
	crc=CRC16(UartBuff, size + 1, 0xFFFF);
	UartBuff[size + 1]=(unsigned char)(crc & 0xFF);
	UartBuff[size + 2]=(unsigned char)(crc >> 8);
			//������ ����������� ������ ������ ������+1(�����)+2(CRC)

	UARTSend(size + 3);
}

void MBClientSend(unsigned char size)
{
    MBSend(MBClientGetHostID0(), size);
}


// �������� �� ��� �� �������� ����� - ����� (�� �����) ����� ������
byte check_modbus_packet(unsigned char clientID, int packet_size)
{
	CheckCRC0(11);
	//�������� ������� ���������� ������
	if (!((packet_size < 4) || (packet_size > MBCLIENT_MAX_LENGHT)))
	{
		//��������� CRC
		if(!CheckCRC0(packet_size))
		{
			CPT0[BUS_MSG_CNTR].i++;
			//���������, ��� �� ������������ �����
			if ((UartBuff[0] == 0) || (UartBuff[0] == clientID))		
			{
				//���� �� ��������� ���������� �����	        
				CPT0[SLAVE_MSG_CNTR].i++;
				return (packet_size - 3);
			}
		}
		else
			{CPT0[BUS_COM_ERR_CNTR].i++;}
	}
	else
	{CPT0[BUS_COM_ERR_CNTR].i++;}
	
	return 0;
}


int MBClientReceive(void)
{
    return UARTCheckPacket();
}


