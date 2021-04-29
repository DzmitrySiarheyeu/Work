#include "Config.h"
#if (_MB_CLIENT_)
#include "MB_client.h"
#include "User_ModBus.h"

#if _MB_WR_S_REG_FUNC_
int WriteSingleRegisterProc(uint16_t requestAddress, uint16_t ValueRegister, uint8_t *size);
//функция 0х06 запись во внутренние "регистры" устройства
int MBCWriteSingleRegister(WRITE_SINGLE_REGISTER* regTask, uint8_t *size)
{
    unsigned short requestAddress;
    unsigned short ValueRegister;

    requestAddress = ((unsigned short)(regTask->st_adr_hi << 8))|((unsigned short)(regTask->st_adr_lo));
    ValueRegister = ((unsigned short)(regTask->value_lo << 8))|((unsigned short)(regTask->value_hi));

	return WriteSingleRegisterProc(requestAddress, ValueRegister, size);
}

#endif  //  _MB_WR_S_REG_FUNC_


#if _MB_RD_IN_REG_FUNC_
int ReadInputRegistersProc(uint16_t requestAddress, uint16_t requestRegister, char *buf, uint8_t *size);
//функция 0х04 чтение внутренних "регистров" устройства
int MBCReadInputRegisters(READ_INPUT_REGS *regTask, uint8_t *size)
{
    unsigned short requestAddress;
    unsigned short requestRegister;

    requestAddress = ((unsigned short)(regTask->request.st_adr_hi << 8))|((unsigned short)(regTask->request.st_adr_lo));
    requestRegister = ((unsigned short)(regTask->request.quantity_hi << 8))|((unsigned short)(regTask->request.quantity_lo));
    regTask->response.count = (uint8_t)(requestRegister * 2);


	return ReadInputRegistersProc(requestAddress, requestRegister, (char *)regTask->response.InpRegs, size);
}
#endif  //_MB_RD_IN_REG_FUNC_


#if _MB_WR_FILE_FUNC_
int WriteFileProc(uint16_t record_number, uint16_t record_length, char *buf);
int MBCWriteFile(WRITE_FILE * regTask, uint8_t *size)
{
    uint16_t record_number;
    uint16_t record_length;

    record_number = ((uint16_t)(regTask->RecordNumberHi << 8))|((uint16_t)(regTask->RecordNumberLo));
    record_length = ((uint16_t)(regTask->RegLengthHi << 8))|((uint16_t)(regTask->RegLengthLo));

    if(regTask->count != *size - 2) 
    {
		return ILLEGAL_DATA_VALUE;
    }

    if(regTask->ref_type != 6) 
    {
        return ILLEGAL_DATA_VALUE;
    }

	*size = (regTask->count + 2);

   	return WriteFileProc(record_number, record_length, (char *)regTask->InpRegs);

}
#endif  //_MB_WR_FILE_FUNC_



#if _MB_RD_FILE_FUNC_
int ReadFileProc(uint16_t record_number, uint16_t record_length, char *buf);
int MBCReadFile(READ_FILE * regTask, uint8_t *size)
{
    uint16_t record_number;
    uint16_t record_length;

    record_number = ((uint16_t)(regTask->read_file_req.RecordNumberHi << 8))|((uint16_t)(regTask->read_file_req.RecordNumberLo)); // это смещение в байтах отн. начала файла
    record_length = ((uint16_t)(regTask->read_file_req.RegLengthHi << 8))|((uint16_t)(regTask->read_file_req.RegLengthLo));	      // это длина файла в полусловах

    if(regTask->read_file_req.count != *size - 2) 
    {
        return ILLEGAL_DATA_VALUE;	
    }

    if(regTask->read_file_req.ref_type != 6) 
    {
        return ILLEGAL_DATA_VALUE;
    }

    regTask->read_file_resp.function_code = 0x14;
    regTask->read_file_resp.file_length = (uint8_t)record_length * 2 + 1;
    regTask->read_file_resp.count = 1 + regTask->read_file_resp.file_length;
    regTask->read_file_resp.ref_type = 6;

	*size = (regTask->read_file_resp.count + 2);

	return ReadFileProc(record_number, record_length, (char *)regTask->read_file_resp.InpRegs);
}

#endif //  	_MB_RD_FILE_FUNC_

#endif
