#ifndef __MBSERVER_H__
#define __MBSERVER_H__
#include <stdint.h>

void ServerReadInputRegister(uint16_t adress, uint8_t count, uint8_t id);
void ServerWriteSingleRegister(uint16_t adress, uint16_t reg, uint8_t id);

float GetInpRegsFLOAT(uint8_t index);
uint16_t GetInpRegsUSHORT(uint8_t index);
uint32_t GetInpRegsLONG(uint8_t index);
//void memrevercy(uint8_t *data, uint8_t count);

void Modbus_enable(bool rs485);
void Modbus_disable();

uint8_t modbus_write_ok(uint8_t id);
uint8_t modbus_read_ok(uint16_t count, uint8_t id);
uint8_t modbus_get_id();

typedef struct __attribute__((__packed__)) tagReadInputRegistersRequest {
    uint8_t function_code;
    uint8_t st_adr_hi;
    uint8_t st_adr_lo;
    uint8_t quantity_hi;
    uint8_t quantity_lo;
}
READ_INPUT_REGS_REQ;

typedef struct __attribute__((__packed__)) tagReadInputRegistersResponse {
    uint8_t function_code;
    uint8_t count;
    uint8_t InpRegs[0xFA];
}
READ_INPUT_REGS_RESP;

typedef struct {
    uint8_t function_code;
    uint8_t st_adr_hi;
    uint8_t st_adr_lo;
    uint8_t value_hi;
    uint8_t value_lo;
} __attribute__((__packed__)) WRITE_SINGLE_REGISTER_REQ;

typedef union __attribute__((__packed__)) tagReadInputRegs {
    READ_INPUT_REGS_REQ request;
    READ_INPUT_REGS_RESP response;
}
READ_INPUT_REGS;

#define MOD_SERV_READ_INPUT_REGISTER         0x04
#define MOD_SERV_WRITE_SINGLE_REGISTER       0x06

#endif



