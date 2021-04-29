#include <string.h>

#include "modbus.h"
#include "uart.h"
#include "fms.h"
#include "sbl_iap.h"

#include "crc.h"

static void ModbusSendError(uint8_t ErrorFlag);
static void ModbusWriteFlashProc(uint8_t *buf);
static void ModbusReadFlashProc(uint8_t *buf);
static void ModbusReadInputRegisters(uint8_t *buf);

int ModbusProcPacket(int PacketSize)
{
	// проверим минимальную длину пакета 
	if(PacketSize < 4)
		return 0;
	
	// проверим ID
	if(UartBuff[0] != MODBUS_ID && UartBuff[0] != 0)
		return 0;	

	// проверим crc
	uint16_t crc = UartBuff[PacketSize - 2];
	crc |= UartBuff[PacketSize - 1] << 8;

	if(crc != CRC16 (UartBuff, PacketSize - 2, 0xffff))
		return 0;
	
	switch(UartBuff[1])
	{
		case MODBUS_WRITE_FLASH:
			ModbusWriteFlashProc(&UartBuff[2]);
			break;
		case MODBUS_READ_FLASH:
			ModbusReadFlashProc(&UartBuff[2]);
			break;
		case MODBUS_READ_INPUT_REGISTERS:
			ModbusReadInputRegisters(&UartBuff[2]);
			break;
		default:
			ModbusSendError(MODBUS_ILLEGAL_FUNCTION);
			break;	
	}

	return 1;
}

static void ModbusSendError(uint8_t ErrorFlag)
{
	UartBuff[0] = MODBUS_ID;
	UartBuff[1] |= MODBUS_ERROR_FLAG;
	UartBuff[2] = ErrorFlag;

	uint16_t crc = CRC16 (UartBuff, 3, 0xffff);

	UartBuff[4] = crc >> 8;
	UartBuff[3] = crc;

	UARTSend(5);
}

static void ModbusSendResp(int length)
{
	UartBuff[0] = MODBUS_ID;
	uint16_t crc = CRC16(UartBuff, length + 1, 0xffff);

	UartBuff[length + 2] = crc >> 8;
	UartBuff[length + 1] = crc;

	UARTSend(length + 3);		
}

static void ModbusWriteFlashProc(uint8_t *buf)
{
	MemoryOperate *mem = (MemoryOperate *)buf;

	if(mem->adress + mem->count > sizeof(FlashFMSStruct))
	{
		ModbusSendError(MODBUS_ILLEGAL_DATA_ADRESS);
		return;
	}

	write_flash((unsigned *)(TABLE_START_ADDRESS + mem->adress), (char *)mem->data, mem->count); 
	ModbusSendResp(7);
}

static void ModbusReadFlashProc(uint8_t *buf)
{
	MemoryOperate *mem = (MemoryOperate *)buf;

	if(mem->adress + mem->count > sizeof(FlashFMSStruct))
	{
		ModbusSendError(MODBUS_ILLEGAL_DATA_ADRESS);
		return;
	}

	memcpy(mem->data, TABLE_START_ADDRESS + mem->adress, mem->count);
	ModbusSendResp(mem->count + 7);
}	  

static void ModbusReadInputRegisters(uint8_t *buf)
{
	ReadInputRegsReq *req = (ReadInputRegsReq *)buf;
	ReadInputRegsResp *resp = (ReadInputRegsResp *)buf;	

	uint16_t num, addr;

	num = req->quantity >> 8;
	num |= req->quantity << 8;

	addr = req->adress >> 8;
	addr |= req->adress << 8;

	if(num != 2 || addr >= 255)
	{
		ModbusSendError(MODBUS_ILLEGAL_DATA_ADRESS);
		return;	
	}

	float value;
	if(!GetFMSValueFloat((uint8_t)addr, &value))
	{
		ModbusSendError(MODBUS_ILLEGAL_DATA_ADRESS);
		return;		
	}

	resp->count = 4;
	resp->regs[0] = *(((uint8_t *)&value) + 3);
	resp->regs[1] = *(((uint8_t *)&value) + 2);
	resp->regs[2] = *(((uint8_t *)&value) + 1);
	resp->regs[3] = *(((uint8_t *)&value) + 0);

	ModbusSendResp(6);
}
