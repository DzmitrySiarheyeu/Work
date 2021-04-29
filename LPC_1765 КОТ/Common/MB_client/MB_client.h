#ifndef _MB_CLIENT_H_
#define _MB_CLIENT_H_
#if (_MB_CLIENT_)

//Exception codes
#define	MB_NO_ERROR			0
#define ILLEGAL_FUNCTION	1
#define ILLEGAL_DATA_ADRESS	2
#define ILLEGAL_DATA_VALUE	3
#define SLAVE_DEVICE_FALURE	4

typedef struct __attribute__ ((__packed__)) tagFileRecordWrite
{
	uint8_t function_code;
	uint8_t count;
	uint8_t ref_type;
	uint8_t FileNumberHi;
	uint8_t FileNumberLo;
	uint8_t RecordNumberHi;
	uint8_t RecordNumberLo;
	uint8_t RegLengthHi;
	uint8_t RegLengthLo;
        uint8_t InpRegs[0xF6];
}WRITE_FILE;

typedef struct __attribute__ ((__packed__)) tagReadFileReq
{
	uint8_t function_code;
	uint8_t count;
	uint8_t ref_type;
	uint8_t FileNumberHi;
	uint8_t FileNumberLo;
	uint8_t RecordNumberHi;
	uint8_t RecordNumberLo;
	uint8_t RegLengthHi;
	uint8_t RegLengthLo;
}READ_FILE_REQ;

typedef struct __attribute__ ((__packed__)) tagReadFileResp
{
	uint8_t function_code;
	uint8_t count;
        uint8_t file_length;
	uint8_t ref_type;
	uint8_t InpRegs[0xFA];
}READ_FILE_RESP;

typedef union __attribute__ ((__packed__)) tagReadFile
{
    READ_FILE_REQ read_file_req;
    READ_FILE_RESP read_file_resp;
}READ_FILE;

typedef struct __attribute__ ((__packed__)) tagWriteSingleRegister
{
	uint8_t function_code;
	uint8_t st_adr_hi;
	uint8_t st_adr_lo;
	uint8_t value_hi;
	uint8_t value_lo;
}WRITE_SINGLE_REGISTER;

typedef struct __attribute__ ((__packed__)) tagWriteMultipleRegisterRequest
{
	uint8_t function_code;
	uint8_t st_adr_hi;
	uint8_t st_adr_lo;
	uint8_t quantity_hi;
	uint8_t quantity_lo;
	uint8_t count;
	uint8_t InpRegs[0xFA];
}WRITE_MULTIPLE_REGS_REQ;

typedef struct __attribute__ ((__packed__)) tagWriteMultipleRegisterRest
{
	uint8_t function_code;
	uint8_t st_adr_hi;
	uint8_t st_adr_lo;
	uint8_t quantity_hi;
	uint8_t quantity_lo;
}WRITE_MULTIPLE_REGS_RESP;

typedef union __attribute__ ((__packed__)) tagWriteMultipleRegister
{
	WRITE_MULTIPLE_REGS_REQ request;
	WRITE_MULTIPLE_REGS_RESP response;
}WRITE_MULTIPLE_REGS;


typedef struct __attribute__ ((__packed__)) tagReadInputRegistersRequest
{
	uint8_t function_code;
	uint8_t st_adr_hi;
	uint8_t st_adr_lo;
	uint8_t quantity_hi;
	uint8_t quantity_lo;
}	READ_INPUT_REGS_REQ;

typedef struct __attribute__ ((__packed__)) tagReadInputRegistersResponse
{
	uint8_t function_code;
	uint8_t count;
	uint8_t InpRegs[0xFA];
}READ_INPUT_REGS_RESP;

typedef union __attribute__ ((__packed__)) tagReadInputRegs
{
	READ_INPUT_REGS_REQ request;
	READ_INPUT_REGS_RESP response;
}READ_INPUT_REGS;

typedef struct __attribute__ ((__packed__)) tagSlaveID
{
	uint8_t function_code;
	uint8_t count;
	uint8_t ID;
	uint8_t RunStatus;
	uint8_t ExtraData[249];
}SLAVE_ID;

typedef struct __attribute__ ((__packed__)) tagSlaveIDExt
{
	uint8_t function_code;
	uint8_t count;
	uint8_t ID;
	uint8_t RunStatus;
	uint16_t version;
	uint8_t MKStatus;
	uint8_t reserved;
}SLAVE_ID_EXT;

typedef struct __attribute__ ((__packed__)) tagFileRecordRequest
{
	uint8_t function_code;
	uint8_t count;
	uint8_t ref_type;
	uint8_t FileNumberHi;
	uint8_t FileNumberLo;
	uint8_t RecordNumberHi;
	uint8_t RecordNumberLo;
	uint8_t RegLengthHi;
	uint8_t RegLengthLo;
}FILE_RECORD_REQUEST;

typedef struct __attribute__ ((__packed__)) tagFileRecordResponse
{
	uint8_t function_code;
	uint8_t RespDataLength;
	uint8_t FileRespLength;
	uint8_t ref_type;
	uint8_t data[0xF5];
}FILE_RECORD_RESPONSE;

typedef union __attribute__ ((__packed__)) tagFileRecord
{
	FILE_RECORD_REQUEST request;
	FILE_RECORD_RESPONSE response;
}FILE_RECORD;

typedef struct __attribute__ ((__packed__)) tagMemoryOperate
{
	uint8_t function_code;
	uint8_t device;
	unsigned long adress;
	uint8_t count;
	uint8_t data[0xF7];
}MEMORY_OPERATE;

// Запись параметров устройства
typedef struct __attribute__ ((__packed__)) tagWriteOption
{
	uint8_t function_code;
	uint8_t mode;
	uint8_t ID0;
	uint8_t ID1;
	unsigned long boudrate0;
	unsigned long boudrate1;
}WRITE_OPTION;

#if _MB_WR_S_REG_FUNC_
	int MBCWriteSingleRegister(WRITE_SINGLE_REGISTER* regTask, uint8_t *size);
#endif
#if _MB_RD_IN_REG_FUNC_
	int MBCReadInputRegisters(READ_INPUT_REGS *regTask, uint8_t *size);
#endif
#if _MB_WR_FILE_FUNC_
	int MBCWriteFile(WRITE_FILE * regTask, uint8_t *size);
#endif
#if _MB_RD_FILE_FUNC_
	int MBCReadFile(READ_FILE * regTask, uint8_t *size);
#endif

#endif
#endif

