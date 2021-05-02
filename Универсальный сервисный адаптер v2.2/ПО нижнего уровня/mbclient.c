#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "mbfanction0.h"
#include <stdint.h>
#include <avr/wdt.h>
#include <string.h>
#include "mbclient.h"
#include "usb_adapt.h"
#include "ADC.h"
#include "loader.h"
#include "util.h"


//void SET_DE485_0(void){/* cbi(UCSR0B, RXEN0); sbi(DE485_0_PORT, DE485_0_PIN);*/ }
//void CLR_DE485_0(void){/* cbi(DE485_0_PORT, DE485_0_PIN); sbi(UCSR0B, RXEN0); */}


//функция 0х06 запись во внутренние "регистры" устройства
void modWriteSingleRegister(WRITE_SINGLE_REGISTER* regTask)
{
	
	unsigned short requestAddress;
	unsigned short ValueRegister;

	requestAddress = ((uint16_t)(regTask->st_adr_hi << 8))|((uint16_t)(regTask->st_adr_lo));
	ValueRegister = ((uint16_t)(regTask->value_lo << 8))|((uint16_t)(regTask->value_hi));

	if(requestAddress <  sizeof(MBDIO_REGISTR)/2)
	{
		if(PassCheck(regTask->st_adr_lo) == 0)
		{
			if(regTask->st_adr_lo != BT_IMP_CNT)
				MB_REG.DIO_array[requestAddress] = ValueRegister;
			if(Mod_process(regTask->st_adr_lo) == 0)
				MBClientSend(5);
			else SendErrorResponse(ILLEGAL_DATA_VALUE);
		}
		else SendErrorResponse(ILLEGAL_DATA_VALUE);
		
	}
	else if(requestAddress == 0x4000 && ValueRegister == 0x0038)
	{
		MBClientSend(sizeof(WRITE_SINGLE_REGISTER));
		loader();
	} 
	else {
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
	regTask->response.count = (uint8_t)(requestRegister * 2);

	if(requestAddress <  sizeof(MBDIO_REGISTR)/2)
	{
		if((requestAddress + requestRegister) <= (sizeof(MBDIO_REGISTR)/2))
		{

			memcpy((regTask->response.InpRegs),&(MB_REG.DIO_array[requestAddress]), regTask->response.count);
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



//Чтение памяти

/*
void MBReadMemory(MEMORY_OPERATE *p)
{
	FLASH_FILE f;
	//если хотят считать много данных, то это плохо
	if(p->count > 245)
	{
		SendErrorResponse(ILLEGAL_DATA_VALUE);
		return;
	}
	switch(p->device)
	{
	case 1:
		//тут считываем из еепром, только если последний адрес < 512, больше в меге168 нет
		if (p->adress + p->count > 512)
		{
			//тут все плохо
			SendErrorResponse(ILLEGAL_DATA_ADRESS);
			return;
		}
		//собственно считываем
		eeprom_read_safe(p->data, (void *)((unsigned int)p->adress), p->count);
		break;
	case 2:
		//тут считываем из рам, а рам в меге только 1024
		if (p->adress + p->count > 25000)
		{
			//все плохо
			SendErrorResponse(ILLEGAL_DATA_ADRESS);
			return;
		}
		//собственно читаем
		f.m_Address = p->adress;
		FlashRead(&f, p->data, p->count);
		break;
	case 0:
		//тут считываем из рам, а рам в меге только 1024
		if (p->adress + p->count > 1024)
		{
			//все плохо
			SendErrorResponse(ILLEGAL_DATA_ADRESS);
			return;
		}
		//собственно читаем
		memmove(p->data, (void *)((unsigned int)p->adress), p->count);
		break;
	default:
		//другой памяти у нас нет
		SendErrorResponse(ILLEGAL_DATA_VALUE);
		return;
	}
	//Посылаем прочитанное
	MBClientSend(p->count + 7);	
	Write_mod =0;	
}

*/


void modWriteOption(WRITE_OPTION *buffer)
{
	unsigned long ValueBoudrate;
	//unsigned long ValueFREQ;
	*((unsigned char *)&ValueBoudrate    ) = *((unsigned char *)&buffer->boudrate0 + 3);
	*((unsigned char *)&ValueBoudrate + 1) = *((unsigned char *)&buffer->boudrate0 + 2);
	*((unsigned char *)&ValueBoudrate + 2) = *((unsigned char *)&buffer->boudrate0 + 1);
	*((unsigned char *)&ValueBoudrate + 3) = *((unsigned char *)&buffer->boudrate0    );
	switch(ValueBoudrate)
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
	
	if((buffer->ID0 > 0) )
	{
	//	eeprom_write_byte((void*)&EEP_ID, buffer->ID0);
	//	eeprom_write_block(&ValueBoudrate, (void*)&EEP_Boudrate, sizeof(ValueBoudrate));	
		
	}
		else {SendErrorResponse(ILLEGAL_DATA_VALUE); return;}

	MBClientSend(sizeof(WRITE_OPTION));

	InitUART();	
}


void MBClientProcess(uint8_t *buffer,uint8_t Size)
{
	switch(buffer[0])
	{
		case MOD_DIAGNOSTIC:
			DiagnosticModbus0(buffer,Size);
			break;
		case MOD_READ_INPUT_REGISTER:
			modReadInputRegisters((READ_INPUT_REGS *)buffer);	
			break;
		case MOD_WRITE_SINGLE_REGISTER:
			modWriteSingleRegister((WRITE_SINGLE_REGISTER*)buffer);
			break;
		case MOD_WRITE_MULTIPLE_REGISTERS:
			//modWriteMultipleRegister((WRITE_MULTIPLE_REGS*)buffer);
			SendErrorResponse(ILLEGAL_FUNCTION);
			break;
	//	case MOD_READ_FLASH:
	//		MBReadMemory((MEMORY_OPERATE *)buffer);
	//		break;
		case MOD_WRITE_OPTION :
			modWriteOption((WRITE_OPTION *)buffer);
			break; 
		//case MOD_WRITE_FLASH :
		//	modWriteFlash((MEMORY_OPERATE *)buffer);
			//break;
		default:
			SendErrorResponse(ILLEGAL_FUNCTION);
	}
}



