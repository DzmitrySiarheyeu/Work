#include "Config.h"
#if (_MB_SERIAL_ && _MB_SERIAL_CLIENT_)
#include <stdint.h>
#include "MB_client.h"
#include "MB_serial_client.h"
#include "MB_serial_port.h"

#if _MB_DIAGNOSTIC_FUNC_
// Обработка ф-ции 0x08 - диагностика
int MBCDiagnosticModbus(uint8_t *buffer, uint8_t *size)
{
	uint8_t i;
	//Если код подф-ции возвращения какого-то счётчика
	if ((buffer[2] >= BUS_MESSAGE_COUNT) && (buffer[2] <= BUS_CHARRACTER_OVERRUN_COUNT))
	{
		buffer[3] = GetErrCounterHigh(buffer[2] - 0x0A);	//вычисляем и возвращаем этот счётчик
		buffer[4] = GetErrCounterLow(buffer[2] - 0x0A);
		*size = 5;								//установка размера сообщения
		return;
	}
	switch(buffer[2])
	{
		case CLEAR_COUNTERS:					//подф-ция очистки счётчиков
			ClearCounters();
			break;
		case RETURN_QUERY_DATA:					//подф-ция ЭХО
			break;								//ничего не делаем
		default:										//подф-ция неизвестна 
			MBSerialErrorResponse(ILLEGAL_FUNCTION);	//Послать соответствующее исключение
			return;
	}
}
#endif //_MB_DIAGNOSTIC_FUNC_


void MBClientProcess(MB_SER_PORT_INFO *mbInfo, uint8_t *buffer, uint8_t size, uint8_t id)
{
	int err = 0;
	switch(buffer[0])
	{
#if _MB_DIAGNOSTIC_FUNC_
		case MOD_DIAGNOSTIC:
			err = MBCDiagnosticModbus(buffer, &size);
			break;
#endif
#if _MB_RD_IN_REG_FUNC_
		case MOD_READ_INPUT_REGISTER:
			err = MBCReadInputRegisters((READ_INPUT_REGS *)buffer, &size);	
			break;
#endif
#if _MB_WR_S_REG_FUNC_
		case MOD_WRITE_SINGLE_REGISTER:
			err = MBCWriteSingleRegister((WRITE_SINGLE_REGISTER*)buffer, &size);
			break;
#endif
#if	_MB_WR_FILE_FUNC_
        case MOD_WRITE_FILE :
             err = MBCWriteFile((WRITE_FILE *)buffer, &size);
             break;
#endif
#if	_MB_RD_FILE_FUNC_
       case MOD_READ_FILE_RECORD :
             err = MBCReadFile((READ_FILE *)buffer, &size);
			 break;
#endif
		default:
			size = MBSerialErrorResponse(mbInfo, ILLEGAL_FUNCTION);
                        break;
	}

	if(err != MB_NO_ERROR)
		size = MBSerialErrorResponse(mbInfo, err);
	MBSerialSend(mbInfo, size, id);
}

#endif
