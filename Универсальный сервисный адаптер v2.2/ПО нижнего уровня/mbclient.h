#ifndef __MBCLIENT_H__
#define __MBCLIENT_H__
/*
#define DE485_0_PORT	PORTD
#define DE485_0_PIN		2
#define DE485_0_DDR   	DDRD
*/

void MBClientProcess(uint8_t *buffer,uint8_t Size);

typedef struct tagWriteMultipleRegisterRequest
{
	uint8_t function_code;
	uint8_t st_adr_hi;
	uint8_t st_adr_lo;
	uint8_t quantity_hi;
	uint8_t quantity_lo;
	uint8_t count;
	uint8_t InpRegs[0xFA];
}WRITE_MULTIPLE_REGS_REQ;

typedef struct tagWriteMultipleRegisterRest
{
	uint8_t function_code;
	uint8_t st_adr_hi;
	uint8_t st_adr_lo;
	uint8_t quantity_hi;
	uint8_t quantity_lo;
}WRITE_MULTIPLE_REGS_RESP;

typedef union tagWriteMultipleRegister
{
	WRITE_MULTIPLE_REGS_REQ request;
	WRITE_MULTIPLE_REGS_RESP response;
}WRITE_MULTIPLE_REGS;

extern uint8_t Reserv;
extern int ConvertFormatForInt(int Value);
#define MAX_BOOT_PAGE 		0x64

#endif
