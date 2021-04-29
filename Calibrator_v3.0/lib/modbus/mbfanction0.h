#ifndef __MODBUS0_H__
#define __MODBUS0_H__

#include <avr/eeprom.h>
#include <stdint.h>

#define BUS_MSG_CNTR            1
#define BUS_COM_ERR_CNTR        2
#define SLAVE_EXSEPT_ERR_CNTR   3
#define SLAVE_MSG_CNTR          4
#define SLAVE_NO_RESP_CNTR      5
#define SLAVE_NAK_CNTR          6
#define SLAVE_BUSY_CNTR         7
#define BUS_CHAR_OVRRUN_CNTR    8

#ifdef _AVR_IOMX8_H_
#define UCR0 (*(pUDR0 - 5))
#define USR0 (*(pUDR0 - 6))
#define UBR0 (*(pUDR0 - 2))
#else
#define UCR0 (*(pUDR0 - 2))
#define USR0 (*(pUDR0 - 1))
#define UBR0 (*(pUDR0 - 3))
#endif

//Максимальная длина пакета
#define MBCLIENT_MAX_LENGHT 64 //def:140

/*Definitions*/
#define State_INITIAL       0
#define State_IDLE          1
#define State_RECEIVING     2
#define State_SENDING       3
#define State_PROCESSING    4
#define State_WAITING       5

#define T35_Ticks 202 //Количество тиков таймера(переполнений) для 1,750мс def:202
#define T15_Ticks 128 //Количество тиков таймера(переполнений) для 0,750мс def:87

//коды функций
#define MOD_READ_COILS                  (0x01)
#define MOD_READ_HOLDING_REGISTERS      (0x03)
#define MOD_READ_INPUT_REGISTER         (0x04)
#define MOD_WRITE_MULTIPLE_REGISTERS    (0x10)
#define MOD_ERROR_CODE_FLAG             (0x80)
#define MOD_DIAGNOSTIC                  (0x08)
#define MOD_WRITE_MULTIPLE_COILS        (0x0F)
#define MOD_READ_FILE_RECORD            (0x14)
#define MOD_REPORT_SLAVE_ID             (0x11)
#define MOD_WRITE_SINGLE_REGISTER       (0x06)
#define MOD_READ_FLASH                  (0x46)
#define MOD_WRITE_FLASH                 (0x45)
#define MOD_WRITE_OPTION                (0x47)////////////// Моя функция записи параметров МОДБАС
#define MOD_RESET                       (0x44)
#define MOD_BOOT                        (0x50)


#define FRAME_ERROR 1
#define FRAME_SIZE_ERROR 2
#define CRC_ERROR 3

//Exception codes
#define ILLEGAL_FUNCTION    1
#define ILLEGAL_DATA_ADRESS 2
#define ILLEGAL_DATA_VALUE  3
#define SLAVE_DEVICE_FALURE 4

//Diagnostic subfunctions
#define RETURN_QUERY_DATA               0x00
#define CLEAR_COUNTERS                  0x0A
#define BUS_MESSAGE_COUNT               0x0B
#define BUS_COM_ERROR_COUNT             0x0C
#define SLAVE_EXCEPT_ERROR_COUNT        0x0D
#define SLAVE_MESSAGE_COUNT             0x0E
#define SLAVE_NO_RESPONCE_COUNT         0x0F
#define SLAVE_NAK_COUNT                 0x10
#define SLAVE_BUSY_COUNT                0x11
#define BUS_CHARRACTER_OVERRUN_COUNT    0x12

typedef struct tagWriteMultipleRegisterRequest {
    uint8_t function_code;
    uint8_t st_adr_hi;
    uint8_t st_adr_lo;
    uint8_t quantity_hi;
    uint8_t quantity_lo;
    uint8_t count;
    uint8_t Regs[0xFA];
} WRITE_MULTIPLE_REGS_REQ;

typedef struct tagWriteMultipleRegisterResp {
    uint8_t function_code;
    uint8_t st_adr_hi;
    uint8_t st_adr_lo;
    uint8_t quantity_hi;
    uint8_t quantity_lo;
} WRITE_MULTIPLE_REGS_RESP;

typedef union tagWriteMultipleRegister {
    WRITE_MULTIPLE_REGS_REQ request;
    WRITE_MULTIPLE_REGS_RESP response;
} WRITE_MULTIPLE_REGS;

typedef struct tagWriteSingleRegister {
    uint8_t function_code;
    uint8_t st_adr_hi;
    uint8_t st_adr_lo;
    uint8_t value_hi;
    uint8_t value_lo;
} WRITE_SINGLE_REGISTER;

//typedef struct tagReadInputRegistersRequest
//{
//    uint8_t function_code;
//    uint8_t st_adr_hi;
//    uint8_t st_adr_lo;
//    uint8_t quantity_hi;
//    uint8_t quantity_lo;
//}   READ_INPUT_REGS_REQ;
//
//typedef struct tagReadInputRegistersResponse
//{
//    uint8_t function_code;
//    uint8_t count;
//    uint8_t InpRegs[0xFA];
//}READ_INPUT_REGS_RESP;
//
//typedef union tagReadInputRegs
//{
//    READ_INPUT_REGS_REQ request;
//    READ_INPUT_REGS_RESP response;
//}READ_INPUT_REGS;

typedef struct tagSlaveID {
    uint8_t function_code;
    uint8_t count;
    uint8_t ID;
    uint8_t RunStatus;
    uint8_t ExtraData[249];
} SLAVE_ID;

typedef struct tagSlaveIDExt {
    uint8_t function_code;
    uint8_t count;
    uint8_t ID;
    uint8_t RunStatus;
    unsigned int version;
    uint8_t MKStatus;
    uint8_t reserved;
} SLAVE_ID_EXT;

typedef struct tagFileRecordRequest {
    uint8_t function_code;
    uint8_t count;
    uint8_t ref_type;
    uint8_t FileNumberHi;
    uint8_t FileNumberLo;
    uint8_t RecordNumberHi;
    uint8_t RecordNumberLo;
    uint8_t RegLengthHi;
    uint8_t RegLengthLo;
} FILE_RECORD_REQUEST;

typedef struct tagFileRecordResponse {
    uint8_t function_code;
    uint8_t RespDataLength;
    uint8_t FileRespLength;
    uint8_t ref_type;
    uint8_t data[0xF5];
} FILE_RECORD_RESPONSE;

typedef union tagFileRecord {
    FILE_RECORD_REQUEST request;
    FILE_RECORD_RESPONSE response;
} FILE_RECORD;

typedef struct tagMemoryOperate {
    uint8_t function_code;
    uint8_t device;
    unsigned long adress;
    uint8_t count;
    uint8_t data[0xF7];
} MEMORY_OPERATE;

// Запись параметров устройства

typedef struct tagWriteOption {
    uint8_t function_code;
    uint8_t mode;
    uint8_t ID0;
    uint8_t ID1;
    unsigned long boudrate0;
    unsigned long boudrate1;
} WRITE_OPTION;

extern uint8_t DataBuffer0[ MBCLIENT_MAX_LENGHT ];
extern volatile unsigned char *pUDR0;
//extern volatile uint8_t * volatile pBuf0;
//extern volatile uint8_t BufferSize0;
//extern volatile uint8_t state0;
extern uint8_t use_rs485;

typedef union tagModbusInt {
    unsigned int i;
    uint8_t b[2];
} MODBUS_INT;

extern MODBUS_INT CPT0[9];

/**
 * Send packet (master)
 * 
 * @param size packet size
 * @param id slave id
 */
void MBSerialSend(uint8_t size, uint8_t id);

uint8_t UartReceivePacket(void);
uint8_t check_modbus_packet(unsigned char clientID);

#endif
