#include "Config.h"
#if (_MB_SERIAL_)
#include <stdio.h>
#include <string.h>
#include "DriversCore.h"
#include "MB_serial_port.h"

uint16_t CRC16(uint8_t * puchMsg, uint16_t usDataLen, uint16_t prevCRC);

DEVICE_HANDLE GetMBDriverHandle(MB_SER_PORT_INFO *mbInfo)
{
	return mbInfo->mb_driver;
}

uint8_t * GetMBDataBuff(MB_SER_PORT_INFO *mbInfo)
{
	return mbInfo->buf;
}

void MBSBusSerPortInit(MB_SER_PORT_INFO *mbInfo)
{
    mbInfo->mb_driver = OpenDevice(_MBS_L_L_DRIVER_NAME_);
#if _MB_DE_CONTROL_
	if(mbInfo->useRS485)
		INIT_DE485();
#endif
}

void MBCBusSerPortInit(MB_SER_PORT_INFO *mbInfo)
{
    mbInfo->mb_driver = OpenDevice(_MBC_L_L_DRIVER_NAME_);
#if _MB_DE_CONTROL_
	if(mbInfo->useRS485)
		INIT_DE485();
#endif
}

// ������� ��������� �� ������
int MBSerialErrorResponse(MB_SER_PORT_INFO *mbInfo, uint8_t ExceptionCode)
{
	mbInfo->CPT0[SLAVE_EXSEPT_ERR_CNTR].i++;		//��������� �����. �������
	mbInfo->buf[1] |= MOD_ERROR_CODE_FLAG;	//� ������ �-��� ��������� ��������� �������� ������ (0x80)
	mbInfo->buf[2] = ExceptionCode;			//�������� ��� ����������
	return 2;   //  ������ ����������� ������ � �������
}

uint8_t GetErrCounterHigh(MB_SER_PORT_INFO *mbInfo, uint8_t index)
{
	return mbInfo->CPT0[index].b[1];
}

uint8_t GetErrCounterLow(MB_SER_PORT_INFO *mbInfo, uint8_t index)
{
	return mbInfo->CPT0[index].b[0];
}

void ClearCounters(MB_SER_PORT_INFO *mbInfo)
{
	int k = 0;
	for(k = 1; k < 9; k++)				//���������� ������� ��������
		mbInfo->CPT0[k].i = 0;
}

uint8_t mb_check_crc(MB_SER_PORT_INFO *mbInfo)
{
	//���������, ��������� �� ��������� CRC � �����������
	if (CRC16(mbInfo->buf, mbInfo->size - 2, 0xFFFF) != *(uint16_t *)(mbInfo->buf + mbInfo->size - 2))
		return CRC_ERROR; //���� ��� - ������� ������
	return 0;
}


void MBSerialSend(MB_SER_PORT_INFO *mbInfo, uint8_t size, uint8_t id)
{
	uint16_t crc;

	if(size > (MBCLIENT_MAX_LENGHT - 3))
	{
		mbInfo->CPT0[SLAVE_EXSEPT_ERR_CNTR].i++;		//��������� �����. �������
		mbInfo->buf[1] |= MOD_ERROR_CODE_FLAG;	//� ������ �-��� ��������� ��������� �������� ������ (0x80)
		mbInfo->buf[2] = ILLEGAL_DATA_VALUE;			//�������� ��� ����������
		size = 2;
	}
#if _MB_DE_CONTROL_
	if(mbInfo->useRS485)
    	SET_DE485();
#endif
	mbInfo->buf[0] = id;	//������ ��-� ������ - ����� �����
	//���������� CRC
	crc=CRC16(mbInfo->buf, size + 1, 0xFFFF);
	mbInfo->buf[size + 1]=(uint8_t)(crc & 0xFF);
	mbInfo->buf[size + 2]=(uint8_t)(crc >> 8);
	//����������� ���������(������������ � ����������� ���������� UART) �� ����� 
	mbInfo->size = size + 3;		//������ ����������� ������ ������ ������+1(�����)+2(CRC)
    WriteDevice(mbInfo->mb_driver, (void *)mbInfo->buf, mbInfo->size);
    mbInfo->size = 0;
#if _MB_DE_CONTROL_
	if(mbInfo->useRS485)
		CLR_DE485();
#endif
}

uint8_t * MBSerialReceive(MB_SER_PORT_INFO *mbInfo, uint8_t id, uint8_t *size)
{	
    mbInfo->size = ReadDevice(mbInfo->mb_driver, (void *)mbInfo->buf, MBCLIENT_MAX_LENGHT);
	if (!((mbInfo->size < 4) || (mbInfo->size > MBCLIENT_MAX_LENGHT)))
	{
		//��������� CRC
		if(!mb_check_crc(mbInfo))
		{
			mbInfo->CPT0[BUS_MSG_CNTR].i++;
			//���������, ��� �� ������������ �����
			if ((mbInfo->buf[0] == 0) || (mbInfo->buf[0] == id))		
			{
				//���� �� ��������� ���������� �����	        
				mbInfo->CPT0[SLAVE_MSG_CNTR].i++;
				*size = (mbInfo->size - 3);

				return mbInfo->buf;
			}
		}
		else
			{mbInfo->CPT0[BUS_COM_ERR_CNTR].i++;}
	}
	else
		{mbInfo->CPT0[BUS_COM_ERR_CNTR].i++;}
	return NULL; 
}

#endif
