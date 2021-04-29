#ifndef _MB_SERIAL_CLIENT_H_
#define _MB_SERIAL_CLIENT_H_

#include "Config.h"
#if (_MB_SERIAL_ && _MB_SERIAL_CLIENT_)
#include <stdint.h>
#include "MB_serial_port.h"

//Diagnostic subfunctions
#define RETURN_QUERY_DATA				0x00
#define CLEAR_COUNTERS					0x0A
#define BUS_MESSAGE_COUNT				0x0B
#define BUS_COM_ERROR_COUNT				0x0C
#define SLAVE_EXCEPT_ERROR_COUNT        0x0D
#define SLAVE_MESSAGE_COUNT				0x0E
#define SLAVE_NO_RESPONCE_COUNT         0x0F
#define SLAVE_NAK_COUNT					0x10
#define SLAVE_BUSY_COUNT				0x11
#define BUS_CHARRACTER_OVERRUN_COUNT    0x12

//коды функций
#define MOD_READ_COILS					(0x01)
#define MOD_READ_HOLDING_REGISTERS      (0x03)
#define MOD_READ_INPUT_REGISTER         (0x04)
#define MOD_WRITE_MULTIPLE_REGISTERS    (0x10)
#define MOD_DIAGNOSTIC					(0x08)
#define MOD_WRITE_MULTIPLE_COILS        (0x0F)
#define MOD_READ_FILE_RECORD            (0x14)
#define MOD_REPORT_SLAVE_ID				(0x11)
#define MOD_WRITE_SINGLE_REGISTER       (0x06)
#define MOD_READ_FLASH					(0x46)
#define MOD_WRITE_FLASH					(0x45)
#define MOD_WRITE_OPTION				(0x47)////////////// Моя функция записи параметров МОДБАС
#define MOD_WRITE_FILE                  (0x15)

//Exception codes
#define	MB_NO_ERROR			0
#define ILLEGAL_FUNCTION	1
#define ILLEGAL_DATA_ADRESS	2
#define ILLEGAL_DATA_VALUE	3
#define SLAVE_DEVICE_FALURE	4

void MBClientProcess(MB_SER_PORT_INFO *mbInfo, uint8_t *buffer, uint8_t size, uint8_t id);


#endif
#endif

