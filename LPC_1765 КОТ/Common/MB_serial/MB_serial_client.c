#include "Config.h"
#if (_MB_SERIAL_ && _MB_SERIAL_CLIENT_)
#include <stdint.h>
#include "MB_client.h"
#include "MB_serial_client.h"
#include "MB_serial_port.h"

#if _MB_DIAGNOSTIC_FUNC_
// ��������� �-��� 0x08 - �����������
int MBCDiagnosticModbus(uint8_t *buffer, uint8_t *size)
{
	uint8_t i;
	//���� ��� ����-��� ����������� ������-�� ��������
	if ((buffer[2] >= BUS_MESSAGE_COUNT) && (buffer[2] <= BUS_CHARRACTER_OVERRUN_COUNT))
	{
		buffer[3] = GetErrCounterHigh(buffer[2] - 0x0A);	//��������� � ���������� ���� �������
		buffer[4] = GetErrCounterLow(buffer[2] - 0x0A);
		*size = 5;								//��������� ������� ���������
		return;
	}
	switch(buffer[2])
	{
		case CLEAR_COUNTERS:					//����-��� ������� ���������
			ClearCounters();
			break;
		case RETURN_QUERY_DATA:					//����-��� ���
			break;								//������ �� ������
		default:										//����-��� ���������� 
			MBSerialErrorResponse(ILLEGAL_FUNCTION);	//������� ��������������� ����������
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
