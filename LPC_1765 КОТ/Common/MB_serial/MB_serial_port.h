#ifndef _MODBUS_SERIAL_PORT_H_
#define _MODBUS_SERIAL_PORT_H_

#include "Config.h"

#if (_MB_SERIAL_)

#include <stdint.h>
#include "driverscore.h"


#define BUS_MSG_CNTR					1
#define BUS_COM_ERR_CNTR				2
#define SLAVE_EXSEPT_ERR_CNTR           3
#define SLAVE_MSG_CNTR					4
#define SLAVE_NO_RESP_CNTR				5
#define SLAVE_NAK_CNTR					6
#define SLAVE_BUSY_CNTR					7
#define BUS_CHAR_OVRRUN_CNTR            8

#define FRAME_ERROR       1
#define FRAME_SIZE_ERROR  2
#define CRC_ERROR         3

//Exception codes
#define	MB_NO_ERROR			0
#define ILLEGAL_FUNCTION	1
#define ILLEGAL_DATA_ADRESS	2
#define ILLEGAL_DATA_VALUE	3
#define SLAVE_DEVICE_FALURE	4


#define MOD_ERROR_CODE_FLAG				0x80  // Код функции ошибки

//Максимальная длина пакета
#define MBCLIENT_MAX_LENGHT	250

typedef union __attribute__ ((__packed__)) tagModbusInt
{
	uint16_t i;
	uint8_t b[2]; 
}MODBUS_INT;

typedef struct tagMB_SER_PORT_INFO
{
	MODBUS_INT CPT0[9];
	uint8_t buf[ MBCLIENT_MAX_LENGHT ];
	uint8_t size;
	uint8_t useRS485;
	DEVICE_HANDLE mb_driver;
}MB_SER_PORT_INFO;


void MBSBusSerPortInit(MB_SER_PORT_INFO *mbInfo);
void MBCBusSerPortInit(MB_SER_PORT_INFO *mbInfo);
	
int MBSerialErrorResponse(MB_SER_PORT_INFO *mbInfo, uint8_t ExceptionCode);
void MBSerialSend(MB_SER_PORT_INFO *mbInfo, uint8_t size, uint8_t id);
uint8_t * MBSerialReceive(MB_SER_PORT_INFO *mbInfo, uint8_t id, uint8_t *size);

uint8_t GetErrCounterHigh(MB_SER_PORT_INFO *mbInfo, uint8_t index);
uint8_t GetErrCounterLow(MB_SER_PORT_INFO *mbInfo, uint8_t index);
void ClearCounters(MB_SER_PORT_INFO *mbInfo);
uint8_t * GetMBDataBuff(MB_SER_PORT_INFO *mbInfo);
DEVICE_HANDLE GetMBDriverHandle(MB_SER_PORT_INFO *mbInfo);

#endif
#endif
