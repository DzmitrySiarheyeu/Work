#ifndef _MODBUS_H_
#define _MODBUS_H_

#include <stdint.h>

#define MODBUS_ID 			99 

#define MODBUS_ERROR_FLAG	0x80

#define MODBUS_ILLEGAL_FUNCTION		1
#define MODBUS_ILLEGAL_DATA_ADRESS	2
#define MODBUS_ILLEGAL_DATA_VALUE	3
#define MODBUS_SLAVE_DEVICE_FALURE	4

#define MODBUS_READ_INPUT_REGISTERS	0x04
#define MODBUS_WRITE_FLASH  		0x45
#define MODBUS_READ_FLASH			0x46

typedef struct 
{
	uint8_t device;
	uint32_t adress;
	uint8_t count;
	uint8_t data[0xF7];
} __attribute__ (( __packed__ )) MemoryOperate;

typedef struct 
{
	uint16_t adress;
	uint16_t quantity;
} __attribute__ (( __packed__ )) ReadInputRegsReq;

typedef struct 
{
	uint8_t count;
	uint8_t regs[4];
} __attribute__ (( __packed__ )) ReadInputRegsResp;

int ModbusProcPacket(int PacketSize);

#endif // _MODBUS_H_
