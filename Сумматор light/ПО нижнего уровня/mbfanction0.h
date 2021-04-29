#ifndef __MODBUS0_H__
#define __MODBUS0_H__

#include "base_type.h"
//#include <synchro.h>

#include "util.h"

#include "rout.h"
#include "usart.h" 


#define BUS_MSG_CNTR			1
#define BUS_COM_ERR_CNTR		2
#define SLAVE_EXSEPT_ERR_CNTR	3
#define SLAVE_MSG_CNTR			4
#define SLAVE_NO_RESP_CNTR		5
#define SLAVE_NAK_CNTR			6
#define SLAVE_BUSY_CNTR			7
#define BUS_CHAR_OVRRUN_CNTR	8

//Максимальная длина пакета
#define MBCLIENT_MAX_LENGHT	USART_PACKET_MAX_LENGHT


//коды функций
#define MOD_READ_COILS					(0x01)
#define MOD_READ_HOLDING_REGISTERS		(0x03)
#define MOD_READ_INPUT_REGISTER			(0x04)
#define MOD_WRITE_MULTIPLE_REGISTERS	(0x10)
#define MOD_ERROR_CODE_FLAG				(0x80)
#define MOD_DIAGNOSTIC					(0x08)
#define MOD_WRITE_MULTIPLE_COILS		(0x0F)
#define MOD_READ_FILE_RECORD			(0x14)
#define MOD_REPORT_SLAVE_ID				(0x11)
#define MOD_WRITE_SINGLE_REGISTER		(0x06)
#define MOD_READ_FLASH					(0x46)
#define MOD_WRITE_FLASH					(0x45)
#define MOD_WRITE_OPTION				(0x47)////////////// Моя функция записи параметров МОДБАС
#define MOD_RESET						(0x44)
#define MOD_BOOT						(0x50)


#define FRAME_ERROR 1
#define FRAME_SIZE_ERROR 2
#define CRC_ERROR 3

//Exception codes
#define ILLEGAL_FUNCTION	1
#define ILLEGAL_DATA_ADRESS	2
#define ILLEGAL_DATA_VALUE	3
#define SLAVE_DEVICE_FALURE	4

//Diagnostic subfunctions
#define RETURN_QUERY_DATA				0x00
#define CLEAR_COUNTERS					0x0A
#define BUS_MESSAGE_COUNT				0x0B
#define BUS_COM_ERROR_COUNT				0x0C
#define SLAVE_EXCEPT_ERROR_COUNT		0x0D
#define SLAVE_MESSAGE_COUNT				0x0E
#define SLAVE_NO_RESPONCE_COUNT			0x0F
#define SLAVE_NAK_COUNT					0x10
#define SLAVE_BUSY_COUNT				0x11
#define BUS_CHARRACTER_OVERRUN_COUNT	0x12

typedef struct tagWriteSingleRegister
{
	byte function_code;
	byte st_adr_hi;
	byte st_adr_lo;
	byte value_hi;
	byte value_lo;
}WRITE_SINGLE_REGISTER;

typedef struct tagReadInputRegistersRequest
{
	byte function_code;
	byte st_adr_hi;
	byte st_adr_lo;
	byte quantity_hi;
	byte quantity_lo;
}	READ_INPUT_REGS_REQ;

typedef struct tagReadInputRegistersResponse
{
	byte function_code;
	byte count;
	byte InpRegs[0xFA];
}READ_INPUT_REGS_RESP;

typedef union tagReadInputRegs
{
	READ_INPUT_REGS_REQ request;
	READ_INPUT_REGS_RESP response;
}READ_INPUT_REGS;

typedef struct tagSlaveID
{
	byte function_code;
	byte count;
	byte ID;
	byte RunStatus;
	byte ExtraData[249];
}SLAVE_ID;

typedef struct tagSlaveIDExt
{
	byte function_code;
	byte count;
	byte ID;
	byte RunStatus;
	unsigned int version;
	byte MKStatus;
	byte reserved;
}SLAVE_ID_EXT;

typedef struct tagFileRecordRequest
{
	byte function_code;
	byte count;
	byte ref_type;
	byte FileNumberHi;
	byte FileNumberLo;
	byte RecordNumberHi;
	byte RecordNumberLo;
	byte RegLengthHi;
	byte RegLengthLo;
}FILE_RECORD_REQUEST;

typedef struct tagFileRecordResponse
{
	byte function_code;
	byte RespDataLength;
	byte FileRespLength;
	byte ref_type;
	byte data[0xF5];
}FILE_RECORD_RESPONSE;

typedef union tagFileRecord
{
	FILE_RECORD_REQUEST request;
	FILE_RECORD_RESPONSE response;
}FILE_RECORD;

typedef struct tagMemoryOperate
{
	byte function_code;
	byte device;
	unsigned long adress;
	byte count;
	byte data[0xF7];
}MEMORY_OPERATE;

// Запись параметров устройства
typedef struct tagWriteOption
{
	byte function_code;
	byte mode;
	byte ID0;
	byte ID1;
	unsigned long boudrate0;
	unsigned long boudrate1;
}WRITE_OPTION;

extern byte hostID0;

static inline void StartModbusTimer0(void)
{
	//Запуск таймера
	timer0 = 0;
}

typedef union tagModbusInt
{
	unsigned int i;
	byte b[2]; 
}MODBUS_INT;

extern MODBUS_INT CPT0[9];

/*
Посылка пакета.Параметры:
	size - размер пакета
*/
void MBClientSend(unsigned char size);
void MBSend(unsigned char clientID, unsigned char size);
/*
Приём пакета.
Возвращает размер пришедшего пакета.
*/
byte MBClientReceive0(void);

// проверка на точ то принятый пакет - целый (не битый) пакет модбас
byte check_modbus_packet(unsigned char clientID);

//Увеличивает таймер. Вставлять в обработчик прерывания 8-битного таймера
//настроенного на счёт CK 3,6864Mhz
static inline void MBClientIncTimer0(void)
{
	timer0++;//нарастим таймер
}

static inline byte MBClientGetHostID0(void)
{
	//Читаем адрес этого клиента в EEPROM
	//в переменной hostID
	return hostID0;
}

static inline void MBClientSetHostID0(byte host)
{
	//сохранить адрес этого клиента в EEPROM
	//в переменной hostID
	hostID0 = host;
}

static inline void MBClientSetBaudrate0(byte baudrate)
{
	//установить скорость UBRR=(Fck/(16*baudrate))-1
	UBR0 = baudrate;
}

static inline byte MBClientGetBaudrate0(void)
{
	//вернуть скорость
	return UBR0;
}

//Посылка сообщения об ошибке
void SendErrorResponse(byte ExceptionCode);

//Обработка ф-ции 0x08 - диагностика
void DiagnosticModbus0(byte *buffer, byte Size);


/* The function returns the CRC as a unsigned short type 
	Вычисление CRC
*/
unsigned int CRC16(byte * puchMsg, byte usDataLen, unsigned int prevCRC);

extern __ATTR_EEPROM__ byte  EEP_ID0;	
extern __ATTR_EEPROM__ unsigned long   EEP_Boudrate0;

#endif
