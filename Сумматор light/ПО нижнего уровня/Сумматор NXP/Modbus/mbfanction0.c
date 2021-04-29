#include <string.h>
#include <stdint.h>
#include "mbfanction0.h"
#include "mbclient.h"
#include "crc16.h"
#include "uart.h"
 													   
MODBUS_INT CPT0[9];
byte hostID0 = 99;

// Посылка сообщения об ошибке
void SendErrorResponse(byte ExceptionCode)
{
	CPT0[SLAVE_EXSEPT_ERR_CNTR].i++;		//нарастить соотв. каунтер
	UartBuff[1] |= MOD_ERROR_CODE_FLAG;	//К номеру ф-ции логически прибавить значение ошибки (0x80)
	UartBuff[2] = ExceptionCode;			//записать код исключения
	MBClientSend(2);			//послать
}

// Обработка ф-ции 0x08 - диагностика
void DiagnosticModbus0(byte *buffer,byte Size)
{
	byte i;
	//Если код подф-ции возвращения какого-то счётчика
	if ((buffer[2] >= BUS_MESSAGE_COUNT) && (buffer[2] <= BUS_CHARRACTER_OVERRUN_COUNT))
	{
		buffer[3] = CPT0[buffer[2] - 0x0A].b[1];	//вычисляем и возвращаем этот счётчик
		buffer[4] = CPT0[buffer[2] - 0x0A].b[0];
		Size = 5;								//установка размера сообщения
		MBClientSend(Size);				//послать
		return;
	}
	switch(buffer[2])
	{
		case CLEAR_COUNTERS:					//подф-ция очистки счётчиков
			for(i = 1;i < 9;i++)				//собственно очищаем
				CPT0[i].i = 0;
			break;
		case RETURN_QUERY_DATA:					//подф-ция ЭХО
			break;								//ничего не делаем
		default:										//подф-ция неизвестна 
			SendErrorResponse(ILLEGAL_FUNCTION);	//Послать соответствующее исключение
			return;
	}
	MBClientSend(Size);//послать
}

byte CheckCRC0(int length)
{
	//Проверяем, совпадает ли пришедшее CRC с вычисленным
	uint16_t crc = 0;
	memcpy((char *)&crc, (UartBuff + length - 2), 2);
	if (CRC16(UartBuff, length - 2, 0xFFFF) != crc)
		return CRC_ERROR; //Если нет - вернуть ошибку
	return 0;
}

void MBSend(unsigned char clientID, unsigned char size)
{
	unsigned int crc;

	if(size > 252)
	{
		CPT0[SLAVE_EXSEPT_ERR_CNTR].i++;		//нарастить соотв. каунтер
		UartBuff[1] |= MOD_ERROR_CODE_FLAG;	//К номеру ф-ции логически прибавить значение ошибки (0x80)
		UartBuff[2] = ILLEGAL_DATA_VALUE;			//записать код исключения
		size = 2;
	}

	UartBuff[0] = clientID;	//первый эл-т буфера - номер хоста
	//дописываем CRC
	crc=CRC16(UartBuff, size + 1, 0xFFFF);
	UartBuff[size + 1]=(unsigned char)(crc & 0xFF);
	UartBuff[size + 2]=(unsigned char)(crc >> 8);
			//размер посылаемого пакета размер данных+1(адрес)+2(CRC)

	UARTSend(size + 3);
}

void MBClientSend(unsigned char size)
{
    MBSend(MBClientGetHostID0(), size);
}


// проверка на точ то принятый пакет - целый (не битый) пакет модбас
byte check_modbus_packet(unsigned char clientID, int packet_size)
{
	CheckCRC0(11);
	//контроль размера полученных данных
	if (!((packet_size < 4) || (packet_size > MBCLIENT_MAX_LENGHT)))
	{
		//проверяем CRC
		if(!CheckCRC0(packet_size))
		{
			CPT0[BUS_MSG_CNTR].i++;
			//проверяем, нам ли предназначен пакет
			if ((UartBuff[0] == 0) || (UartBuff[0] == clientID))		
			{
				//если всё правильно возвращаем длину	        
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


