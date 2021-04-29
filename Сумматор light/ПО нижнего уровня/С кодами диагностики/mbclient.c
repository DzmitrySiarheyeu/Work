#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/boot.h>
#include <string.h>

#include "util.h"
#include "settings.h"

#ifdef __MODBUS__

#include "mbfanction0.h"
#include "mbclient.h"
#include "iit_boot.h"
#include "usart.h"
#include "omnicomm.h"

#define MAX_BOOT_PAGE 133

void boot_program_page (uint32_t page, uint8_t *buf, uint8_t count) BOOTLOADER_SECTION;
void boot_program_page (uint32_t page, uint8_t *buf, uint8_t count)	
    {
        uint16_t i;
        uint8_t sreg;
		uint16_t w;

        // Disable interrupts.

        sreg = SREG;
        cli();
    
        eeprom_busy_wait ();
	
        boot_page_erase (page);
        boot_spm_busy_wait ();      // Wait until the memory is erased.

        for (i=0; i<SPM_PAGESIZE; i+=2)
        {
           	w = *buf++;
           	w += (*buf++) << 8;
		    boot_page_fill (page + i, w);
		}
        boot_page_write (page);     // Store buffer in flash page.
        boot_spm_busy_wait();       // Wait until the memory is written.

        // Reenable RWW-section again. We need this if we want to jump back
        // to the application after bootloading.

        boot_rww_enable ();

        // Re-enable interrupts (if they were ever enabled).

        SREG = sreg;
    }

inline unsigned char CheckRegisterValue(unsigned short address, unsigned short value)
{
    unsigned short regAddres;
	unsigned short regValue;
	
	unsigned short i;

	regAddres = (unsigned char*)&RegFile.mb_struct.dot_info.N - (unsigned char*)&RegFile;
    regValue = Reverse2(value);
	
	if (address * 2 == regAddres && (regValue == 0 || regValue > MAX_DOT_COUNT))
    {
    	return false;
    }
    
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

void modWriteSingleRegister(WRITE_SINGLE_REGISTER* regTask)
{
	unsigned short requestAddress;
	unsigned short requestAddressBase;
    unsigned short ValueRegister;

	requestAddressBase = ((unsigned short)(regTask->st_adr_hi << 8))|((unsigned short)(regTask->st_adr_lo));

	ValueRegister = ( ((unsigned short)regTask->value_lo << 8)|(((unsigned short)regTask->value_hi) ));

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
            	eeprom_write_block(&ValueRegister, &EepromRegFile.mb_array[requestAddress], sizeof(ValueRegister));
				eeprom_read_block(&RegFile.mb_array[requestAddress], &EepromRegFile.mb_array[requestAddress], sizeof(ValueRegister));
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

void WriteFlash(MEMORY_OPERATE *buffer)
{
	if(buffer->adress > MAX_BOOT_PAGE * SPM_PAGESIZE)
	{
		SendErrorResponse(ILLEGAL_DATA_ADRESS);
		return;
	}
	boot_program_page(buffer->adress, buffer->data, buffer->count);		

	MBClientSend(7);
}

//Чтение памяти
void MBReadMemory(MEMORY_OPERATE *p)
{
	//если хотят считать много данных, то это плохо
	if(p->count > 245)
	{
		SendErrorResponse(ILLEGAL_DATA_VALUE);
		return;
	}
	switch(p->device)
	{
	case 0:
		//тут считываем из еепром, только если последний адрес < 512, больше в меге168 нет
		if (p->adress + p->count > (SPM_PAGESIZE * MAX_BOOT_PAGE))
		{
			//тут все плохо
			SendErrorResponse(ILLEGAL_DATA_ADRESS);
			return;
		}
		//собственно считываем
		memcpy_P(p->data, (void *)((unsigned int)p->adress), p->count);
	
		break;
	case 1:
		//тут считываем из еепром, только если последний адрес < 512, больше в меге168 нет
		if (p->adress + p->count > 512)
		{
			//тут все плохо
			SendErrorResponse(ILLEGAL_DATA_ADRESS);
			return;
		}
		//собственно считываем
		eeprom_read_block(p->data, (void *)((unsigned int)p->adress), p->count);
	
		break;
/*	case 2:
		//тут считываем из рам, а рам в меге только 1024
		if (p->adress + p->count > 1024)
		{
			//все плохо
			SendErrorResponse(ILLEGAL_DATA_ADRESS);
			return;
		}
		//собственно читаем
		memmove(p->data, (void *)((unsigned int)p->adress), p->count);
	
		break;*/
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
			break;
	}

	if(((buffer->mode) < 2) && (buffer->ID0 > 0) && (buffer->ID1 > 0))
	{
		eeprom_write_byte((void*)&EEP_ID0, buffer->ID0);
		eeprom_write_block(&temp_Value0, (void*)&EEP_Boudrate0, sizeof(temp_Value0));		
	}
	else {
		SendErrorResponse(ILLEGAL_DATA_VALUE); return;
	}

	MBClientSend(sizeof(WRITE_OPTION));

	InitUART();	
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
			WriteFlash((MEMORY_OPERATE *)buffer);
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

inline void ModbusProcess()
{
	unsigned char packet_size;

    if (!RegFile.mb_struct.Configured || RegFile.mb_struct.dot_info.fl_freq)
	{
		packet_size = UartRecivePacket();
	
		if(packet_size)
		{
			if(check_modbus_packet(MBClientGetHostID0()))
			{
				report_on = 0;  // отключаем омникомовский репит
				MBClientProcess(DataBuffer0 + 1, packet_size);
			}
			//do_omnicom();
		}
		do_omnicom();				
	
}

#endif
}
