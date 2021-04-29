#include "Config.h"
#if _MODBUS_SERVER_
#include <string.h>
#include "MB_serial_port.h"
#include "MBServer.h"

static uint8_t *Buf_data;

MB_SER_PORT_INFO MBSInfo = {.useRS485 = 1};

uint8_t ServerReadInputRegisters1(uint16_t adress, uint8_t count, uint8_t id)
{
    READ_INPUT_REGS_REQ *req;
    req = (READ_INPUT_REGS_REQ *)(GetMBDataBuff(&MBSInfo) + 1);
    req->function_code = MOD_SERV_READ_INPUT_REGISTER;
    req->st_adr_hi = (uint8_t)(adress >> 8);
    req->st_adr_lo = (uint8_t)adress;
    req->quantity_hi = 0;
    req->quantity_lo = count;
    uint16_t TrustSize = 2 + 3 + count*2;
    MBSerialSend(&MBSInfo, sizeof(READ_INPUT_REGS_REQ), id);
    uint8_t Size_R = 0;
    Buf_data = MBSerialReceive(&MBSInfo, id, &Size_R);
    if((TrustSize - 3) == Size_R ) return 1;
    else return 0;
}

uint8_t ServerWriteSingleRegister1(uint16_t adress, uint16_t reg, uint8_t id)
{
    WRITE_SINGLE_REGISTER_REQ *req;
    req = (WRITE_SINGLE_REGISTER_REQ *)(GetMBDataBuff(&MBSInfo) + 1);
    req->function_code = MOD_SERV_WRITE_SINGLE_REGISTER;
    req->st_adr_hi = (uint8_t)(adress >> 8);
    req->st_adr_lo = (uint8_t)adress;
    req->value_hi = (uint8_t)(reg >> 8);
    req->value_lo = (uint8_t)reg;
    uint16_t TrustSize = 8;
    MBSerialSend(&MBSInfo, sizeof(WRITE_SINGLE_REGISTER_REQ), id);
    
    uint8_t Size_R = 0;
    Buf_data = MBSerialReceive(&MBSInfo, id, &Size_R);
    if((TrustSize - 3) == Size_R ) return 1;
    else return 0;
}

void memrevercy(uint8_t *data, uint8_t count);							  

uint16_t GetInpRegsUSHORT(uint8_t index)
{
    uint16_t hi;
    hi = Buf_data[3 + index * 2] << 8;
    return (uint16_t)(hi + Buf_data[ 4 + index * 2 ]);
}

float GetInpRegsFLOAT(uint8_t index)
{
    float in_reg[1];
    uint8_t in_data[4];
    memcpy((uint8_t *)in_data, (uint8_t *)(&Buf_data[3 + index * 2]), 4);
    memrevercy(in_data, 4);
    memcpy(in_reg, in_data, 4);
    return in_reg[0];
}

uint32_t GetInpRegsLONG(uint8_t index)
{
    uint32_t in_reg[1];
    uint8_t in_data[4];
    memcpy((uint8_t *)in_data, (uint8_t *)(&Buf_data[3 + index * 2]), 4);
    memrevercy(in_data, 4);
    memcpy(in_reg, in_data, 4);
    return in_reg[0];
}

void memrevercy(uint8_t *data, uint8_t count)
{
    uint8_t i;
    uint8_t temp;
    for (i = 0; i < count / 2; i++)
    {
        temp = data[i];
        data[i] = data[count - i - 1];
        data[count - i - 1] = temp;
    }
}

uint8_t ServerReadInputRegisters(uint16_t adress, uint8_t count, uint8_t id)
{
    uint8_t c = 2;
    uint8_t a;
    do
    {
        a = ServerReadInputRegisters1(adress, count, id);
    }
    while(c-- && !(a));   //  ≈сли ничего ни принимаем повтор€ем попытку 5 раз
    return a;
}

 #endif


