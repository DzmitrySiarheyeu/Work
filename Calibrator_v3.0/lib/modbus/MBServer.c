#include <string.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "MBServer.h"
#include "mbfanction0.h"

static void memrevercy(uint8_t *data, uint8_t count);

static uint8_t modbus_ok(uint16_t TrustSize, uint8_t id);
static uint8_t *Buf_data;

void Modbus_enable(bool rs485) {
    pUDR0 = &UDR0;
    TIMSK2 |= (1 << TOV2);
    use_rs485 = (rs485) ? 1 : 0;
}

void Modbus_disable() {
    TCCR2A = 0x00;
    TCCR2B = 0x00;
    TIMSK2 &= ~(1 << TOV2);
}

void ServerReadInputRegister(uint16_t adress, uint8_t count, uint8_t id) {
    READ_INPUT_REGS_REQ *req;

    req = (READ_INPUT_REGS_REQ *) (&DataBuffer0[1]);
    req->function_code = MOD_SERV_READ_INPUT_REGISTER;
    req->st_adr_hi = (uint8_t) (adress >> 8);
    req->st_adr_lo = (uint8_t) adress;
    req->quantity_hi = 0;
    req->quantity_lo = count;
    //uint16_t TrustSize = 2 + 3 + count * 2;

    MBSerialSend(sizeof (READ_INPUT_REGS_REQ), id);
}

void ServerWriteSingleRegister(uint16_t adress, uint16_t reg, uint8_t id) {
    WRITE_SINGLE_REGISTER_REQ *req;

    req = (WRITE_SINGLE_REGISTER_REQ *) (&DataBuffer0[1]);
    req->function_code = MOD_SERV_WRITE_SINGLE_REGISTER;
    req->st_adr_hi = (uint8_t) (adress >> 8);
    req->st_adr_lo = (uint8_t) adress;
    req->value_hi = (uint8_t) (reg >> 8);
    req->value_lo = (uint8_t) reg;

    MBSerialSend(sizeof (WRITE_SINGLE_REGISTER_REQ), id);
}

static uint8_t modbus_ok(uint16_t TrustSize, uint8_t id) {
    if (!UartReceivePacket())
        return 0;

    uint8_t Size_R = check_modbus_packet(id);
    Buf_data = (Size_R) ? DataBuffer0 : 0;

    if ((TrustSize - 3) == Size_R) return 1;
    else return 0;
}

uint8_t modbus_get_id() {
    return DataBuffer0[0];
}

uint8_t modbus_write_ok(uint8_t id) {
    uint16_t TrustSize = 8;
    return modbus_ok(TrustSize, id);
}

uint8_t modbus_read_ok(uint16_t count, uint8_t id) {
    uint16_t TrustSize = 2 + 3 + count * 2;
    return modbus_ok(TrustSize, id);
}

uint16_t GetInpRegsUSHORT(uint8_t index) {
    uint16_t hi;
    hi = Buf_data[3 + index * 2] << 8;
    return (uint16_t) (hi + Buf_data[ 4 + index * 2 ]);
}

float GetInpRegsFLOAT(uint8_t index) {
    float in_reg[1];
    uint8_t in_data[4];
    memcpy((uint8_t *) in_data, (uint8_t *) (&Buf_data[3 + index * 2]), 4);
    memrevercy(in_data, 4);
    memcpy(in_reg, in_data, 4);
    return in_reg[0];
}

uint32_t GetInpRegsLONG(uint8_t index) {
    uint32_t in_reg[1];
    uint8_t in_data[4];
    memcpy((uint8_t *) in_data, (uint8_t *) (&Buf_data[3 + index * 2]), 4);
    memrevercy(in_data, 4);
    memcpy(in_reg, in_data, 4);
    return in_reg[0];
}

static void memrevercy(uint8_t *data, uint8_t count) {
    uint8_t i;
    uint8_t temp;
    for (i = 0; i < count / 2; i++) {
        temp = data[i];
        data[i] = data[count - i - 1];
        data[count - i - 1] = temp;
    }
}
