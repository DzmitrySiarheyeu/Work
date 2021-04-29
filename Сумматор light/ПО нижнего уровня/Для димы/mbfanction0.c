
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


///////////////////////// ДЛЯ ПЕРВОГО ПОРТА //////////////////
__ALSO_VAR__ MODBUS_INT CPT0[9];
__ID__ byte hostID0 = 99;

// Посылка сообщения об ошибке
__SEND_ERROR__ void SendErrorResponse(byte ExceptionCode)
{
	CPT0[SLAVE_EXSEPT_ERR_CNTR].i++;		//нарастить соотв. каунтер
	DataBuffer0[1] |= MOD_ERROR_CODE_FLAG;	//К номеру ф-ции логически прибавить значение ошибки (0x80)
	DataBuffer0[2] = ExceptionCode;			//записать код исключения
	MBClientSend(2);			//послать
}

// Обработка ф-ции 0x08 - диагностика
__DIAGNOSTIC__ void DiagnosticModbus0(byte *buffer,byte Size)
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

byte CheckCRC0(void)
{
	//Проверяем, совпадает ли пришедшее CRC с вычисленным
	if (CRC16(DataBuffer0, BufferSize0 - 2, 0xFFFF) != *(unsigned int *)(DataBuffer0 + BufferSize0 - 2))
		return CRC_ERROR; //Если нет - вернуть ошибку
	return 0;
}

__MBCL_SEND__ void MBSend(unsigned char clientID, unsigned char size)
{
	unsigned int crc;

	if(size > 252)
	{
		CPT0[SLAVE_EXSEPT_ERR_CNTR].i++;		//нарастить соотв. каунтер
		DataBuffer0[1] |= MOD_ERROR_CODE_FLAG;	//К номеру ф-ции логически прибавить значение ошибки (0x80)
		DataBuffer0[2] = ILLEGAL_DATA_VALUE;			//записать код исключения
		size = 2;
	}
	SET_DE485_0();
	DataBuffer0[0] = clientID;	//первый эл-т буфера - номер хоста
	//дописываем CRC
	crc=CRC16(DataBuffer0, size + 1, 0xFFFF);
	DataBuffer0[size + 1]=(unsigned char)(crc & 0xFF);
	DataBuffer0[size + 2]=(unsigned char)(crc >> 8);
	//Настраиваем указатель(используется в обработчике прерывания UART) на буфер 
	pBuf0=DataBuffer0;
	BufferSize0=size + 3;		//размер посылаемого пакета размер данных+1(адрес)+2(CRC)
	UCR0 |= (1 << UDRIE0);		//разрешение прерывания
	while (!(UCSR0B & (1 << RXEN0)));			//пока не очистится буфер.
	state0 = State_INITIAL; //Установить состояние инициализации
}

inline void MBClientSend(unsigned char size)
{
    MBSend(MBClientGetHostID0(), size);
}


// проверка на точ то принятый пакет - целый (не битый) пакет модбас
byte check_modbus_packet(unsigned char clientID)
{
	//контроль размера полученных данных
	if (!((BufferSize0 < 4) || (BufferSize0 > MBCLIENT_MAX_LENGHT)))
	{
		//проверяем CRC
		if(!CheckCRC0())
		{
			CPT0[BUS_MSG_CNTR].i++;
			//проверяем, нам ли предназначен пакет
			if ((DataBuffer0[0] == 0) || (DataBuffer0[0] == clientID))		
			{
				//если всё правильно возвращаем длину	        
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


