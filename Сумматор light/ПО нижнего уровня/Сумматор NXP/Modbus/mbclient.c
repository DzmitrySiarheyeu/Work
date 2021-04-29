#include <string.h>
#include <stdint.h>

#include "mbfanction0.h"
#include "mbclient.h"
#include "uart.h"
#include "omnicomm.h"
#include "reply.h"
#include "settings.h"

unsigned char CheckRegisterValue(unsigned short address, unsigned short value)
{
    unsigned short regAddres;
	unsigned short regValue;
	
	unsigned short i;

	regAddres = (unsigned char*)&RegFile.mb_struct.dot_info.N - (unsigned char*)&RegFile;
    regValue = Reverse2(value);
	
	// проверяем запись значения N - количестов ДОТов
	if (address * 2 == regAddres && (regValue == 0 || regValue > MAX_DOT_COUNT))
    {
    	return false;
    }
    
	// проверяем запись параметра - размер таблицы тприровки ДОТа
    for(i = 0; i < Reverse2(RegFile.mb_struct.dot_info.N); i ++)
    {
        regValue = Reverse2(value);
		regAddres = (unsigned char *)& RegFile.mb_struct.dot_info.DOT[i].N - (unsigned char*)&RegFile;
		if (address * 2  == regAddres && (regValue < 2 || regValue > DOT_PAR_COUNT))
        {
			return false;
        }
    }

    return true;
}

__asm void boot(void)
{
	ldr r0, =0x30
	ldr r0, [r0] 
	mov pc, r0
}

void modWriteSingleRegister(WRITE_SINGLE_REGISTER* regTask)
{
	unsigned short requestAddress;
	unsigned short requestAddressBase;
    unsigned short ValueRegister;

	requestAddressBase = ((unsigned short)(regTask->st_adr_hi << 8))|((unsigned short)(regTask->st_adr_lo));

	ValueRegister = ( ((unsigned short)regTask->value_lo << 8)|(((unsigned short)regTask->value_hi) ));

	if(requestAddressBase == 1019)
	{
		if(Reverse2(ValueRegister) == 123)
			boot();	
	}

	if(requestAddressBase <  sizeof(REGISTER_FILE))
	{
		requestAddress = requestAddressBase;
		if(requestAddress >= sizeof(REGISTER_FILE)/2) // Значит хотим писать только в ОЗУ, без записи в EEPROM
			requestAddress = requestAddressBase - sizeof(REGISTER_FILE)/2;
		if (!CheckRegisterValue(requestAddress, ValueRegister))
        {
            SendErrorResponse(ILLEGAL_DATA_VALUE);
        }
        else
        {
			if(requestAddressBase >= sizeof(REGISTER_FILE)/2)
			{
				memcpy(&RegFile.mb_array[requestAddress], &ValueRegister, sizeof(ValueRegister));
				RegFile.mb_struct.U_type = VERSION_PO;
			}
			else
			{
				if(ValueRegister != 0)
					ValueRegister = ValueRegister;
				FL_REGISTER_STRUCT *pROMFile = GetFlashActualData();
				memcpy((char *)&(((uint16_t *)pROMFile)[requestAddress]), (char *)&ValueRegister, sizeof(ValueRegister));
				WriteFlash(pROMFile);
				//memcpy((char *)&RegFile, (char *)pFlashRegFile, sizeof(REGISTER_FILE));
				memcpy((char *)&RegFile.mb_array[requestAddress], &(((uint16_t *)pFlashRegFile)[requestAddress]), sizeof(ValueRegister));
            	//eeprom_write_block(&ValueRegister, &EepromRegFile.mb_array[requestAddress], sizeof(ValueRegister));
				//eeprom_read_block(&RegFile.mb_array[requestAddress], &EepromRegFile.mb_array[requestAddress], sizeof(ValueRegister));
				RegFile.mb_struct.U_type = VERSION_PO;
			}
		    MBClientSend(sizeof(WRITE_SINGLE_REGISTER));
        }
	}
	else 
    {
		SendErrorResponse(ILLEGAL_DATA_ADRESS);
	}
}

//функция 0х04 чтение внутренних "регистров" устройства
void modReadInputRegisters(READ_INPUT_REGS *regTask)
{
	unsigned short requestAddress;
	unsigned short requestRegister;

	requestAddress = ((unsigned short)(regTask->request.st_adr_hi << 8))|((unsigned short)(regTask->request.st_adr_lo));
	requestRegister = ((unsigned short)(regTask->request.quantity_hi << 8))|((unsigned short)(regTask->request.quantity_lo));
	regTask->response.count = (byte)(requestRegister * 2);

	if(requestAddress <  sizeof(REGISTER_FILE)/2)
	{
		if((requestAddress + requestRegister) <= (sizeof(REGISTER_FILE)/2))
		{
			memcpy(regTask->response.InpRegs,&(RegFile.mb_array[requestAddress]), regTask->response.count);
			MBClientSend(2+regTask->response.count);
		}
		else {
				SendErrorResponse(ILLEGAL_DATA_VALUE);	
			 }
	} 
	else {
			SendErrorResponse(ILLEGAL_DATA_ADRESS);
		 }
}

void WriteFlashMB(MEMORY_OPERATE *buffer)
{
	uint32_t addr = buffer->adress - FL_ADDR_START;
	uint32_t len = 0;
	if(addr > sizeof(FL_REGISTER_STRUCT))
	{
		SendErrorResponse(ILLEGAL_DATA_ADRESS);
		return;
	}
//	boot_program_page(buffer->adress, buffer->data, buffer->count);		

	FL_REGISTER_STRUCT *pROMFile = GetFlashActualData();
	if((addr + buffer->count) > sizeof(DOT_TABLE) * MAX_DOT_COUNT)
		len = (addr + buffer->count) - (sizeof(DOT_TABLE) * MAX_DOT_COUNT);
	else len = buffer->count;
	memcpy((char *)&(((uint8_t *)pROMFile->ProgramDotTable)[addr]), buffer->data, len);
	WriteFlash(pROMFile);

	MBClientSend(7);
}

//Чтение памяти


void MBReadMemory(MEMORY_OPERATE *p)
{
	uint32_t addr = p->adress;
	//если хотят считать много данных, то это плохо
	if(p->count > 245)
	{
		SendErrorResponse(ILLEGAL_DATA_VALUE);
		return;
	}
	//memcpy((char *)&addr, (char *)&p->adress, 4);
/*	((char *)&addr)[0] = ((char *)&p->adress)[0];
	((char *)&addr)[1] = ((char *)&p->adress)[1];
	((char *)&addr)[2] = ((char *)&p->adress)[2];
	((char *)&addr)[3] = ((char *)&p->adress)[3];  */
	switch(p->device)
	{
	case 0:
		addr -= FL_ADDR_START;
		//тут считываем из еепром, только если последний адрес < 512, больше в меге168 нет
		if (addr + p->count > (sizeof(DOT_TABLE) * MAX_DOT_COUNT + 256))
		{
			//тут все плохо
			SendErrorResponse(ILLEGAL_DATA_ADRESS);
			return;
		}
		//собственно считываем
		memcpy(p->data, (void *)((unsigned int)addr + (char *)ProgramDotTable), p->count);
	
		break;
//	case 1:
//		//тут считываем из еепром, только если последний адрес < 512, больше в меге168 нет
//		if (addr + p->count > 512)
//		{
//			//тут все плохо
//			SendErrorResponse(ILLEGAL_DATA_ADRESS);
//			return;
//		}
//		//собственно считываем
//		memcpy(p->data, (void *)((unsigned int)(addr + TABLE_START_ADDRESS)), p->count);
//		break;
	default:
		//другой памяти у нас нет
		SendErrorResponse(ILLEGAL_DATA_VALUE);
		return;
	}
	//Посылаем прочитанное
	MBClientSend(p->count + 7);			
}

void modWriteOption(WRITE_OPTION *buffer)
{
	unsigned long temp_Value0;

	*((unsigned char *)&temp_Value0    ) = *((unsigned char *)&buffer->boudrate0 + 3);
	*((unsigned char *)&temp_Value0 + 1) = *((unsigned char *)&buffer->boudrate0 + 2);
	*((unsigned char *)&temp_Value0 + 2) = *((unsigned char *)&buffer->boudrate0 + 1);
	*((unsigned char *)&temp_Value0 + 3) = *((unsigned char *)&buffer->boudrate0    );
	switch(temp_Value0)
	{
		case 2400: 	
		case 4800:
		case 9600:
		case 14400:
		case 19200:
		case 28800:
		case 38400:
		case 57600:
		case 76800:
		case 115200:	
			break;
		default:SendErrorResponse(ILLEGAL_DATA_VALUE); 
				return;
	}

	if(((buffer->mode) < 2) && (buffer->ID0 > 0) && (buffer->ID1 > 0))
	{
		FL_REGISTER_STRUCT *pROMFile = GetFlashActualData();
		pROMFile->ID = buffer->ID0;
		pROMFile->boudrate = temp_Value0;
		WriteFlash(pROMFile);
		//eeprom_write_byte((void*)&EEP_ID0, buffer->ID0);
		//eeprom_write_block(&temp_Value0, (void*)&EEP_Boudrate0, sizeof(temp_Value0));		
	}
	else {
		SendErrorResponse(ILLEGAL_DATA_VALUE); return;
	}

	MBClientSend(sizeof(WRITE_OPTION));

	UARTInit(pFlashRegFile->boudrate);	
}


void MBClientProcess(byte *buffer,byte Size)
{
	switch(buffer[0])
	{
		case MOD_READ_INPUT_REGISTER:
			modReadInputRegisters((READ_INPUT_REGS *)buffer);	
			break;
		case MOD_WRITE_SINGLE_REGISTER:
			modWriteSingleRegister((WRITE_SINGLE_REGISTER*)buffer);
			break;
		case MOD_READ_FLASH:
			MBReadMemory((MEMORY_OPERATE *)buffer);
			break;
		case MOD_WRITE_FLASH :
			WriteFlashMB((MEMORY_OPERATE *)buffer);
			break;	
		case MOD_WRITE_OPTION :
			modWriteOption((WRITE_OPTION *)buffer);
			break;
        case MOD_DIAGNOSTIC:
			DiagnosticModbus0(buffer,Size);
			break;

        default:
			SendErrorResponse(ILLEGAL_FUNCTION);
	}
}

void ModbusProcess()
{
	int packet_size;

    if (!RegFile.mb_struct.Configured || RegFile.mb_struct.dot_info.fl_freq)
	{
		packet_size = MBClientReceive();
	
		if(packet_size)
		{
			if(check_modbus_packet(MBClientGetHostID0(), packet_size))
			{
				omn_param.e2_report_on = 0;  // отключаем омникомовский репит
				MBClientProcess(UartBuff + 1, packet_size);
				//UARTResetRxBuf();
			}
		}


		do_omnicom(packet_size);
		
		if(packet_size)
			UARTResetRxBuf();					
	}
}
